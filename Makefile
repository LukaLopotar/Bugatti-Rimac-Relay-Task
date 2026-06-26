CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -D_POSIX_C_SOURCE=199309L
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

TARGET = main
SRC = $(SRC_DIR)/main.c \
			$(SRC_DIR)/relay_controller.c \
			$(SRC_DIR)/relays_config.c \
			$(SRC_DIR)/hal_dio.c \
			$(SRC_DIR)/simulation_config.c

all:
	mkdir -p $(BUILD_DIR)
	
	$(CC) -I$(INCLUDE_DIR) $(CFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET)

run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
