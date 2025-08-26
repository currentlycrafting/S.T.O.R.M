FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config \
    wget \
    curl \
    protobuf-compiler \
    libprotobuf-dev \
    libprotoc-dev \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Build and install gRPC
WORKDIR /tmp
RUN git clone --recurse-submodules -b v1.56.2 https://github.com/grpc/grpc && \
    cd grpc && mkdir -p cmake/build && cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF ../.. && \
    make -j$(nproc) && make install

WORKDIR /app
COPY . /app

RUN mkdir -p build && cd build && cmake .. && cmake --build . -- -j$(nproc)

EXPOSE 50051
CMD ["./build/storm_server"]
