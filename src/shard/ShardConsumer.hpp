#ifndef SHARD_CONSUMER_H_
#define SHARD_CONSUMER_H_

#include <immintrin.h> // For _mm_pause
#include <thread>
#include <condition_variable>
#include <memory>
#include <atomic>

#include <ThreadConstants.h>
#include <thread_utils.hpp>
#include <time_utils.hpp>
#include <queue_wrapper.h>
#include <Message.hpp>

#include <PerStockLedger.h>
#include <PerStockOrderState.h>
#include <PerStockVWAPPrefixes.hpp>
#include <PerStockVWAP.hpp>

#include <PerStockTWAPPrefixes.hpp>

#include <SystemEventSequence.h>

#include <iostream>
// Global counter tracking how many messages consumers have processed
// inline std::atomic<uint64_t> numMessages{0};

struct PerStockState {
    std::unique_ptr<PerStockOrderState> orderState;
    std::unique_ptr<PerStockLedger> ledger;

    PerStockVWAPPrefixHistory vwapPrefixes;
    PerStockVWAP vwap;

    PerStockTWAPPrefixHistory twapPrefixes;
    PerStockTWAP twap;

    std::optional<VWAPIntervalQueryResult> queryVWAPInterval(uint64_t startNs, uint64_t endNs) {
        return vwapPrefixes.queryInterval(startNs, endNs);
    }

    std::optional<VWAPIntervalQueryResult> queryLastNMinutesVWAP(uint64_t nowNs, uint64_t minutes) {
        vwapPrefixes.maybeAdvanceVWAPBucket(nowNs, vwap);
        uint64_t startNs = nowNs - (minutes * VWAPConstants::NANOSECOND_PER_MINUTE);
        return queryVWAPInterval(startNs, nowNs);
    } 
    
    std::optional<TWAPIntervalQueryResult> queryLastNMniutesTWAP(uint64_t nowNs, uint64_t minutes) {
        twapPrefixes.maybeAdvanceTWAPBucket(nowNs, twap);
        PerStockTWAP asOfTWAPCopy = twap;
        accumulateTWAP(asOfTWAPCopy, asOfTWAPCopy.lastPrice, nowNs);
        uint64_t startNs = nowNs - (minutes * TWAPConstants::NANOSECOND_PER_MINUTE);
        return twapPrefixes.queryAsOfInterval(startNs, nowNs, asOfTWAPCopy);
    }

};

using LocateIndexedPerStockState = std::array<PerStockState, PerStockOrderStateConstants::NUM_OF_STOCK_LOCATE>;
using Symbols = std::array<char[MessageFieldSizes::STOCK_SIZE], PerStockOrderStateConstants::NUM_OF_STOCK_LOCATE>;
using Locates = ankerl::unordered_dense::map<std::string, uint16_t>;

class ShardConsumer {

    private:

        void backoff() {
            // Step 1: Busy wait
            for(auto i = 0; i < ThreadConstants::BUSY_WAIT_CYCLES; ++i) {
                _mm_pause();
                if(!(*_queue).buffer.empty()) return;
            }
            
            // Step 2: thread yield
            for(auto i = 0; i < ThreadConstants::YIELD_CYCLES; ++i) {
                std::this_thread::yield();
                if(!(*_queue).buffer.empty()) return;
            }

            // Step 3: Condvar sleep 
            // _cv.wait();
        }

        MessageBody parseMessage(const char messageType, const uint16_t stockLocate, const char buffer[MAX_BUFFER_SIZE]) {
            switch (messageType) {
                case MessageTypes::MESSAGE_TYPE_ADD_ORDER_NO_MPID:
                return MessageBody {
                    .tag = MessageTag::AddOrder,
                    .addOrder = parseAddOrderBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_ADD_ORDER_WITH_MPID:
                return MessageBody {
                    .tag = MessageTag::AddOrderWithMPID, 
                    .addOrderWithMPID = parseAddOrderWithMPIDBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_TRADE_BROKEN:
                return MessageBody {
                    .tag = MessageTag::BrokenTradeOrOrderExecution,
                    .brokenTradeOrOrderExecution = parseBrokenTradeOrOrderExecutionBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_ORDER_CANCELLED:
                return MessageBody {
                    .tag = MessageTag::OrderCancel,
                    .orderCancel = parseOrderCancelBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_ORDER_DELETE:
                return MessageBody {
                    .tag = MessageTag::OrderDelete,
                    .orderDelete = parseOrderDeleteBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_ORDER_EXECUTED:
                return MessageBody {
                    .tag = MessageTag::OrderExecuted,
                    .orderExecuted = parseOrderExecutedBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_ORDER_EXECUTED_WITH_PRICE:
                return MessageBody {
                    .tag = MessageTag::OrderExecutedWithPrice ,
                    .orderExecutedWithPrice = parseOrderExecutedWithPriceBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_ORDER_REPLACE:
                return MessageBody {
                    .tag = MessageTag::OrderReplace,
                    .orderReplace = parseOrderReplaceBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_STOCK_DIRECTORY:
                {
                    // Initialize this stock locate 
                    (*_locateIndexedPerStockState)[stockLocate].orderState = std::make_unique<PerStockOrderState>();
                    (*_locateIndexedPerStockState)[stockLocate].ledger = std::make_unique<PerStockLedger>();
                    auto message = MessageBody {
                        .tag = MessageTag::StockDirectory,
                        .stockDirectory = parseStockDirectoryBody((*_symbols)[stockLocate], buffer)
                    };
                    std::string name = std::string((*_symbols)[stockLocate], 8);
                    stripWhitespaceFromCPPString(name);
                    _locates -> emplace(name, stockLocate);
                    return message;
                }

                case MessageTypes::MESSAGE_TYPE_STOCK_TRADING_ACTION:
                return MessageBody {
                    .tag = MessageTag::StockTradingAction,
                    .stockTradingAction = parseStockTradingActionBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_SYSTEM_EVENT:
                return MessageBody {
                    .tag = MessageTag::SystemEvent,
                    .systemEvent = parseSystemEventBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_TRADE_CROSS:
                return MessageBody {
                    .tag = MessageTag::TradeCross,
                    .tradeCross = parseTradeCrossBody(buffer)
                };

                case MessageTypes::MESSAGE_TYPE_TRADE_NON_CROSS:
                return MessageBody {
                    .tag = MessageTag::TradeNonCross,
                    .tradeNonCross = parseTradeNonCrossBody(buffer)
                };

                default:
                return MessageBody {
                    .tag = MessageTag::NullMessage  
                };
            }
        }

        void processMessage(Message&& message) {
            std::unique_ptr<PerStockOrderState>& orderState = (*_locateIndexedPerStockState)[message.header.stockLocate].orderState;
            std::unique_ptr<PerStockLedger>& ledger = (*_locateIndexedPerStockState)[message.header.stockLocate].ledger;
            PerStockVWAP& vwap = (*_locateIndexedPerStockState)[message.header.stockLocate].vwap;
            PerStockVWAPPrefixHistory& vwapPrefixes = (*_locateIndexedPerStockState)[message.header.stockLocate].vwapPrefixes;
            PerStockTWAP& twap = (*_locateIndexedPerStockState)[message.header.stockLocate].twap;
            PerStockTWAPPrefixHistory& twapPrefixes = (*_locateIndexedPerStockState)[message.header.stockLocate].twapPrefixes;
            switch(message.body.tag) {

                // ======================= Order State Mutation Message Handlers (Minmally) =======================
                case MessageTag::AddOrder: 
                    if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;
                    orderState -> addOrder(message.body.addOrder.shares, message.body.addOrder.price, message.body.addOrder.orderReferenceNumber);        
                    break;

                case MessageTag::AddOrderWithMPID:  
                    if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;
                    orderState -> addOrder(message.body.addOrderWithMPID.shares, message.body.addOrderWithMPID.price, message.body.addOrderWithMPID.orderReferenceNumber);  
                    break;
                
                case MessageTag::OrderExecuted:
                    {
                        if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;
                        // Do order State here
                        uint32_t price = orderState -> executeOrder(message.body.orderExecuted.executedShares, message.body.orderExecuted.orderReferenceNumber, message.body.orderExecuted.matchNumber);
                        ledger -> addExecutedTradeToLedger(
                            true, // Include in VWAP metrics, since non-printable option does not exist 
                            // Order State call returns execution price 
                            price, 
                            message.body.orderExecuted.executedShares, 
                            message.body.orderExecuted.matchNumber
                        );
                        if(outside_of_market_hours(_systemEventSequenceTracker, message.seq)) break;
                        // Accumulate VWAP
                        vwapPrefixes.processVWAPUpdate(
                            price, 
                            message.body.orderExecuted.executedShares, 
                            message.header.timestamp, 
                            vwap
                        );
                        // Accumulate TWAP
                        if(twap.lastTimestamp < message.header.timestamp && twap.lastPrice != price) {
                            twapPrefixes.processTWAPUpdate(price, message.header.timestamp, twap);
                        }
                        break;
                    }

                case MessageTag::OrderExecutedWithPrice:
                    if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;
                    orderState -> executeOrderWithPrice(message.body.orderExecutedWithPrice.executionPrice, message.body.orderExecutedWithPrice.executedShares, message.body.orderExecutedWithPrice.orderReferenceNumber, message.body.orderExecutedWithPrice.matchNumber); 
                    ledger -> addExecutedTradeToLedger(
                        message.body.orderExecutedWithPrice.printable == PRINTABLE,
                        message.body.orderExecutedWithPrice.executionPrice, 
                        message.body.orderExecutedWithPrice.executedShares, 
                        message.body.orderExecutedWithPrice.matchNumber
                    );
                    if(outside_of_market_hours(_systemEventSequenceTracker, message.seq) 
                    || message.body.orderExecutedWithPrice.printable != PRINTABLE) break;
                    vwapPrefixes.processVWAPUpdate(
                        message.body.orderExecutedWithPrice.executionPrice, 
                        message.body.orderExecutedWithPrice.executedShares, 
                        message.header.timestamp, 
                        vwap
                    );
                    // Accumulate TWAP
                    if(twap.lastTimestamp < message.header.timestamp && twap.lastPrice != message.body.orderExecutedWithPrice.executionPrice) {
                        twapPrefixes.processTWAPUpdate(message.body.orderExecutedWithPrice.executionPrice, message.header.timestamp, twap);
                    }
                    break;

                case MessageTag::OrderCancel:
                    if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;
                    orderState -> cancelOrder(message.body.orderCancel.cancelledShares, message.body.orderCancel.orderReferenceNumber);             
                    break;

                case MessageTag::OrderDelete:
                    if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;
                    orderState -> deleteOrder(message.body.orderDelete.orderReferenceNumber);                
                    break;

                case MessageTag::OrderReplace:  
                    if(outside_of_system_hours(_systemEventSequenceTracker, message.seq)) break;    
                    orderState -> replaceOrder(message.body.orderReplace.price, message.body.orderReplace.shares, message.body.orderReplace.oldOrderReferenceNumber, message.body.orderReplace.newOrderReferenceNumber);         
                    break;
                
                // ======================= Ledger Mutation Message Handlers (Which DO NOT Touch Order State) =======================
                case MessageTag::TradeNonCross:
                    ledger -> addExecutedTradeToLedger(
                        true, // This data is "printable"
                        message.body.tradeNonCross.price,
                        message.body.tradeNonCross.shares,
                        message.body.tradeNonCross.matchNumber
                    );
                    if(outside_of_market_hours(_systemEventSequenceTracker, message.seq)) break;
                    // DO VWAP
                    vwapPrefixes.processVWAPUpdate(
                        message.body.tradeNonCross.price,
                        message.body.tradeNonCross.shares,
                        message.header.timestamp,
                        vwap
                    );
                    // Accumulate TWAP
                    if(twap.lastTimestamp < message.header.timestamp && twap.lastPrice != message.body.tradeNonCross.price) {
                        twapPrefixes.processTWAPUpdate(message.body.tradeNonCross.price, message.header.timestamp, twap);
                    }
                    break;
                
                case MessageTag::TradeCross:
                    ledger -> addExecutedTradeToLedger(
                        true, // This data is "printable"
                        message.body.tradeCross.crossPrice,
                        message.body.tradeCross.crossShares,
                        message.body.tradeCross.matchNumber
                    );
                    if(outside_of_market_hours(_systemEventSequenceTracker, message.seq)) break;
                    // DO VWAP
                    vwapPrefixes.processVWAPUpdate(
                        message.body.tradeCross.crossPrice,
                        message.body.tradeCross.crossShares,
                        message.header.timestamp,
                        vwap
                    );
                    // Accumulate TWAP
                    if(twap.lastTimestamp < message.header.timestamp && twap.lastPrice != message.body.tradeCross.crossPrice) {
                        twapPrefixes.processTWAPUpdate(message.body.tradeCross.crossPrice, message.header.timestamp, twap);
                    }                    
                    break;
                
                case MessageTag::BrokenTradeOrOrderExecution: 
                    {
                        PerStockVWAPCorrection correction = ledger -> handleBrokenTradeOrOrderExecution(message.body.brokenTradeOrOrderExecution.matchNumber);
                        correctVWAP(vwap, correction);
                        break;
                    }

                default:
                    break;
            }
        }

        // Non-owning pattern. Guaranteed that lifetime of shard manager object outlives this consumer object 100% of the time
        queue_type* _queue = nullptr;
        LocateIndexedPerStockState* _locateIndexedPerStockState = nullptr;
        Symbols* _symbols = nullptr;
        Locates* _locates = nullptr;
        SystemEventSequence* _systemEventSequenceTracker = nullptr;

    public:

        std::atomic<bool> finished{false};

        void run(queue_type& queue, int run_on_this_core, LocateIndexedPerStockState& locateIndexedPerStockState, Symbols& symbols, Locates& locates, SystemEventSequence& systemEventSequenceTracker) {

            std::cout << "Pinned consumer " << (((run_on_this_core-ThreadConstants::FIRST_CONSUMER_CORE)/2)+1) << " to core " << run_on_this_core << std::endl;
    
            if(!pin_to_core(run_on_this_core)) {
                std::cout << "Failed to pin consumer " << (((run_on_this_core-ThreadConstants::FIRST_CONSUMER_CORE)/2)+1) << " to core " << run_on_this_core << std::endl;
            }
            
            // Set shard level data structure objects
            {
                _queue = &queue;
                _locateIndexedPerStockState = &locateIndexedPerStockState;
                _symbols = &symbols;
                _locates = &locates;
                _systemEventSequenceTracker = &systemEventSequenceTracker;
            }

            while(!finished.load()) {
                if(!queue.buffer.empty()) {
                    BinaryMessageWrapper bmsg = *(queue.buffer.front());
                    queue.buffer.pop();
                    // numMessages.fetch_add(1, std::memory_order_relaxed);
                    MessageBody msgBody = parseMessage(bmsg.header.messageType, bmsg.header.stockLocate, bmsg.buffer);
                    processMessage(Message{bmsg.seq, bmsg.header, msgBody});
                } else {
                    // backoff();
                    std::this_thread::yield();
                }
            } 
        }
    
};

#endif  // SHARD_CONSUMER_H_
