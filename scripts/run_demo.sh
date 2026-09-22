#!/usr/bin/env bash
set -e
make
mkdir -p data
./build/aerotask-server 8080 &
SERVER_PID=$!
trap 'kill $SERVER_PID 2>/dev/null || true' EXIT
sleep 1
./build/aerotask-client 127.0.0.1 8080 submit 10 "critical_backup"
./build/aerotask-client 127.0.0.1 8080 submit 5 "normal_job"
./build/aerotask-client 127.0.0.1 8080 submit 9 "fail_demo"
./build/aerotask-client 127.0.0.1 8080 health
./build/aerotask-client 127.0.0.1 8080 list
