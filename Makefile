CC ?= gcc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude
LDFLAGS ?= -lpthread -lsqlite3
SRC = src/common.c src/logger.c src/queue.c src/storage.c src/scheduler.c src/network.c
BUILD = build

all: $(BUILD)/aerotask-server $(BUILD)/aerotask-client $(BUILD)/test_queue

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/aerotask-server: $(SRC) src/server.c | $(BUILD)
	$(CC) $(CFLAGS) $(SRC) src/server.c -o $@ $(LDFLAGS)

$(BUILD)/aerotask-client: $(SRC) client/client.c | $(BUILD)
	$(CC) $(CFLAGS) $(SRC) client/client.c -o $@ $(LDFLAGS)

$(BUILD)/test_queue: $(SRC) tests/test_queue.c | $(BUILD)
	$(CC) $(CFLAGS) $(SRC) tests/test_queue.c -o $@ $(LDFLAGS)

test: $(BUILD)/test_queue
	./$(BUILD)/test_queue

clean:
	rm -rf $(BUILD)
