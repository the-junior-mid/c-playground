CC      := gcc
CFLAGS  := -Wall -Wextra -O2
BUILD   := build

SERVER  := $(BUILD)/rc-server
CLIENT  := $(BUILD)/rc-client

.PHONY: all clean

all: $(SERVER) $(CLIENT)

# The "|" makes $(BUILD) an order-only prerequisite:
# it must exist before the build, but its timestamp never triggers a rebuild.
$(SERVER): src/rc-server.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $<

$(CLIENT): src/rc-client.c | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)
