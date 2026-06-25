CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -D_POSIX_C_SOURCE=199309L
BUILD_DIR = build

TARGET = main
SRC = main.c sleep_ms.c

all:
	mkdir -p $(BUILD_DIR)
	
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET)

run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)