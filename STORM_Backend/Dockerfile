# Use official Ubuntu LTS image
FROM ubuntu:22.04

# Set non-interactive mode for apt-get
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    git \
    wget \
    libgtest-dev \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# Build GoogleTest
RUN cd /usr/src/gtest && cmake . && make && cp lib/*.a /usr/lib

# Set working directory
WORKDIR /app

# Copy project files
COPY . .

# Build the project
RUN cmake -S . -B build \
    && cmake --build build -- -j$(nproc)

# Default command to start the server CLI
CMD ["./build/server"]
