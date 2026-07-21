CC = g++
CFLAGS = -O3 -march=native -Wall -Wextra -std=c++11

# Default target
all: example benchmark optimized_benchmark

# Example program with speed tests
example: example.cpp mem_arena.h mem_align.h mem_defs.h mem_scope.h
	$(CC) $(CFLAGS) example.cpp -o example

# Comprehensive benchmark
benchmark: benchmark.cpp mem_arena.h mem_align.h mem_defs.h mem_scope.h
	$(CC) $(CFLAGS) benchmark.cpp -o benchmark

# Optimized benchmark (old vs new paths)
optimized_benchmark: optimized_benchmark.cpp mem_arena.h mem_align.h mem_defs.h mem_scope.h
	$(CC) $(CFLAGS) optimized_benchmark.cpp -o optimized_benchmark

# Run all programs
run: all
	@echo "Running example..."
	./example
	@echo ""
	@echo "Running comprehensive benchmark..."
	./benchmark
	@echo ""
	@echo "Running optimized benchmark..."
	./optimized_benchmark

# Run just the example
run-example: example
	./example

# Run just the benchmarks
run-benchmark: benchmark
	./benchmark

run-opt-benchmark: optimized_benchmark
	./optimized_benchmark

# Clean build artifacts
clean:
	rm -f example benchmark optimized_benchmark

# Help
help:
	@echo "mem-arena Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make all                  - Build all programs"
	@echo "  make example              - Build example program"
	@echo "  make benchmark            - Build comprehensive benchmark"
	@echo "  make optimized_benchmark  - Build optimized benchmark"
	@echo "  make run                  - Build and run all programs"
	@echo "  make run-example          - Run example program"
	@echo "  make run-benchmark        - Run comprehensive benchmark"
	@echo "  make run-opt-benchmark    - Run optimized benchmark"
	@echo "  make clean                - Remove build artifacts"
	@echo "  make help                 - Show this help message"
	@echo ""
	@echo "Environment variables:"
	@echo "  CC                        - C++ compiler (default: g++)"
	@echo "  CFLAGS                    - Compiler flags (default: -O3 -march=native -Wall -Wextra -std=c++11)"
	@echo ""
	@echo "Examples:"
	@echo "  make CC=clang++ run       - Build with clang++ and run all"
	@echo "  make CFLAGS='-O2' all     - Build with -O2 optimization level"

.PHONY: all run run-example run-benchmark run-opt-benchmark clean help
