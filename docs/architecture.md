# AeroTask Architecture

## Components

- TCP server: receives commands from clients.
- Scheduler: owns the concurrent task queue.
- Priority queue: higher priority tasks are selected first.
- Worker pool: executes safe simulated workloads concurrently.
- Retry manager: requeues failed tasks with exponential backoff.
- SQLite storage: persists task state.
- Logger: timestamped operational logs.

## Concurrency model

The server starts four worker threads. Workers block on a condition variable when the queue is empty. Queue operations are protected by a mutex.

## Failure handling

Tasks whose name contains `fail` intentionally simulate failure for testing. They are retried up to the configured maximum and then moved to DEAD state.

## Security note

The server does not execute arbitrary client-provided shell commands. Work is simulated by the worker function. This is intentional for a safe portfolio project.
