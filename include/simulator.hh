#pragma once
#include <vector>
#include "orderbook.hh"
#include "market_data.pb.h"

class Simulator {
public:
    Simulator(std::vector<int>& instrumentIDs, std::vector<int>& depths);
    void run();

private:
    std::vector<std::unique_ptr<OrderBook>> books_;
};