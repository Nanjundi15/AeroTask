# AeroTask — High-Performance Task Scheduler in C

A production-style systems programming project demonstrating C, concurrency, TCP/IP networking, priority queues, retries, persistence, testing, Docker, and CI.

## Recommended environment

### Easiest on Windows
- Visual Studio Code + MinGW-w64 (GCC), or
- CLion + MinGW-w64.
- PyCharm can edit C files, but it is **not the best IDE for C**. PyCharm is primarily Python-focused.

### Recommended
Use **VS Code** with the C/C++ extension and GCC/MinGW-w64.

## Build on Linux / WSL

```bash
sudo apt update
sudo apt install build-essential sqlite3 libsqlite3-dev
make
./build/aerotask-server 8080
```

In another terminal:

```bash
./build/aerotask-client 127.0.0.1 8080 submit "backup_database" 10
./build/aerotask-client 127.0.0.1 8080 list
./build/aerotask-client 127.0.0.1 8080 stats
```

## Windows with MinGW

Install GCC/MinGW-w64 and SQLite development libraries. Then use:

```bash
mingw32-make
build\aerotask-server.exe 8080
```

Open a second terminal:

```bash
build\aerotask-client.exe 127.0.0.1 8080 submit "backup_database" 10
build\aerotask-client.exe 127.0.0.1 8080 list
build\aerotask-client.exe 127.0.0.1 8080 stats
```

## What this project demonstrates

- C structures and modular design
- Dynamic memory management
- Priority queue
- Thread pool
- Mutex and condition-variable synchronization
- TCP client/server sockets
- Task retry with exponential backoff
- Dead-letter handling
- SQLite persistence
- Structured logging
- Metrics
- Graceful shutdown
- Unit tests
- AddressSanitizer-ready build
- Docker
- GitHub Actions CI

## Project structure

```text
aerotask/
├── src/
├── include/
├── tests/
├── client/
├── docs/
├── scripts/
├── .github/workflows/
├── Makefile
├── CMakeLists.txt
├── Dockerfile
└── README.md
```

## Notes

The worker executes a safe built-in simulated workload instead of arbitrary shell commands. This avoids turning the demo into a remote-command-execution service.

## Roadmap

1. Run locally.
2. Read every module and understand it.
3. Add tests and benchmark results.
4. Add Docker.
5. Push to GitHub.
6. Optionally deploy later to Azure if you obtain a free/trial account.
