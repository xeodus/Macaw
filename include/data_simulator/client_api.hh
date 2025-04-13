#pragma once
#include <string>
#include <future>
#include "common.hh"

namespace market_data {

    namespace client {
        class MarketDataClient;
    }
    // Callback functions for snapshot, update and connection
    using SnapshotCallback = std::function<void(const std::string& instrument_id, const OrderBookSnapshot& snapshot)>;
    using UpdateCallback = std::function<void(const std::string& instrument_id, const OrderBookUpdate& update)>;
    using ConnectionCallback = std::function<void(bool connected, const std::string& message)>; 

    class ClientAPI {

    public:
        ClientAPI(std::string& server_address);
        ~ClientAPI();
        // Connect to the server using future that resolves to true if the connection was successful
        std::future<bool> connect();
        void disconnect();
        bool is_connected() const;
        // Using future for subscription mechanism using intrument ID
        std::future<bool> subscribe(std::string& instrument_id);
        // Unsubscribe using future
        std::future<bool> unsubscribe(std::string& instrument_id);
        void set_snapshot_callback(const SnapshotCallback& callback);
        void set_update_callback(const UpdateCallback& callback);
        void set_connection_callback(const ConnectionCallback& callback);
        std::string& get_endpoint() const;

    private:
        std::unique_ptr<client::MarketDataClient> client_;
        SnapshotCallback snapshot_callback_;
        UpdateCallback update_callback_;
        ConnectionCallback connection_callback_;
        bool connected_;
    };
}