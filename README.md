# S.T.O.R.M. — Full-stack MVP (zipped)

This archive contains the full-stack MVP for the S.T.O.R.M. key-value store:
- C++ gRPC backend (STORM_Backend)
- Envoy gRPC-Web proxy config
- React frontend that uses grpc-web client (STORM_Frontend)
- docker-compose to run everything

IMPORTANT:
* You MUST generate the JS protobuf/grpc-web bindings before running the frontend. From `STORM_Backend/proto`:

  protoc -I=. storm.proto \
    --js_out=import_style=commonjs:../../STORM_Frontend/src/proto \
    --grpc-web_out=import_style=commonjs,mode=grpcwebtext:../../STORM_Frontend/src/proto

* Alternatively run backend/frontend locally — see original message for local steps.

