#pragma once
#include "config.hh"
#include "common.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include "json.hh"

namespace market_data {
    namespace server {
        std::map<std::string, Instrument> ConfigLoader::load_config(const std::string& config_path) {
            std::map<std::string, Instrument> instruments;

            try {
                std::ifstream config_file(config_path);

                if (!config_file.is_open()) {
                    std::cerr << "Failed to open the configuration file: " << config_path << "\n";
                    return instruments;
                }

                // JSON parser
                nlohmann::json config_json;
                config_file >> config_json;

                for (const auto& item: config_json["instruments"]) {
                    Instrument instrument;
                    instrument.order_id = item["order_id"];
                    instrument.symbol = item["symbol"];

                    instrument.contract_specification.orderbook_depth =
                    (item.contains("orderbook_depth") ? item["orderbook_depth"].get<int32_t>() : 10);

                    instruments[instrument.order_id] = instrument;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error loading config file: " << e.what() << "\n";
            }

            return instruments;
        }
    }
}