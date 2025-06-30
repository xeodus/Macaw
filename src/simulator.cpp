#include "simulator.hh"
#include <chrono>
#include <thread>
#include <vector>
#include <memory>

Simulator::Simulator(std::vector<int>& instrumentIDs, std::vector<int>& depths) {
    constexpr double LAMBDA_LIMIT = 5.0;
    constexpr double LAMBDA_MARKET = 3.0;
    constexpr double LAMBDA_CANCEL = 2.0;

    for (size_t i = 0; i < instrumentIDs.size(); i++) {
        books_.emplace_back(std::make_unique<OrderBook> (
            instrumentIDs[i],
            depths[i],
            LAMBDA_LIMIT,
            LAMBDA_MARKET,
            LAMBDA_CANCEL
        ));
    }
}

void Simulator::run(double horizon_seconds, double snapshot_interval) {
    while (true) {
        for (const auto& ob: books_) {
            ob->simulateSteps(horizon_seconds, snapshot_interval);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
