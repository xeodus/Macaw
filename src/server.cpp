#include "grpc++/grpc++.h"
#include "market_data.grpc.pb.h"
#include "simulator.hh"
#include <thread>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using market_simulator::MarketData;

class MarketDataServiceImpl final : public MarketData::Service {
public:
    Status GetConfig(ServerContext*,
        const market_simulator::ConfigRequest*,
        market_simulator::ConfigResponse* response
    )
    override
    {
        return Status::OK;
    }

    Status StreamData(ServerContext* context,
        grpc::ServerReaderWriter<market_simulator:: StreamResponse, market_simulator::StreamRequest>* stream
    ) override
    {
        return Status::OK;
    }

};

int main() {
    std::vector<int> ids = {1};
    std::vector<int> depths = {5};

    Simulator simulator(ids, depths);
    std::thread simulatorThread(&Simulator::run, &simulator);
    std::string serverAddress("0.0.0.0:50051");
    MarketDataServiceImpl service;

    ServerBuilder builder;
    builder.AddListeningPort(serverAddress, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    server->Wait();

    simulatorThread.join();
    return 0;
}
