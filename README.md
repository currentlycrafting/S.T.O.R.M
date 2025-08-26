# S.T.O.R.M – Scalable, Thread-Safe, Orchestrated, Replicated Memory-store

⚡️ A prototype single-node key-value store built for concurrency, real-time visualization, and future distributed replication.

**Tech Stack:** C++17, gRPC, React, Docker, Envoy

----------

## System Architecture

S.T.O.R.M's architecture is built around a multi-component system that handles requests from a web browser, translates them, and directs them to a thread-safe backend.

-   **Frontend**: A React-based UI that provides an interactive interface for `PUT`, `GET`, and `DELETE` operations. It also dynamically displays the key-value pairs in real time.
    
-   **Envoy Proxy**: This proxy acts as a crucial intermediary, translating gRPC-Web requests from the browser into standard gRPC requests that the backend server can understand.
    
-   **Backend**: The core of the system, this is a C++ server with a thread-safe, in-memory key-value store. It uses `std::mutex` to ensure that concurrent operations don't corrupt the data.
    
-   **Proto Definitions**: The `storm.proto` file defines the gRPC services and messages, serving as the contract between the frontend and backend.
    

----------

## Features

-   **Thread-Safe KV Store**: The backend supports concurrent read and write operations on the key-value store using C++17's `std::mutex`.
    
-   **High-Performance gRPC API**: Utilizes gRPC for efficient, high-performance remote procedure calls between services.
    
-   **Real-Time Visualization**: The React frontend provides a live, interactive view of the stored key-value pairs as they are added, updated, or deleted.
    
-   **Dockerized Deployment**: The entire stack, including the backend, frontend, and Envoy proxy, can be deployed and run with a single `docker-compose up` command.
    
-   **Extensible for Raft**: The system's design is modular, making it easy to integrate a **Raft consensus algorithm** for future distributed replication and fault tolerance.
    

----------

## Protobuf Definitions

The `storm.proto` file defines all the necessary RPC services and messages for the key-value store.

Protocol Buffers

```
syntax = "proto3";
package storm;

service KeyValueStore {
  rpc Get (GetRequest) returns (GetResponse);
  rpc Put (PutRequest) returns (PutResponse);
  rpc Delete (DeleteRequest) returns (DeleteResponse);
}

message GetRequest {
  string key = 1;
}
message GetResponse {
  string value = 1;
  bool success = 2;
}
message PutRequest {
  string key = 1;
  string value = 2;
}
message PutResponse {
  bool success = 1;
}
message DeleteRequest {
  string key = 1;
}
message DeleteResponse {
  bool success = 1;
}

```

----------

## Getting Started

### Backend

The backend server is written in C++17 and uses gRPC and Protobuf.

**Dependencies:** gRPC, Protobuf

**Concurrency:** `std::mutex` for thread safety

**Local Build & Run:**

Bash

```
cd STORM_Backend
mkdir build && cd build
cmake ..
make
./storm_server

```

The server will launch on port `50051`.

----------

### Frontend

The frontend is a React application that interacts with the backend via gRPC-Web.

**Dependencies:** `google-protobuf`, `grpc-web`

**Local Setup & Run:**

Bash

```
cd STORM_Frontend
npm install
npm start

```

Access the frontend in your browser at `http://localhost:3000`.

----------

### Docker Setup

The `docker-compose.yml` file orchestrates all three services: `backend`, `envoy`, and `frontend`.

**Run All Services:**

Bash

```
docker-compose up --build

```

**Service Endpoints:**

-   **Frontend:** `http://localhost:3000`
    
-   **Envoy:** `http://localhost:8080`
    
-   **Backend gRPC:** `localhost:50051`
    

----------

## Usage

-   **`PUT`**: Store a new key-value pair.
    
-   **`GET`**: Retrieve a value by its key.
    
-   **`DELETE`**: Remove a key-value pair from the store.
    

All operations are visualized in real-time on the React frontend, which updates the display of the key-value pairs dynamically.

----------

## Development Notes

-   gRPC-Web requires a proxy like **Envoy** for browser compatibility.
    
-   Generated JavaScript files for Protobuf (`storm_pb.js`, `storm_grpc_web_pb.js`) must be present in `src/proto`.
    
-   The current backend is a single-node implementation; future plans include integrating **Raft consensus** for distributed replication.
    
-   The backend logs all operations to the console, which is useful for debugging.
