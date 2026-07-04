CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=build/%.o)
BIN := mlq_scheduler

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(BIN)

# Ejemplo: make run FILE=data/mlq001.txt
run: all
	./$(BIN) $(FILE) --trace
