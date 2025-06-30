#pragma once

#include <vector>
#include <mutex>
#include "market_data.pb.h"

class OrderBook {
public:
    OrderBook(int instrumentId, int depth, double lamdaLimit, double lamdaMarket, double lamdaCancel);
    market_simulator::Snapshot makeSnapshots();
    market_simulator::Incremental makeIncremental();
    void simulateSteps(double horizon_seconds, double snapshot_interval);

private:
    int instrumentID_;
    int maxDepth_;
    std::vector<market_simulator::OrderLevel> bids_, asks_;
    std::mutex mtx_;
    std::mt19937 gen_;
    std::exponential_distribution<> expLimit_, expMarket_, expCancel_;
    std::uniform_int_distribution<> sizeDist_{0, 50};
    std::uniform_int_distribution<> levelDist_;
    double nextLimit_t, nextMarket_t, nextCancel_t;
};
