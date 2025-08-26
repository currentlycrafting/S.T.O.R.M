#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "kv_store.h"
#include "storm.grpc.pb.h"

using namespace storm;

class KeyValueStoreServiceImpl final : public KeyValueStore::Service {
public:
    grpc::Status Get(grpc::ServerContext* context, const GetRequest* request, GetResponse* response) override {
        std::string value;
        if (kv_store_.get(request->key(), &value)) {
            response->set_value(value);
            response->set_success(true);
            return grpc::Status::OK;
        }
        response->set_success(false);
        return grpc::Status::OK;
    }

    grpc::Status Put(grpc::ServerContext* context, const PutRequest* request, PutResponse* response) override {
        kv_store_.put(request->key(), request->value());
        response->set_success(true);
        return grpc::Status::OK;
    }

    grpc::Status Delete(grpc::ServerContext* context, const DeleteRequest* request, DeleteResponse* response) override {
        if (kv_store_.del(request->key())) {
            response->set_success(true);
        } else {
            response->set_success(false);
        }
        return grpc::Status::OK;
    }

private:
    KVStore kv_store_;
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    KeyValueStoreServiceImpl service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "S.T.O.R.M. server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char** argv) {
    RunServer();
    return 0;
}
