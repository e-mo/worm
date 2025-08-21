# Commented generic C Makefile. 
SHELL := /bin/bash 
# Silence Make directory traversal.
MAKEFLAGS  += --no-print-directory
.SHELLFLAGS = -ec

build_root  := build
object_root := $(build_root)/obj
source_root := src

include_dirs := $(source_root) include

include_flags := $(foreach inc_dir,$(include_dirs),-I./$(inc_dir))
define_flags := -D_POSIX_C_SOURCE=200809L

flags := $(include_flags) $(define_flags)
flags += -std=c23 -Wall -Wextra -pedantic -Wno-missing-field-initializers -lgit2 

sources := $(shell find $(source_root) -type f -name '*.c')
sources_no_root := $(foreach src,$(sources),$(patsubst $(source_root)/%,%,$(src)))

objects := $(patsubst %.c,%.o,$(addprefix $(object_root)/,$(sources_no_root)))
deps := $(objects:.o=.d)
object_dir_structure := $(sort $(dir $(objects))) 

target_name := a.out
target := $(build_root)/$(target_name)
build_cmd := gcc $(flags) $(objects) -o $(target)

all: $(target)

test.o: test.c
	
-include $(deps)

$(target): $(objects) | $(build_root)
	$(build_cmd)

$(object_root)/%.o: $(source_root)/%.c | $(object_dir_structure)
	gcc $(flags) -MMD -MP -c  $< -o $@

$(object_dir_structure):
	mkdir -p $@

$(build_root):
	mkdir -p $@

init:
	mkdir -p src/
	mkdir -p include/

run: $(target)
	@$(CURDIR)/$(target)

clean:
	rm -rf $(object_root) $(build_root)

.PHONY: all run clean init
