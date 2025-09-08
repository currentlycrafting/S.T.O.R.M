# S.T.O.R.M – Scalable, Thread-Safe, Orchestrated, Replicated Memory-store

S.T.O.R.M is a **high-performance, in-memory key-value store** implemented in C++17. It is designed for **high-concurrency workloads**, supporting sharded LRU eviction, thread-safe operations, batch writes, and an interactive CLI interface. Ideal for applications requiring fast, scalable memory storage.

---

## Tech Stack

- **Language:** C++17  
- **Testing:** GoogleTest  
- **Build System:** CMake  
- **Containerization:** Docker (Ubuntu 22.04 base)

---

## Features

- **Thread-Safe Operations:** Each shard uses `std::mutex` to minimize lock contention for multi-threaded `PUT`, `GET`, and `DEL` operations.  
- **Sharding:** Keys are distributed across multiple shards to reduce bottlenecks, with independent LRU eviction per shard.  
- **LRU Eviction:** Automatically removes the least recently used entries when a shard reaches capacity.  
- **Batch Operations:** Supports `putMany` for efficient batch writes, reducing lock overhead.  
- **CLI Interface:** Interactive command-line tool with commands: `PUT`, `GET`, `DEL`, `LIST`, `CLEAR`, `HISTORY`, `HELP`, and `EXIT`.  
- **High-Concurrency Testing:** Designed to handle 100+ threads performing millions of operations reliably.

---


## 📊 Performance Metrics

**Test Configuration:**

| Parameter | Value |
|-----------|-------|
| Number of Shards | 30 |
| Shard Capacity | 4,000 keys |
| Threads | 350 |
| Ops per Thread | 300,000 |
| Total Ops | 105,000,000 |
| Runtime | ~75 seconds |
| Throughput | ~1.39M ops/sec |

**Interpretation:**

- High throughput demonstrates effective **sharding** and **mutex-per-shard** design.  
- LRU eviction works correctly under heavy load.  
- Recency updates are accurate on both `GET` and `PUT`.

**Performance Graph (Ops/sec vs Threads)**

<img width="1221" height="864" alt="Screenshot 2025-09-06 at 5 05 04 PM" src="https://github.com/user-attachments/assets/20b7a031-2b62-410c-b2a9-78ae79be1c1e" />


## Stress-test

[==========] Running 8 tests from 1 test suite.
[ RUN      ] StoreTest.ConcurrencyStress
[       OK ] StoreTest.ConcurrencyStress (75501 ms)
[  PASSED  ] 8 tests.


## Getting Started

### Clone the Repository

```bash
git clone https://github.com/yourusername/storm.git
cd storm
```

### Build Locally

```bash
mkdir build
cd build
cmake ..
make
```

### Run CLI

```bash
./server
```

### Docker Usage

**Build Docker Image**

```bash
docker build -t storm-store .
```

**Run Container**

```bash
docker run -it --rm storm-store
```

---

## Example CLI Commands

```bash
> PUT foo bar
{ "success": true }

> GET foo
{ "success": true, "value": "bar" }

> LIST
{ "shard_0": { "foo": "bar" } }

> DEL foo
{ "success": true }

> HISTORY
{ "history": ["PUT foo bar", "GET foo", "LIST", "DEL foo"] }
```

---

## Testing

Run all unit tests with GoogleTest:

```bash
mkdir build
cd build
cmake ..
make
./tests
```

---

## Contributing

- Follow **clean coding practices**: descriptive variable names, consistent indentation, and clear comments.  
- Use `@brief` comment style for new functions.  
- Ensure **thread-safety** in all new operations.  
- Submit pull requests with clear descriptions and maintain compatibility with existing tests.

---

```
