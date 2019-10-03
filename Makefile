# This Makefile was written for use on Linux. This project
# depends on MPI library in C and optionally on the Boost
# library for C++, but this Makefile e does not check that
# these prerequisites are met.

# Heavily inspired on: https://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/ 

CC := mpic++
# CC := clang --analyze
SOURCE_DIR := src
BUILD_DIR := bin
TARGET := bin/clustering bin/kless_clustering
 
SOURCE_EXT := cc
SOURCES := $(shell find $(SOURCE_DIR) -type f -name *.$(SOURCE_EXT))
OBJECTS := $(patsubst $(SOURCE_DIR)/%,$(BUILD_DIR)/%,$(SOURCES:.$(SOURCE_EXT)=.o))
CCFLAGS := -g -Wall -Wextra -std=c++11
LIB := -L lib 
INC := -I include

all: $(TARGET)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.$(SOURCE_EXT)
	mkdir -p $(@D)
	$(CC) $(CCFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILD_DIR) $(TARGET)

# Tests
distributed_test: test/distributed_test.cc $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o bin/distributed_test $(INC) $(LIB)

sequential_test: test/sequential_test.cc $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o bin/sequential_test $(INC) $(LIB)

sequential_runtime_test: test/sequential_runtime_test.cc $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o bin/sequential_runtime_test $(INC) $(LIB)

distributed_runtime_test: test/distributed_runtime_test.cc $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o bin/distributed_runtime_test $(INC) $(LIB)

bin/clustering: test/k.cc $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o bin/clustering $(INC) $(LIB)

bin/kless_clustering: test/kless.cc $(OBJECTS)
	$(CC) $(CCFLAGS) $^ -o bin/kless_clustering $(INC) $(LIB)




.PHONY: all clean