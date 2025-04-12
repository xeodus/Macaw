#pragma once 
#include <functional>

namespace market_data {

    namespace server {
        class MarketDataServer;
    }
    // Callback function for 
    using ServerEventCallback = std::function<void(const std::string& event, std::string& message)>;

    class ServerAPI {
        
    public:
        ServerAPI(const std::string& config_path);
        ~ServerAPI();
        // Initialize the server before starting
        bool initialize();
        bool start();
        bool stop();
        // Check if the server is up and running and return accordingly
        bool is_running() const;
        // callback function for server when the server event occurs
        void set_event_callback(const ServerEventCallback& call_back);
        // Return server's end point address
        std::string& get_endpoint() const;

    private:
        std::unique_ptr<server::MarketDataServer> server_;
        ServerEventCallback server_event_callback_;
        bool running_;
        std::string endpoint_;
    };
}