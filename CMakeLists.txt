cmake_minimum_required(VERSION 3.15)
project(MarketDataSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use absolute paths for everything
set(HOMEBREW_PREFIX "/opt/homebrew")
set(PROTOC_PATH "${HOMEBREW_PREFIX}/bin/protoc")
set(GRPC_CPP_PLUGIN_PATH "${HOMEBREW_PREFIX}/bin/grpc_cpp_plugin")

include_directories(${HOMEBREW_PREFIX}/include)
include_directories(SYSTEM ${HOMEBREW_PREFIX}/include)

# Verify tools exist
if(NOT EXISTS "${PROTOC_PATH}")
    message(FATAL_ERROR "protoc not found at: ${PROTOC_PATH}")
endif()
message(STATUS "Using protoc: ${PROTOC_PATH}")

if(NOT EXISTS "${GRPC_CPP_PLUGIN_PATH}")
    message(FATAL_ERROR "grpc_cpp_plugin not found at: ${GRPC_CPP_PLUGIN_PATH}")
endif()
message(STATUS "Using grpc_cpp_plugin: ${GRPC_CPP_PLUGIN_PATH}")

# Set directories
set(PROTO_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${GENERATED_DIR}")

set(PROTO_FILE "${PROTO_SRC_DIR}/market_data.proto")

# Output files
set(PROTO_SRCS
    "${GENERATED_DIR}/market_data.pb.cc"
    "${GENERATED_DIR}/market_data.grpc.pb.cc"
)
set(PROTO_HDRS
    "${GENERATED_DIR}/market_data.pb.h"
    "${GENERATED_DIR}/market_data.grpc.pb.h"
)

# Generate protobuf and gRPC code using a script
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/generate_proto.sh"
"#!/bin/bash
\"${PROTOC_PATH}\" \\
    --proto_path=\"${PROTO_SRC_DIR}\" \\
    --cpp_out=\"${GENERATED_DIR}\" \\
    --grpc_out=\"${GENERATED_DIR}\" \\
    --plugin=protoc-gen-grpc=\"${GRPC_CPP_PLUGIN_PATH}\" \\
    \"${PROTO_FILE}\"
")

# Make the script executable
execute_process(COMMAND chmod +x "${CMAKE_CURRENT_BINARY_DIR}/generate_proto.sh")

# Custom command to generate proto files
add_custom_command(
  OUTPUT ${PROTO_SRCS} ${PROTO_HDRS}
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/generate_proto.sh"
  DEPENDS "${PROTO_FILE}"
  COMMENT "Generating Protobuf and gRPC sources"
  VERBATIM
)

add_custom_target(generate_proto DEPENDS ${PROTO_SRCS} ${PROTO_HDRS})

# Find packages - this is the key fix
find_package(gRPC CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)

# Build proto library
add_library(proto STATIC ${PROTO_SRCS})
add_dependencies(proto generate_proto)

target_include_directories(proto PUBLIC
    ${GENERATED_DIR}
    ${PROTO_SRC_DIR}
    ${HOMEBREW_PREFIX}/include
)

# Fixed linking - use proper CMake targets
target_link_libraries(proto
    PUBLIC
        gRPC::grpc++
        gRPC::grpc
        protobuf::libprotobuf
        absl::log
        absl::log_internal_message
        absl::log_internal_check_op
        absl::strings
        absl::cord
        absl::synchronization
        absl::base
        absl::memory
)

# Server target
add_executable(server
    src/server.cpp
    src/simulator.cpp
    src/orderbook.cpp
)
add_dependencies(server generate_proto)
target_include_directories(server PRIVATE include ${GENERATED_DIR})
target_link_libraries(server PRIVATE proto)

# Client target
add_executable(client
    src/client.cpp
)
add_dependencies(client generate_proto)
target_include_directories(client PRIVATE include ${GENERATED_DIR})
target_link_libraries(client PRIVATE proto)