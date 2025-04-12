#pragma once 
#include <map>
#include <string>
#include "common.hh"

namespace market_data {
    namespace server {

        class ConfigLoader {
        public:
            std::map<std::string, Instrument> load_config(std::string& config_path);
        };
    }
}