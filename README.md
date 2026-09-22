<div align="center">

<a href="https://github.com/Nanjundi15/AeroTask">
  <img src="https://capsule-render.vercel.app/api?type=waving&height=210&color=0:0F172A,50:1D4ED8,100:06B6D4&text=AeroTask&fontColor=FFFFFF&fontSize=58&fontAlignY=40&desc=High-Performance%20Distributed%20Task%20Scheduler%20in%20C&descAlignY=62&descSize=18&animation=fadeIn" alt="AeroTask banner"/>
</a>

<br/>

<img src="https://readme-typing-svg.demolab.com?font=JetBrains+Mono&size=19&duration=2800&pause=900&color=38BDF8&center=true&vCenter=true&width=820&lines=Multithreaded+%7C+TCP%2FIP+%7C+SQLite+%7C+Priority+Queue;Retry+%2B+Exponential+Backoff+%7C+Dead-Letter+Handling;Built+for+Systems%2C+Backend+%26+Cloud+Engineering+Learning" alt="Animated AeroTask tagline"/>

<br/><br/>







<br/>

<a href="https://github.com/Nanjundi15/AeroTask/stargazers">⭐ Star</a>
  •  
<a href="https://github.com/Nanjundi15/AeroTask/issues">🐛 Issues</a>
  •  
<a href="https://github.com/Nanjundi15/AeroTask/actions">⚙️ CI</a>

</div>

🚀 What is AeroTask?

AeroTask is a lightweight, high-performance distributed task scheduler written in C.

It is designed as a practical systems-engineering project that demonstrates how a task can travel through a real execution pipeline:

Client
  │
  ▼
TCP Server
  │
  ▼
Priority Queue
  │
  ▼
Worker Pool (4 pthread workers)
  │
  ├──► Success ───────────────► SQLite
  │
  └──► Failure
         │
         ▼
    Exponential Backoff
         │
         ├──► Retry
         │
         └──► Dead-Letter State

AeroTask intentionally keeps the infrastructure local, so you can build and demonstrate the core concepts without requiring a paid cloud subscription.

✨ Why this project?

AeroTask was built to explore the engineering problems behind production-style background job systems:

Problem

AeroTask approach

Concurrent task execution

POSIX threads + worker pool

Fast task dispatch

In-memory priority queue

Client/server communication

TCP sockets

Failed jobs

Retry policy + exponential backoff

Permanently failing jobs

Dead-letter state

Persistent state

SQLite

Operational visibility

Health, status and statistics commands

Repeatable builds

Make + CMake

Automated validation

Queue unit test + GitHub Actions

🧠 Core Features

⚡ Priority-based scheduling

Tasks carry a priority value and are inserted into the queue based on priority.

Priority 10  → Critical
Priority 5   → Medium
Priority 1   → Low

🧵 Multithreaded worker pool

AeroTask starts 4 worker threads using pthread.

           ┌───────────────┐
           │ Priority Queue│
           └───────┬───────┘
                   │
       ┌───────────┼───────────┐
       ▼           ▼           ▼           ▼
   Worker 1    Worker 2    Worker 3    Worker 4

🔁 Automatic retries

Failed tasks are retried with exponential backoff:

Attempt 1 → 1s
Attempt 2 → 2s
Attempt 3 → 4s
      ↓
    DEAD

💀 Dead-letter handling

After the configured retry limit is reached, the task moves to DEAD instead of retrying forever.

💾 SQLite persistence

Task metadata and execution state are persisted in:

data/aerotask.db

🌐 TCP client/server

The client communicates with the scheduler over TCP:

aerotask-client  ───── TCP ─────►  aerotask-server

🩺 Health & operations

Built-in commands provide basic observability:

health
status <task_id>
list
stats
submit <priority> <task_name>

🏗️ Architecture

flowchart LR
    A[CLI Client] -->|TCP| B[Network Server]
    B --> C[Scheduler]
    C --> D[Priority Queue]
    D --> E1[Worker 1]
    D --> E2[Worker 2]
    D --> E3[Worker 3]
    D --> E4[Worker 4]
    E1 --> F[Task Execution]
    E2 --> F
    E3 --> F
    E4 --> F
    F --> G{Success?}
    G -->|Yes| H[SQLite]
    G -->|No| I[Retry + Backoff]
    I -->|Retry Available| D
    I -->|Retry Limit Reached| J[DEAD]
    J --> H

📁 Project Structure

AeroTask/
├── .github/
│   └── workflows/
│       └── ci.yml
├── client/
│   └── client.c
├── docs/
│   └── architecture.md
├── include/
│   ├── common.h
│   ├── logger.h
│   ├── network.h
│   ├── queue.h
│   ├── scheduler.h
│   └── storage.h
├── scripts/
│   └── run_demo.sh
├── src/
│   ├── common.c
│   ├── logger.c
│   ├── network.c
│   ├── queue.c
│   ├── scheduler.c
│   ├── server.c
│   └── storage.c
├── tests/
│   └── test_queue.c
├── CMakeLists.txt
├── Dockerfile
├── Makefile
├── README.md
└── .gitignore

🛠️ Tech Stack

Layer

Technology

Language

C11

Concurrency

POSIX Threads (pthread)

Networking

TCP/IP sockets

Scheduling

Priority queue

Persistence

SQLite3

Build

GNU Make, CMake

Testing

C test executable

CI

GitHub Actions

Environment

Ubuntu / WSL2

Containerization

Docker

💻 Run Locally

Prerequisites

Ubuntu / WSL2

GCC

Make

SQLite3 development libraries

Git

Install dependencies

sudo apt update
sudo apt install build-essential sqlite3 libsqlite3-dev git -y

Clone

git clone https://github.com/Nanjundi15/AeroTask.git
cd AeroTask

Build

make

Run tests

make test

Expected:

test_queue: PASS

Start the server

mkdir -p data
./build/aerotask-server 8080

Expected:

AeroTask server listening on port 8080

🎮 Demo

Open a second terminal.

1. Health check

./build/aerotask-client 127.0.0.1 8080 health

Expected:

OK HEALTH queue=0

2. Submit a task

./build/aerotask-client 127.0.0.1 8080 submit 10 "critical_backup"

Expected:

OK task_id=1 status=PENDING

3. Check status

./build/aerotask-client 127.0.0.1 8080 status 1

Example:

TASK 1 | COMPLETED | priority=10 | retries=0/3 | worker=2 | error=

4. Test retry + dead-letter handling

A task name containing fail intentionally triggers the deterministic failure path used for testing.

./build/aerotask-client 127.0.0.1 8080 submit 10 "fail_retry_demo"

After the retry cycle:

TASK 2 | DEAD | priority=10 | retries=3/3 | worker=1 | error=Simulated worker failure

5. View statistics

./build/aerotask-client 127.0.0.1 8080 stats

Statistics are printed on the server console.

Example:

Task statistics:
  COMPLETED   1
  DEAD        1

🧪 Testing

The project includes a queue unit test:

make test

Current verified test result:

test_queue: PASS

The end-to-end demo has also been verified for:

✅ TCP client/server communication

✅ Task submission

✅ Worker execution

✅ SQLite persistence

✅ Priority values

✅ Retry handling

✅ Exponential backoff

✅ Dead-letter state

✅ 4-worker execution

✅ Health checks

📊 Task Lifecycle

                ┌──────────┐
                │ PENDING  │
                └────┬─────┘
                     │
                     ▼
                ┌──────────┐
                │ RUNNING  │
                └────┬─────┘
                     │
              ┌──────┴──────┐
              ▼             ▼
         ┌──────────┐   ┌───────────┐
         │COMPLETED │   │  FAILED   │
         └──────────┘   └─────┬─────┘
                               │
                               ▼
                         ┌──────────┐
                         │ RETRYING │
                         └────┬─────┘
                              │
                    retries remaining?
                       /           \
                     yes            no
                      │              │
                      ▼              ▼
                  PENDING          DEAD

🔐 Engineering & Safety Notes

AeroTask's failure demonstration is deterministic and based on the task name.

For example:

fail_demo
fail_retry_demo
fail_final

The scheduler does not execute arbitrary shell commands from submitted task names.

This keeps the demonstration predictable and avoids turning the scheduler into an arbitrary command-execution mechanism.

🐳 Docker

Build the image:

docker build -t aerotask .

Run:

docker run --rm -p 8080:8080 aerotask

Docker support is included as a packaging/deployment option; the primary development environment is Ubuntu/WSL2.

🔄 CI

GitHub Actions configuration is included at:

.github/workflows/ci.yml

The CI workflow is intended to validate the build and automated tests on pushes and pull requests.

🎯 Engineering Concepts Demonstrated

AeroTask is intentionally focused on fundamentals that transfer to backend, cloud, systems and infrastructure engineering:

Concurrency
   ↓
Thread synchronization
   ↓
Producer / consumer scheduling
   ↓
Priority-based dispatch
   ↓
Network protocols
   ↓
Failure recovery
   ↓
Persistence
   ↓
Testing + CI

This makes the project useful as a practical demonstration of systems programming + backend engineering + reliability concepts.

🚧 Future Enhancements

Planned directions:

Worker heartbeat and health monitoring

Graceful server shutdown with queue draining

Configurable worker count

Configurable retry policies

Metrics endpoint

Structured JSON logs

Authentication for client connections

Rate limiting

REST API gateway

Redis-backed distributed queue

Cloud deployment

Benchmark suite and latency measurements

Integration-test framework

📚 Learning Outcomes

Building AeroTask provides hands-on experience with:

C systems programming

POSIX threads

Mutexes and condition variables

TCP sockets

Scheduling algorithms

Priority queues

SQLite database operations

Retry and backoff strategies

Fault handling

Build automation

Docker

CI/CD fundamentals

👨‍💻 Author

<div align="center">

Nanjundi K

Software Engineer | AI/ML | Cloud | Backend | Systems

<a href="https://github.com/Nanjundi15">
  <img src="https://img.shields.io/badge/GitHub-Nanjundi15-181717?style=for-the-badge&logo=github" alt="GitHub"/>
</a>

</div>

⭐ Support

If AeroTask is useful for learning or reference, consider giving the repository a ⭐ on GitHub.

<div align="center">

Built with C, pthreads, sockets, SQLite and a lot of debugging.

<img src="https://capsule-render.vercel.app/api?type=waving&height=120&section=footer&color=0:06B6D4,50:1D4ED8,100:0F172A&animation=fadeIn" alt="AeroTask footer"/>

</div>

📄 License

A license has not been added to the repository yet. Add a LICENSE file when you decide which license you want to use.
