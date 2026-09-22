cmake_minimum_required(VERSION 3.16)
project(aerotask C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(SQLite3 REQUIRED)

include_directories(include)

add_library(aerotask_core
    src/common.c src/logger.c src/queue.c src/storage.c src/scheduler.c src/network.c
)

target_link_libraries(aerotask_core PRIVATE Threads::Threads SQLite::SQLite3)

add_executable(aerotask-server src/server.c)
target_link_libraries(aerotask-server PRIVATE aerotask_core)

add_executable(aerotask-client client/client.c)
target_link_libraries(aerotask-client PRIVATE aerotask_core)

add_executable(test_queue tests/test_queue.c)
target_link_libraries(test_queue PRIVATE aerotask_core)

enable_testing()
add_test(NAME queue COMMAND test_queue)
