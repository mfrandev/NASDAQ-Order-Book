#ifndef SHARD_CONSUMER_H_
#define SHARD_CONSUMER_H_

#include <immintrin.h> // For _mm_pause
#include <thread>
#include <condition_variable>
#include <memory>
#include <atomic>

#include <ThreadConstants.h>
#include <thread_utils.hpp>
#include <queue_wrapper.h>
#include <Message.hpp>

#include <PerStockLedger.h>
#include <PerStockOrderBook.h>

#include <SystemEventSequence.h>

#include <iostream>
// Global counter tracking how many messages consumers have processed
// inline std::atomic<uint64_t> numMessages{0};

struct PerStockState {
    std::unique_ptr<PerStockOrderBook> orderBook;
    std::unique_ptr<PerStockLedger> ledger;
};

using LocateIndexedPerStockState = std::array<PerStockState, PerStockOrderBookConstants::NUM_OF_STOCK_LOCATE>;
using Symbols = std::array<char[MessageFieldSizes::STOCK_SIZE], PerStockOrderBookConstants::NUM_OF_STOCK_LOCATE>;

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
                // Initialize this stock locate 
                (*_locateIndexedPerStockState)[stockLocate].orderBook = std::make_unique<PerStockOrderBook>();
                (*_locateIndexedPerStockState)[stockLocate].ledger = std::make_unique<PerStockLedger>();
                return MessageBody {
                    .tag = MessageTag::StockDirectory,
                    .stockDirectory = parseStockDirectoryBody((*_symbols)[stockLocate], buffer)
                };

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
            std::unique_ptr<PerStockOrderBook>& orderBook = (*_locateIndexedPerStockState)[message.header.stockLocate].orderBook;
            std::unique_ptr<PerStockLedger>& ledger = (*_locateIndexedPerStockState)[message.header.stockLocate].ledger;
            switch(message.body.tag) {

                // ======================= Order Book Mutation Message Handlers =======================
                case MessageTag::AddOrder: 
                    if(message.seq < _systemEventSequenceTracker->startOfSystemHoursSeq.load(std::memory_order_acquire) 
                    || message.seq > _systemEventSequenceTracker->endOfSystemHoursSeq.load(std::memory_order_acquire)) {
                        break;
                    }
                    orderBook -> addOrder(message.body.addOrder.shares, message.body.addOrder.price, message.body.addOrder.orderReferenceNumber);        
                    break;

                case MessageTag::AddOrderWithMPID:  
                    if(message.seq < _systemEventSequenceTracker->startOfSystemHoursSeq.load(std::memory_order_acquire) 
                    || message.seq > _systemEventSequenceTracker->endOfSystemHoursSeq.load(std::memory_order_acquire)) {
                        break;
                    }
                    orderBook -> addOrder(message.body.addOrderWithMPID.shares, message.body.addOrderWithMPID.price, message.body.addOrderWithMPID.orderReferenceNumber);  
                    break;
                
                case MessageTag::OrderExecuted:
                    if(message.seq < _systemEventSequenceTracker->startOfMarketHoursSeq.load(std::memory_order_acquire) 
                    || message.seq > _systemEventSequenceTracker->endOfMarketHoursSeq.load(std::memory_order_acquire)) {
                        break;
                    }
                    ledger -> addExecutedTradeToLedger(
                        true, // Include in VWAP metrics 
                        // Order book call returns execution price
                        orderBook -> executeOrder(message.body.orderExecuted.executedShares, message.body.orderExecuted.orderReferenceNumber, message.body.orderExecuted.matchNumber), 
                        message.body.orderExecuted.executedShares, 
                        message.body.orderExecuted.matchNumber
                    );
                    break;

                case MessageTag::OrderExecutedWithPrice:
                    if(message.seq < _systemEventSequenceTracker->startOfMarketHoursSeq.load(std::memory_order_acquire) 
                    || message.seq > _systemEventSequenceTracker->endOfMarketHoursSeq.load(std::memory_order_acquire)) {
                        break; 
                    }
                    orderBook -> executeOrderWithPrice(message.body.orderExecutedWithPrice.executionPrice, message.body.orderExecutedWithPrice.executedShares, message.body.orderExecutedWithPrice.orderReferenceNumber, message.body.orderExecutedWithPrice.matchNumber); 
                    ledger -> addExecutedTradeToLedger(
                        message.body.orderExecutedWithPrice.printable == PRINTABLE,
                        message.body.orderExecutedWithPrice.executionPrice, 
                        message.body.orderExecutedWithPrice.executedShares, 
                        message.body.orderExecutedWithPrice.matchNumber
                    );
                    // if( == PRINTABLE) 
                        // Do VWAP;
                    break;

                case MessageTag::OrderCancel:
                    orderBook -> cancelOrder(message.body.orderCancel.cancelledShares, message.body.orderCancel.orderReferenceNumber);             
                    break;

                case MessageTag::OrderDelete:
                    orderBook -> deleteOrder(message.body.orderDelete.orderReferenceNumber);                
                    break;

                case MessageTag::OrderReplace:      
                    orderBook -> replaceOrder(message.body.orderReplace.price, message.body.orderReplace.shares, message.body.orderReplace.oldOrderReferenceNumber, message.body.orderReplace.newOrderReferenceNumber);         
                    break;
                
                // 

                default:
                    break;
            }
        }

        // Non-owning pattern. Guaranteed that lifetime of shard manager object outlives this consumer object 100% of the time
        queue_type* _queue = nullptr;
        LocateIndexedPerStockState* _locateIndexedPerStockState = nullptr;
        Symbols* _symbols = nullptr;
        SystemEventSequence* _systemEventSequenceTracker = nullptr;

    public:

        std::atomic<bool> finished{false};

        void run(queue_type& queue, int run_on_this_core, LocateIndexedPerStockState& locateIndexedPerStockState, Symbols& symbols, SystemEventSequence& systemEventSequenceTracker) {

            std::cout << run_on_this_core << std::endl;
    
            if(!pin_to_core(run_on_this_core)) {
                std::cout << "Failed to pin consumer " << (((run_on_this_core-ThreadConstants::FIRST_CONSUMER_CORE)/2)+1) << " to core " << run_on_this_core << std::endl;
            }
            
            // Set shard level data structure objects
            {
                _queue = &queue;
                _locateIndexedPerStockState = &locateIndexedPerStockState;
                _symbols = &symbols;
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
