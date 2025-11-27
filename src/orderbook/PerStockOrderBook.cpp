#include <PerStockOrderBook.h>

#include <assert.h>
#include <algorithm>
#include <vector>

void PerStockOrderBook::_addOrderToOrderBook(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber) {
    assert(_perStockOrderBook.count(orderReferenceNumber) == 0); // Keys must be unique
    _perStockOrderBook.try_emplace(orderReferenceNumber, shares, price); // TODO: insert into map optimally
    assert(_perStockOrderBook.count(orderReferenceNumber) == 1); // Entry should exist
}

void PerStockOrderBook::_removeOrderFromOrderBook(uint64_t orderReferenceNumber) {
    assert(_perStockOrderBook.count(orderReferenceNumber) == 1);
    _perStockOrderBook.erase(orderReferenceNumber);
    assert(_perStockOrderBook.count(orderReferenceNumber) == 0);
}
// TODO: Benchmark replace key vs erase/insert. This may not be needed
void PerStockOrderBook::_replaceKeyInOrderBook(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber) {
    assert(_perStockOrderBook.count(oldOrderReferenceNumber) == 1);
    assert(_perStockOrderBook.count(newOrderReferenceNumber) == 0);

    (void)_perStockOrderBook.replace_key(_perStockOrderBook.find(oldOrderReferenceNumber), newOrderReferenceNumber);

    assert(_perStockOrderBook.count(oldOrderReferenceNumber) == 0);
    assert(_perStockOrderBook.count(newOrderReferenceNumber) == 1);
}

void PerStockOrderBook::_addExecutedTradeToLedger(uint32_t price, uint64_t numShares, uint64_t matchNumber) {
    PerStockLedgerEntry& entry = _perStockLedger[matchNumber];
    entry.notionalPrice += (price * numShares);
    entry.numShares += numShares;
}



void PerStockOrderBook::addOrder(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber) {
    _addOrderToOrderBook(shares, price, orderReferenceNumber);
}

uint32_t PerStockOrderBook::executeOrder(uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber) {
    assert(_perStockOrderBook.count(orderReferenceNumber) == 1);
    PerStockOrderBookEntry& entry = _perStockOrderBook.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numExecutedShares;
    assert(numOutstandingShares >= 0);
    uint32_t orderPrice = entry.price;
    _addExecutedTradeToLedger(orderPrice, numExecutedShares, matchNumber);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderBook(orderReferenceNumber);
        return orderPrice;
    }
    entry.shares = numOutstandingShares;
    return orderPrice;
}       

void PerStockOrderBook::executeOrderWithPrice(uint32_t executionPrice, uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber) {
    assert(_perStockOrderBook.count(orderReferenceNumber) == 1);
    PerStockOrderBookEntry& entry = _perStockOrderBook.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numExecutedShares;
    assert(numOutstandingShares >= 0);
    _addExecutedTradeToLedger(executionPrice, numExecutedShares, matchNumber);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderBook(orderReferenceNumber);
        return;
    }
    entry.shares = numOutstandingShares;
}


void PerStockOrderBook::cancelOrder(uint32_t numSharesCancelled, uint64_t orderReferenceNumber) {
    assert(_perStockOrderBook.count(orderReferenceNumber) == 1);
    PerStockOrderBookEntry& entry = _perStockOrderBook.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numSharesCancelled;
    assert(numOutstandingShares >= 0);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderBook(orderReferenceNumber);
        return;
    }
    entry.shares = numOutstandingShares;
}

void PerStockOrderBook::deleteOrder(uint64_t orderReferenceNumber) {
    _removeOrderFromOrderBook(orderReferenceNumber);
}

void PerStockOrderBook::replaceOrder(uint32_t newPrice, uint32_t newShares, uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber) {
    _replaceKeyInOrderBook(oldOrderReferenceNumber, newOrderReferenceNumber);
    PerStockOrderBookEntry& entry = _perStockOrderBook.at(newOrderReferenceNumber);
    entry.price = newPrice;
    entry.shares = newShares;
}

void PerStockOrderBook::dump(std::ostream& os) const {
    std::vector<std::pair<uint64_t, PerStockOrderBookEntry>> orders;
    orders.reserve(_perStockOrderBook.size());
    for (const auto& kv : _perStockOrderBook) {
        orders.push_back(kv);
    }
    std::sort(orders.begin(), orders.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    std::vector<std::pair<uint64_t, PerStockLedgerEntry>> ledger;
    ledger.reserve(_perStockLedger.size());
    for (const auto& kv : _perStockLedger) {
        ledger.push_back(kv);
    }
    std::sort(ledger.begin(), ledger.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    os << "=== Order Book Entries (" << orders.size() << ") ===\n";
    for (const auto& [orderRef, entry] : orders) {
        os << "orderRef=" << orderRef << " price=" << entry.price << " shares=" << entry.shares << '\n';
    }

    os << "=== Executed Trades Ledger (" << ledger.size() << ") ===\n";
    for (const auto& [matchNumber, entry] : ledger) {
        os << "matchNumber=" << matchNumber << " price=" << (entry.notionalPrice / entry.numShares) << " shares=" << entry.numShares << '\n';
    }
}
