#pragma once

#include <vector>
#include <mutex>
#include "market_data.pb.h"

class OrderBook {
public:
    OrderBook(int instrumentID, int depth);
    
    market_simulator::Snapshot makeSnapshots();
    market_simulator::Incremental makeIncrements();
    void simulateSteps();

private:
    int instrumentID_;
    int maxDepth_;
    std::vector<market_simulator::OrderLevel> bids_, asks_;
    std::mutex mtx_;
};

