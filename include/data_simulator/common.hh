#pragma once
#include <cstdint>
#include <vector>

namespace market_data {

struct PriceLevel {
    double price;
    double quantity;
    int32_t order_count;
};

enum class UpdateAction {
    ADD,
    UPDATE,
    DELETE
};

struct PriceLevelUpdate {
    double price;
    double quantity;
    int32_t order_count;
    UpdateAction action;
};

struct OrderBookSnapshot {
    std::vector<PriceLevel> bids;
    std::vector<PriceLevel> asks;
    int64_t timestamp;
};

struct OrderBookUpdate {
    std::vector<PriceLevelUpdate> bids;
    std::vector<PriceLevelUpdate> asks;
    int64_t timestamp;
    int64_t sequence_number;
};

struct ContractSpecification {
    int32_t orderbook_depth;
};

struct Instrument {
    std::string symbol;
    std::string order_id;
    ContractSpecification contract_specification;
};

}