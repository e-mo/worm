# Project Configuration
PROJECT := worm
VERSION := 0.1.0
TARGET  := $(PROJECT)

# Shell/Make configuration
SHELL := /bin/bash
MAKEFLAGS += --no-print-directory
.SHELLFLAGS := -ec

BUILD_DIR := ./build
OBJ_ROOT := $(BUILD_DIR)/obj
SRC_ROOT := ./src

INC_DIRS := include $(SRC_ROOT)

INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CC ?= gcc
CFLAGS ?= -std=c23 -g -O0 -Wall -Wextra -pedantic -Wno-missing-field-initializers -D_POSIX_C_SOURCE=200809L
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
LDLIBS ?= -lgit2

SRCS := $(shell find $(SRC_ROOT) -type f -name '*.c')
SRCS_NO_ROOT := $(foreach src,$(SRCS),$(patsubst $(SRC_ROOT)/%,%,$(src)))

OBJS := $(patsubst %.c,%.o,$(addprefix $(OBJ_ROOT)/,$(SRCS_NO_ROOT)))
DEPS := $(OBJS:.o=.d)
OBJ_DIRS := $(sort $(dir $(OBJS))) 

INIT_DIRS := $(SRC_ROOT) ./include ./libs

BUILD_TARGET := $(BUILD_DIR)/$(TARGET)
ARGS ?=

all: $(BUILD_TARGET)
	
$(BUILD_TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(OBJS) -o $@ $(LDLIBS) 

$(OBJ_ROOT)/%.o: $(SRC_ROOT)/%.c | $(OBJ_DIRS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c  $< -o $@

$(OBJ_DIRS):
	mkdir -p $@

$(BUILD_DIR):
	mkdir -p $@

init: $(INIT_DIRS)

$(INIT_DIRS):
	mkdir -p $@

run: $(BUILD_TARGET)
	@$(CURDIR)/$(BUILD_TARGET)

clean:
	rm -rf $(OBJ_ROOT) $(BUILD_DIR)

.PHONY: all run clean init

-include $(DEPS)
