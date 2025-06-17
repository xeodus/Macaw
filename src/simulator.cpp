#include "simulator.hh"
#include <chrono>
#include <thread>

Simulator::Simulator(std::vector<int>& instrumentIDs, std::vector<int>& depths) {
    for (size_t i = 0; i < instrumentIDs.size(); i++) {
        books_.emplace_back(std::make_unique<OrderBook>(instrumentIDs[i], depths[i]));
    }
}

void Simulator::run() {
    while (true) {
        for (const auto& ob: books_) {
            ob->simulateSteps();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
