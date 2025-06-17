# Simple Makefile for the C shell project

CC ?= gcc
CFLAGS ?= -Wall -Wextra -std=c11 -g
LDFLAGS ?= -lreadline

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET := $(BIN_DIR)/shell

# Default target
all: $(TARGET)

$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean 