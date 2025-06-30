#include "orderbook.hh"
#include <random>

OrderBook::OrderBook(int instrumentId, int depth, double lamdaLimit, double lamdaMarket, double lamdaCancel)
    : instrumentID_{instrumentId},
    maxDepth_{depth},
    gen_{std::random_device{}()},
    expLimit_{lamdaLimit},
    expMarket_{lamdaMarket},
    expCancel_{lamdaCancel},
    levelDist_{0, depth-1}
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
    nextLimit_t = expLimit_(gen_);
    nextMarket_t = expMarket_(gen_);
    nextCancel_t = expCancel_(gen_);
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

market_simulator::Incremental OrderBook::makeIncremental() {
    std::lock_guard<std::mutex> lock(mtx_);
    market_simulator::Incremental increment_;

    // Decide which event fires first
    double t_min = std::min({nextLimit_t, nextMarket_t, nextCancel_t});
    enum {LIMIT, MARKET, CANCEL} ev = t_min == nextLimit_t ? LIMIT : t_min == nextMarket_t ? MARKET : CANCEL;
    // Schedule next of that type
    if (ev == LIMIT) nextLimit_t += expLimit_(gen_);
    if (ev == MARKET) nextMarket_t += expMarket_(gen_);
    if (ev == CANCEL) nextCancel_t += expCancel_(gen_);

    // Build incremental
    market_simulator::Incremental inc;
    inc.set_instrument_id(instrumentID_);

    int idx = levelDist_(gen_);
    uint32_t size = sizeDist_(gen_);
    market_simulator::OrderLevel* lvl = nullptr;

    if (ev == CANCEL) {
        bool isBid = (gen_() & 1);
        lvl = isBid ? &bids_[idx] : &asks_[idx];
        uint32_t removeAmt = std::min(lvl->size(), size);
        lvl->set_size(lvl->size() - removeAmt);
        inc.set_type(market_simulator::Incremental::REMOVE);
    }

    if (ev == MARKET) {
        bool isBuy = (gen_() & 1);
        lvl = isBuy ? &asks_[idx] : &bids_[idx];
        uint32_t fill = std::min(lvl->size(), size);
        lvl->set_size(lvl->size() - fill);
        inc.set_type(market_simulator::Incremental::REMOVE);
    }
    else {
        bool bidSide = (gen_() & 1);
        lvl = bidSide ? &bids_[idx] : &asks_[idx];
        lvl->set_size(lvl->size() + size);
        inc.set_type(market_simulator::Incremental::ADD);
    }

    inc.mutable_level()->CopyFrom(*lvl);
    return inc;
}

void OrderBook::simulateSteps(double horizon_seconds, double snapshot_interval) {
    double t = 0.0;
    double next_snap = snapshot_interval;

    while (t < horizon_seconds) {
        double t_next = std::min({nextLimit_t, nextMarket_t, nextCancel_t});
        t = t_next;

        auto inc = makeIncremental();
        
        if (t >= next_snap) {
            auto snap = makeSnapshots();
            next_snap += snapshot_interval;
        }
    }
}
