#ifndef SHARD_MANAGER_H_
#define SHARD_MANAGER_H_

#include <utility>
#include <array>
#include <iostream>

#include <ShardConsumer.hpp>
#include <BinaryMessageWrapper.h>

#include <message_utils.hpp>

template<uint8_t numberOfShards, uint32_t shardSize>
class ShardManager {

    public:
    ShardManager() : _queues(make_queues(std::make_index_sequence<numberOfShards>{})), coreIndex(ThreadConstants::FIRST_CONSUMER_CORE) {}

    // Make sure all of the threads are joined when destructing this object.
    // All threads should have terminated after receiving a "sentinel message," which is guaranteed to have occured before this destructor is invoked 
    ~ShardManager() = default;
    ShardManager(const ShardManager&) = default;
    ShardManager(ShardManager&&) noexcept = default;
    ShardManager& operator=(const ShardManager&) = default;
    ShardManager& operator=(ShardManager&&) noexcept = default;

    void start() {
        for(auto i = 0; i < numberOfShards; ++i) {
            // TODO: Check once the shard threads become CPU bound, and distribute the workload across cores 4-14 inclusive (pass coreIndex by value)
            _threads[i] = std::thread(&ShardConsumer::run, &_shards[i], std::ref(_queues[i]), coreIndex, std::ref(_orderBooks[i]));
            coreIndex += 2;
        }
    }

    bool dispatch(BinaryMessageWrapper&& msg) {
        if(isIrrelevantMessageType(msg.header.messageType)) 
            return false;
        uint8_t queueIndex = getShardIndex(msg.header.stockLocate);
        while(_queues[queueIndex].buffer.size() == _queues[queueIndex].buffer.capacity())
            std::this_thread::yield();
        _queues[queueIndex].buffer.push(msg);
        return true;
    }

    void shutDownConsumers() {
        for(auto& shard: _shards) {
            shard.finished.store(true);
        }
        for(auto& thread: _threads) {
            thread.join();
        }
        // std::cout << "Queue processed " << numMessages << " messages\n";
    }

    private:
    // Shard consumer objects
    std::array<ShardConsumer, numberOfShards> _shards;

    // One thread per-shard
    std::array<std::thread, numberOfShards> _threads;

    // One queue per-shard
    std::array<queue_type, numberOfShards> _queues;

    std::array<OrderBook, numberOfShards> _orderBooks;

    template<size_t... I>
    static std::array<queue_type, numberOfShards> make_queues(std::index_sequence<I...>) {
        return { ((void)I, queue_type{rigtorp::SPSCQueue<BinaryMessageWrapper>(shardSize)})... };
    }

    /**
     * Should be incremented by two, per consumer thread. For MY MACHINE:
     * Main thread: cpu 2 (or maybe 16, first E-core. Subject to test)
     * Consumer 1: cpu 4
     * Consumer N: 2*N + 4
     * My machine has 8 hyperthreaded core families, so 5 - 7 consumers seems optimal 
     */
    int coreIndex;

};

#endif // SHARD_MANAGER_H_
