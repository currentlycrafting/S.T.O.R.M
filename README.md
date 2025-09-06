Of course. Here is the provided text formatted as a GitHub `README.md` file.

````markdown
# S.T.O.R.M – Scalable, Thread-Safe, Orchestrated, Replicated Memory-store

S.T.O.R.M is a high-performance, in-memory key-value store implemented in C++17. It supports sharded LRU eviction, thread-safe operations, and batch writes, making it ideal for high-concurrency workloads.

---

### Tech Stack

* **Language**: C++17
* **Testing**: GoogleTest
* **Build**: CMake
* **Containerization**: Docker (Ubuntu 22.04 base)

---

### Features

* **Thread-Safe Operations**: Individual shards are protected by `std::mutex` to minimize lock contention. It supports multi-threaded `PUT`, `GET`, and `DEL` operations.
* **Sharding**: Keys are distributed across multiple shards to reduce bottlenecks. Each shard has independent LRU eviction and capacity control.
* **LRU Eviction**: Automatically removes the least recently used entries when a shard reaches its capacity.
* **Batch Operations**: Supports `putMany` for efficient batch writes, which significantly reduces lock overhead.
* **CLI Interface**: An interactive command-line tool with commands: `PUT`, `GET`, `DEL`, `LIST`, `CLEAR`, `HISTORY`, `HELP`, and `EXIT`.
* **High-Concurrency Stress Testing**: Supports 100+ threads performing millions of operations, ensuring reliability under heavy load.

---

### Getting Started

#### Clone the repository

```bash
git clone [https://github.com/yourusername/storm.git](https://github.com/yourusername/storm.git)
cd storm
````

#### Build Locally

```bash
mkdir build
cd build
cmake ..
make
```

#### Run CLI

```bash
./server
```

#### Docker Usage

**Build Docker Image**

```bash
docker build -t storm-store .
```

**Run Container**

```bash
docker run -it --rm storm-store
```

-----

### Example CLI Commands

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

-----

### Testing

Run all unit tests with GoogleTest:

```bash
mkdir build
cd build
cmake ..
make
./tests
```

-----

### Contributing

  * Use descriptive variable names, proper indentation, and clear comments.
  * Follow `@brief` comment style for all new functions.
  * Ensure thread-safety for all new operations.

-----

### License

This project is licensed under the MIT License.

```
```