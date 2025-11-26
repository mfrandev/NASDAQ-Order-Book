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
#include <OrderBook.h>

#include <iostream>
// Global counter tracking how many messages consumers have processed
// inline std::atomic<uint64_t> numMessages{0};

class ShardConsumer {

    private:

        void backoff(queue_type& queue) {
            // Step 1: Busy wait
            for(auto i = 0; i < ThreadConstants::BUSY_WAIT_CYCLES; ++i) {
                _mm_pause();
                if(!queue.buffer.empty()) return;
            }
            
            // Step 2: thread yield
            for(auto i = 0; i < ThreadConstants::YIELD_CYCLES; ++i) {
                std::this_thread::yield();
                if(!queue.buffer.empty()) return;
            }

            // Step 3: Condvar sleep
            // _cv.wait();
        }

        void processMessage(Message&& message, OrderBook& orderBook) {
            switch(message.body.tag) {
                case MessageTag::AddOrder:   
                orderBook.addOrder(message.header.stockLocate, message.body.addOrder.shares, message.body.addOrder.price, message.body.addOrder.orderReferenceNumber);        
                break;

                case MessageTag::AddOrderWithMPID:      
                orderBook.addOrder(message.header.stockLocate, message.body.addOrderWithMPID.shares, message.body.addOrderWithMPID.price, message.body.addOrderWithMPID.orderReferenceNumber);   
                break;
                
                case MessageTag::OrderExecuted:
                orderBook.executeOrder(message.body.orderExecuted.executedShares, message.body.orderExecuted.orderReferenceNumber, message.body.orderExecuted.matchNumber);              
                break;

                case MessageTag::OrderExecutedWithPrice: 

                // 1.4.2: "If a firm is looking to use the data in time-and-sales displays or volume calculations, Nasdaq recommends that firms ignore messages marked as non-printable to prevent double counting"
                orderBook.executeOrderWithPrice(message.body.orderExecutedWithPrice.executionPrice, message.body.orderExecutedWithPrice.executedShares, message.body.orderExecutedWithPrice.orderReferenceNumber, message.body.orderExecutedWithPrice.matchNumber); 
                // if(message.body.orderExecutedWithPrice.printable == PRINTABLE) // Do VWAP;
                // TODO: since printable, account for VWAP here
                break;

                case MessageTag::OrderCancel:
                orderBook.cancelOrder(message.body.orderCancel.cancelledShares, message.body.orderCancel.orderReferenceNumber);             
                break;

                case MessageTag::OrderDelete:
                orderBook.deleteOrder(message.body.orderDelete.orderReferenceNumber);                
                break;

                case MessageTag::OrderReplace:      
                orderBook.replaceOrder(message.body.orderReplace.price, message.body.orderReplace.shares, message.body.orderReplace.oldOrderReferenceNumber, message.body.orderReplace.newOrderReferenceNumber);         
                break;
                
                default:
                break;
            }
        }

    public:

        std::atomic<bool> finished{false};

        void run(queue_type& queue, int run_on_this_core, OrderBook& orderBook) {

            std::cout << run_on_this_core << std::endl;
    
            if(!pin_to_core(run_on_this_core)) {
                std::cout << "Failed to pin consumer " << (((run_on_this_core-ThreadConstants::FIRST_CONSUMER_CORE)/2)+1) << " to core " << run_on_this_core << std::endl;
            }
            while(!finished.load()) {
                if(!queue.buffer.empty()) {
                    BinaryMessageWrapper bmsg = *(queue.buffer.front());
                    queue.buffer.pop();
                    // numMessages.fetch_add(1, std::memory_order_relaxed);
                    MessageBody msgBody = parseMessage(bmsg.header.messageType, bmsg.header.stockLocate, bmsg.buffer);
                    processMessage(Message{bmsg.header, msgBody}, orderBook);
                } else {
                    std::this_thread::yield();
                }
            } 
        }
    
};

#endif  // SHARD_CONSUMER_H_
