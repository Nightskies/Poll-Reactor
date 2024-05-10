
CC = gcc
CFLAGS = -Wall -Werror -Wextra

BUILD = client
BUILD_DIR = build
SRC_DIR = src
HEADERS_DIR = include
OBJ_DIR = $(BUILD_DIR)/obj

SRC = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(HEADERS_DIR)/*.h)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: clean

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -I$(HEADERS_DIR) $< -o $@

$(BUILD): $(BUILD_DIR) $(OBJ_DIR) $(OBJ)
	$(CC) -I$(HEADERS_DIR) $(OBJ) -o $(BUILD_DIR)/$@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BUILD_DIR)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)