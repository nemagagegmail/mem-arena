# mem-arena
**Fast, zero-dependency, header-only memory utilities for C/C++.**

Clean alignment helpers + high-performance linear arena allocator.

### Features

- Blazing fast bitwise alignment functions
- Simple and cache-friendly linear arena allocator
- No allocations, no fragmentation, predictable performance
- Single header files, zero dependencies
- **NEW: Optimized fast-paths for common cases**
- **NEW: Batch allocation for better throughput**
- **NEW: Debug utilities for profiling and diagnostics**

# API

## Alignment (`mem_align.h`)

| Function | Description |
|----------|-------------|
| `align_forward()` | Aligns a value up to the specified alignment. |
| `align_backward()` | Aligns a value down. |
| `is_power_of_two()` | Checks whether a value is a power of two. |
| `is_aligned()` | Checks whether a value is aligned. |
| `align_next_pow2()` | Returns the next power of two. |
| `align_padding()` | Returns required alignment padding. |
| `align_ptr_forward()` | Aligns a pointer forward. |

## Arena (`mem_arena.h`)

| Function | Description |
|----------|-------------|
| `mem_arena_init()` | Initializes an arena using an existing buffer. |
| `mem_arena_alloc()` | Allocates aligned memory from the arena. |
| `mem_arena_alloc_unaligned()` | **NEW:** Fast-path for unaligned allocations (no alignment computation). |
| `mem_arena_alloc_batch()` | **NEW:** Batch-allocate N identical objects with single alignment check. |
| `mem_arena_reset()` | Resets the arena to the beginning. |
| `mem_arena_remaining()` | Returns remaining free space. |
| `mem_arena_pop()` | Moves the allocation offset backwards. |

## Scope (`mem_scope.h`)

| Function / Type | Description |
|-----------------|-------------|
| `mem_scope_t` | Structure storing the arena pointer and saved offset. |
| `mem_scope_begin()` | Enters a temporary memory scope and saves the current offset. |
| `mem_scope_end()` | Exits the scope and automatically restores the arena offset. |

## Debug (`mem_debug.h`) - NEW

| Function / Type | Description |
|-----------------|-------------|
| `mem_arena_debug_t` | Tracks allocation statistics and fragmentation. |
| `mem_arena_debug_init()` | Initialize debug tracking. |
| `mem_arena_debug_track_alloc()` | Record allocation for statistics. |
| `mem_arena_debug_reset()` | Reset tracking state. |
| `mem_arena_debug_print()` | Print full statistics report. |
| `mem_arena_debug_status()` | Print brief arena status. |

> **Note**
>
> `alignment` must always be a power of two.

---

## Quick Start

### Basic Usage
```c
#include "mem_arena.h"

int main() {
    unsigned char buffer[1024 * 1024];  // 1 MB arena
    mem_arena_t arena;
    mem_arena_init(&arena, buffer, sizeof(buffer));
    
    // Allocate 256 bytes, 64-byte aligned
    char *data = (char*)mem_arena_alloc(&arena, 256, 64);
    
    // Use the allocation...
    
    // Reset for next frame/cycle
    mem_arena_reset(&arena);
    
    return 0;
}
```

### Batch Allocation (Optimized)
```c
#include "mem_arena.h"

struct Particle {
    float x, y, z;
    float vx, vy, vz;
};

int main() {
    unsigned char buffer[100 * 1024 * 1024];
    mem_arena_t arena;
    mem_arena_init(&arena, buffer, sizeof(buffer));
    
    // Allocate 10,000 particles in one optimized call
    Particle *particles = (Particle*)mem_arena_alloc_batch(
        &arena, 10000, sizeof(Particle), alignof(Particle)
    );
    
    // Much faster than 10,000 separate mem_arena_alloc() calls
    
    return 0;
}
```

### Fast-Path (Unaligned)
```c
#include "mem_arena.h"

int main() {
    unsigned char buffer[1024 * 1024];
    mem_arena_t arena;
    mem_arena_init(&arena, buffer, sizeof(buffer));
    
    // For raw byte buffers or pre-aligned data (no alignment check)
    char *bytes = (char*)mem_arena_alloc_unaligned(&arena, 1024);
    
    return 0;
}
```

### Debug & Profiling
```c
#include "mem_arena.h"
#include "mem_debug.h"

int main() {
    unsigned char buffer[100 * 1024 * 1024];
    mem_arena_t arena;
    mem_arena_init(&arena, buffer, sizeof(buffer));
    
    mem_arena_debug_t debug;
    mem_arena_debug_init(&debug, &arena);
    
    // ... perform allocations ...
    mem_arena_alloc(&arena, 256, 64);
    mem_arena_debug_track_alloc(&debug, 256, 64);
    
    // ... more allocations ...
    
    mem_arena_debug_print(&debug);  // Print statistics
    
    return 0;
}
```

---

## Performance

### Benchmarks
Run the included benchmarks to see performance on your system:

```bash
# Standard comprehensive benchmark
g++ -O3 -march=native benchmark.cpp -o benchmark
./benchmark

# Optimized benchmark comparing new paths
g++ -O3 -march=native optimized_benchmark.cpp -o opt_benchmark
./opt_benchmark
```

### Expected Performance (typical x86-64, -O3)

| Operation | Time | Notes |
|-----------|------|-------|
| `mem_arena_alloc()` (64B, 8-byte aligned) | ~2-3 ns | Small overhead over pointer bump |
| `mem_arena_alloc_unaligned()` (64B) | ~1-2 ns | Fast-path, no alignment check |
| `mem_arena_alloc_batch()` (1000 x 64B) | ~0.002 ns/item | Amortized across batch |
| `mem_arena_reset()` | ~1 ns | O(1) assignment |
| vs `malloc()` | ~100x faster | For typical workloads |

---

## Optimization Techniques Used

1. **Bitwise alignment** - O(1) alignment computation via `(value + align - 1) & -align`
2. **Linear bump allocation** - Simple offset increment, cache-friendly
3. **Fast-path for common cases** - `mem_arena_alloc_unaligned()` for zero-overhead path
4. **Batch allocation** - Single alignment + bounds check for multiple objects
5. **Branch prediction hints** - `MEM_LIKELY()` / `MEM_UNLIKELY()` macros
6. **Force-inline** - Compiler always inlines header functions
7. **Zero-copy reset** - Single assignment restores entire arena

---

## Use Cases

- **Game engines** - Per-frame memory allocation with predictable reset
- **Real-time systems** - Bounded allocation times, no GC pauses
- **Data processing pipelines** - Temporary allocations in hot loops
- **Embedded systems** - Minimal overhead, no external dependencies
- **Rendering** - Frame-local vertex/triangle buffers
- **Physics engines** - Simulation-frame scratch memory

---

## License

MIT License - See LICENSE file
