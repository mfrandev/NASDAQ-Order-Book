#include <OrderBook.h>

#include <assert.h>

OrderBook* OrderBook::_instance = nullptr;
OrderBook& OrderBook::getInstance() {
    if(_instance == nullptr)
        _instance = new OrderBook();
    return *_instance;
}

void OrderBook::_addOrderToOrderBook(uint16_t stockLocate, uint32_t shares, uint32_t price, uint64_t orderReferenceNumber) {
    assert(_orderBook.count(orderReferenceNumber) == 0); // Keys must be unique
    _orderBook.try_emplace(orderReferenceNumber, stockLocate, shares, price); // TODO: insert into map optimally
    assert(_orderBook.count(orderReferenceNumber) == 1); // Entry should exist
}

void OrderBook::_removeOrderFromOrderBook(uint64_t orderReferenceNumber) {
    assert(_orderBook.count(orderReferenceNumber) == 1);
    _orderBook.erase(orderReferenceNumber);
    assert(_orderBook.count(orderReferenceNumber) == 0);
}
// TODO: Benchmark replace key vs erase/insert. This may not be needed
void OrderBook::_replaceKeyInOrderBook(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber) {
    assert(_orderBook.count(oldOrderReferenceNumber) == 1);
    assert(_orderBook.count(newOrderReferenceNumber) == 0);

    (void)_orderBook.replace_key(_orderBook.find(oldOrderReferenceNumber), newOrderReferenceNumber);

    assert(_orderBook.count(oldOrderReferenceNumber) == 0);
    assert(_orderBook.count(newOrderReferenceNumber) == 1);
}

void OrderBook::_addExecutedTradeToLedger(uint16_t stockLocate, uint32_t price, uint64_t numShares, uint64_t matchNumber) {
    assert(_ledger.count(matchNumber) == 0);
    _ledger.try_emplace(matchNumber, stockLocate, price, numShares);
    assert(_ledger.count(matchNumber) == 1);
}



void OrderBook::addOrder(uint16_t stockLocate, uint32_t shares, uint32_t price, uint64_t orderReferenceNumber) {
    _addOrderToOrderBook(stockLocate, shares, price, orderReferenceNumber);
}

uint32_t OrderBook::executeOrder(uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber) {
    assert(_orderBook.count(orderReferenceNumber) == 1);
    OrderBookEntry& entry = _orderBook.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numExecutedShares;
    assert(numOutstandingShares >= 0);
    uint32_t orderPrice = entry.price;
    _addExecutedTradeToLedger(entry.stockLocate, orderPrice, numExecutedShares, matchNumber);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderBook(orderReferenceNumber);
        return orderPrice;
    }
    entry.shares = numOutstandingShares;
    return orderPrice;
}       

void OrderBook::executeOrderWithPrice(uint32_t executionPrice, uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber) {
    assert(_orderBook.count(orderReferenceNumber) == 1);
    OrderBookEntry& entry = _orderBook.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numExecutedShares;
    assert(numOutstandingShares >= 0);
    _addExecutedTradeToLedger(entry.stockLocate, executionPrice, numExecutedShares, matchNumber);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderBook(orderReferenceNumber);
        return;
    }
    entry.shares = numOutstandingShares;
}  


void OrderBook::cancelOrder(uint32_t numSharesCancelled, uint64_t orderReferenceNumber) {
    assert(_orderBook.count(orderReferenceNumber) == 1);
    OrderBookEntry& entry = _orderBook.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numSharesCancelled;
    assert(numOutstandingShares >= 0);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderBook(orderReferenceNumber);
        return;
    }
    entry.shares = numOutstandingShares;
}

void OrderBook::deleteOrder(uint64_t orderReferenceNumber) {
    _removeOrderFromOrderBook(orderReferenceNumber);
}

void OrderBook::replaceOrder(uint32_t newPrice, uint32_t newShares, uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber) {
    _replaceKeyInOrderBook(oldOrderReferenceNumber, newOrderReferenceNumber);
    OrderBookEntry& entry = _orderBook.at(newOrderReferenceNumber);
    entry.price = newPrice;
    entry.shares = newShares;
}
