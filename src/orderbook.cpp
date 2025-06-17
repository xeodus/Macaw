#include "orderbook.hh"
#include <random>

OrderBook::OrderBook(int instrumentID, int depth)
: instrumentID_{instrumentID}, maxDepth_{depth}
{
    double basePrice = 100.0 + depth;

    for (int i = 0; i < maxDepth_; i++) {
        market_simulator::OrderLevel bid;
        bid.set_price(basePrice - i * 0.1);
        bid.set_size(100 - i * 10);
        bids_.push_back(bid);

        market_simulator::OrderLevel ask;
        ask.set_price(basePrice + i * 0.1);
        ask.set_size(100 - i * 10);
        asks_.push_back(ask);
    }
}

market_simulator::Snapshot OrderBook::makeSnapshots() {
    std::lock_guard<std::mutex> lock(mtx_);
    market_simulator::Snapshot snap;
    
    snap.set_instrument_id(instrumentID_);

    for (const auto& bid: bids_) {
        *snap.add_bids() = bid;
    }
    for (const auto& ask: asks_) {
        *snap.add_asks() = ask;
    }
    return snap;
}

market_simulator::Incremental OrderBook::makeIncrements() {
    std::lock_guard<std::mutex> lock(mtx_);
    market_simulator::Incremental increment_;

    increment_.set_instrument_id(instrumentID_);

    // Random number generation
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> side_dist(0, 1);
    std::uniform_int_distribution<> indx_dist(0, maxDepth_ - 1);
    std::uniform_int_distribution<> type_dist(0, 2);
    
    int side = side_dist(gen);
    int index = indx_dist(gen);
    int type = type_dist(gen);

    market_simulator::OrderLevel* level = nullptr;

    if (side == 0) {
        level = &bids_[index];
    }
    else {
        level = &asks_[index];
    }

    switch (type) {
        case 0:
            level->set_size(level->size() + 10);
            increment_.set_type(market_simulator::Incremental::ADD);
            break;
        case 1:
            level->set_size(level->size() + 5);
            increment_.set_type(market_simulator::Incremental::MODIFY);
            break;
        case 2:
            level->set_size(std::max(0u, (level->size() - 15)));
            increment_.set_type(market_simulator::Incremental::REMOVE);
            break;
    }

    increment_.mutable_level()->CopyFrom(*level);

    return increment_;
}

void OrderBook::simulateSteps() {
    makeIncrements();
}
