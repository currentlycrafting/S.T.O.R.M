cmake_minimum_required(VERSION 3.10)
project(STORM CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)  # requires installed gRPC cmake config files

set(PROTO_FILE "${CMAKE_CURRENT_SOURCE_DIR}/proto/storm.proto")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Generate protobuf + grpc sources
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_FILE})

add_executable(storm_server
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/kv_store.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
)

target_include_directories(storm_server PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(storm_server
    PRIVATE
    gRPC::grpc++
    protobuf::libprotobuf
)
