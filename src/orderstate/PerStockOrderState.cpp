#include <PerStockOrderState.h>

#include <assert.h>
#include <algorithm>
#include <vector>

void PerStockOrderState::_addOrderToOrderState(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber) {
    assert(_perStockOrderState.count(orderReferenceNumber) == 0); // Keys must be unique
    _perStockOrderState.try_emplace(orderReferenceNumber, shares, price); // TODO: insert into map optimally
    assert(_perStockOrderState.count(orderReferenceNumber) == 1); // Entry should exist
}

void PerStockOrderState::_removeOrderFromOrderState(uint64_t orderReferenceNumber) {
    assert(_perStockOrderState.count(orderReferenceNumber) == 1);
    _perStockOrderState.erase(orderReferenceNumber);
    assert(_perStockOrderState.count(orderReferenceNumber) == 0);
}

// TODO: Benchmark replace key vs erase/insert. This may not be needed
void PerStockOrderState::_replaceKeyInOrderState(uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber) {
    assert(_perStockOrderState.count(oldOrderReferenceNumber) == 1);
    assert(_perStockOrderState.count(newOrderReferenceNumber) == 0);

    (void)_perStockOrderState.replace_key(_perStockOrderState.find(oldOrderReferenceNumber), newOrderReferenceNumber);

    assert(_perStockOrderState.count(oldOrderReferenceNumber) == 0);
    assert(_perStockOrderState.count(newOrderReferenceNumber) == 1);
}

void PerStockOrderState::addOrder(uint32_t shares, uint32_t price, uint64_t orderReferenceNumber) {
    _addOrderToOrderState(shares, price, orderReferenceNumber);
}

uint32_t PerStockOrderState::executeOrder(uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber) {
    assert(_perStockOrderState.count(orderReferenceNumber) == 1);
    PerStockOrderStateEntry& entry = _perStockOrderState.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numExecutedShares;
    assert(numOutstandingShares >= 0);
    uint32_t orderPrice = entry.price;
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderState(orderReferenceNumber);
        return orderPrice;
    }
    entry.shares = numOutstandingShares;
    return orderPrice;
}       

void PerStockOrderState::executeOrderWithPrice(uint32_t executionPrice, uint32_t numExecutedShares, uint64_t orderReferenceNumber, uint64_t matchNumber) {
    assert(_perStockOrderState.count(orderReferenceNumber) == 1);
    PerStockOrderStateEntry& entry = _perStockOrderState.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numExecutedShares;
    assert(numOutstandingShares >= 0);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderState(orderReferenceNumber);
        return;
    }
    entry.shares = numOutstandingShares;
}


void PerStockOrderState::cancelOrder(uint32_t numSharesCancelled, uint64_t orderReferenceNumber) {
    assert(_perStockOrderState.count(orderReferenceNumber) == 1);
    PerStockOrderStateEntry& entry = _perStockOrderState.at(orderReferenceNumber);
    uint32_t numOutstandingShares = entry.shares - numSharesCancelled;
    assert(numOutstandingShares >= 0);
    if(numOutstandingShares == 0) {
        _removeOrderFromOrderState(orderReferenceNumber);
        return;
    }
    entry.shares = numOutstandingShares;
}

void PerStockOrderState::deleteOrder(uint64_t orderReferenceNumber) {
    _removeOrderFromOrderState(orderReferenceNumber);
}

void PerStockOrderState::replaceOrder(uint32_t newPrice, uint32_t newShares, uint64_t oldOrderReferenceNumber, uint64_t newOrderReferenceNumber) {
    _replaceKeyInOrderState(oldOrderReferenceNumber, newOrderReferenceNumber);
    PerStockOrderStateEntry& entry = _perStockOrderState.at(newOrderReferenceNumber);
    entry.price = newPrice;
    entry.shares = newShares;
}

void PerStockOrderState::dump(std::ostream& os) const {
    std::vector<std::pair<uint64_t, PerStockOrderStateEntry>> orders;
    orders.reserve(_perStockOrderState.size());
    for (const auto& kv : _perStockOrderState) {
        orders.push_back(kv);
    }
    std::sort(orders.begin(), orders.end(), [](const auto& a, const auto& b) { return a.first < b.first; });

    os << "=== Order State Entries (" << orders.size() << ") ===\n";
    for (const auto& [orderRef, entry] : orders) {
        os << "orderRef=" << orderRef << " price=" << entry.price << " shares=" << entry.shares << '\n';
    }
}
