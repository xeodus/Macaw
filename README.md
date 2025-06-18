# Market Data Disseminator Simulation

## Introduction

![C++](https://img.shields.io/badge/C%2B%2B-0D47A1?style=flat&logo=c%2B%2B&logoColor=white&labelColor=FF272A)

This is an implementation of a simulation engine using gRPC bi-directional stream which has a server side and a client side application that sends subscribe/unsubscribe request and receives order book snapshots, incremental updates, and simulates real-world high frequency trading events.

## Features

- [x] Designed & implemented a high‑throughput C++/gRPC simulator that generates live order‑book snapshots and incremental updates for N instruments.
- [x] Authored a lock‑free, thread‑safe OrderBook engine in C++17, using fine‑grained synchronization and batch‐update strategies to minimize latency and contention under heavy subscription loads.
- [x] Built a gRPC streaming service with bi‑directional RPC: clients send SUBSCRIBE/UNSUBSCRIBE requests and receive full snapshots on subscribe, then incremental deltas—achieving end‑to‑end delivery in <1 ms over 1 Gbps LAN.
- [x] Automated code generation pipeline via CMake, ```protoc``` and ```grpc_cpp_plugin``` to produce ```market_data.pb.{h,cc}``` and ```market_data.grpc.pb.{h,cc}```, ensuring zero‑drift between .proto API and C++ types.

## Requirements

- [x] C++17 or above
- [x] gRPC
- [x] Protobuf

## Setup guide

- Clone Protobuf directly via GitHub repo using

``` bash
    git clone https://github.com/protocolbuffers/protobuf.git
    cd protobuf
    git checkout v21.12

    mkdir -p build && cd build

    cmake ../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -Dprotobuf_BUILD_TESTS=OFF

    make -j$(nproc)    # or $(sysctl -n hw.logicalcpu) on macOS
    sudo make install

    # Verify
    protoc --version
```

Similarly, to get gRPC, we can use:

```bash
    git clone -b v1.57.0 https://github.com/grpc/grpc.git
    cd grpc

    git submodule update --init --recursive

    mkdir -p build && cd build
    cmake ../.. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local

    make -j$(nproc)
    sudo make install

    # Verify
    which grpc_cpp_plugin
```

If you are on MacOS, you can simply use:

```bash
    brew install grpc protobuf

    # Verify their paths and version
    which protoc
    protoc --version

    which grpc_cpp_plugin
```

Now, finally to build and run the program:

```bash
    mkdir build && cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    cmake --build .
```
Cheers 🍻

Project under development..