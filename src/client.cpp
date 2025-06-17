#include "grpc++/grpc++.h"
#include "market_data.grpc.pb.h"
#include "orderbook.cpp"
#include <thread>

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReaderWriter;
using market_simulator::MarketData;

void runClient(std::shared_ptr<Channel> channel) {
    auto stub = MarketData::NewStub(channel);
    ClientContext context;
    auto stream = stub->StreamData(&context);

    // Send requests
    market_simulator::StreamRequest request;
    auto* subscription = request.mutable_sub();
    subscription->set_action(market_simulator::Subscription::SUBSCRIBE);
    subscription->set_instrument_id(1);
    stream->Write(request);

    // Read resposnses asynchronously
    std::thread reader([&] () {
        market_simulator::StreamResponse response;

        while (stream->Read(&response)) {
            if (response.has_snap()) {
                market_simulator::Snapshot snap;
            }
            else if (response.has_inc()) {
                market_simulator::Incremental increment_;
            }
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    subscription->set_action(market_simulator::Subscription::UNSUBSCRIBE);
    stream->Write(request);
    stream->WritesDone();
    reader.join();
    stream->Finish();
}

int main() {
    runClient(grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials()));
    return 0;
}
