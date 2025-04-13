#pragma once
#include "common.hh"
#include <map>
#include <thread>

namespace market_data {
    namespace server {
        class OrderBook {
        public:
            OrderBook(const std::string& instrument_id, const std::string& symbol, int32_t depth = 10, double base_price = 100.0);
            ~OrderBook();

            OrderBookSnapshot get_snapshot();

        private:
            std::string instrument_id_;
            std::string symbol_;
            int32_t depth_;
            double base_price_;
            std::atomic<int32_t> sequence_number_;
            std::map<std::string, PriceLevel> bids_;
            std::map<std::string, PriceLevel> asks_;
            std::mutex mtx;
            std::atomic<bool> running_;
            std::unique_ptr<std::thread> update_thread_;

            void initialize_orderbook();
            // Generate a single update
            OrderBookUpdate generate_update();
            // Runs a background thread to simualte market activity and generate updates
            void generate_updates();
        };
    }
}