#include "mem_arena.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <iomanip>

using namespace std;
using Clock = chrono::high_resolution_clock;

#define BENCH(name, code, iterations) \
    do { \
        auto start = Clock::now(); \
        for (volatile int _i = 0; _i < (iterations); _i++) { code; } \
        auto end = Clock::now(); \
        long long ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count(); \
        double ns_per_op = (double)ns / (iterations); \
        cout << "  " << setw(40) << left << (name) << ": " \
             << setw(10) << right << ns_per_op << " ns/op\n"; \
    } while(0)

int main() {
    cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    cout << "║      mem-arena OPTIMIZED BENCHMARK (Old vs New Paths)              ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════╝\n\n";

    // ========================================================================
    // TEST 1: Small allocations - Standard path
    // ========================================================================
    {
        cout << "─────────────────────────────────────────────────────────────────\n";
        cout << "TEST 1: Standard mem_arena_alloc() [10M iterations]\n";
        cout << "─────────────────────────────────────────────────────────────────\n";
        
        static unsigned char buffer[200 * 1024 * 1024];
        mem_arena_t arena;
        mem_arena_init(&arena, buffer, sizeof(buffer));
        
        BENCH("64B alloc, 8-byte aligned", {
            mem_arena_alloc(&arena, 64, 8);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 10'000'000);
        
        cout << "\n";
    }

    // ========================================================================
    // TEST 2: Fast-path unaligned allocations
    // ========================================================================
    {
        cout << "─────────────────────────────────────────────────────────────────\n";
        cout << "TEST 2: Fast-path mem_arena_alloc_unaligned() [10M iterations]\n";
        cout << "─────────────────────────────────────────────────────────────────\n";
        
        static unsigned char buffer[200 * 1024 * 1024];
        mem_arena_t arena;
        mem_arena_init(&arena, buffer, sizeof(buffer));
        
        BENCH("64B alloc, NO alignment", {
            mem_arena_alloc_unaligned(&arena, 64);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 10'000'000);
        
        cout << "\n";
    }

    // ========================================================================
    // TEST 3: Batch allocation
    // ========================================================================
    {
        cout << "─────────────────────────────────────────────────────────────────\n";
        cout << "TEST 3: Batch allocation mem_arena_alloc_batch() [100K iterations]\n";
        cout << "─────────────────────────────────────────────────────────────────\n";
        
        static unsigned char buffer[200 * 1024 * 1024];
        mem_arena_t arena;
        mem_arena_init(&arena, buffer, sizeof(buffer));
        
        BENCH("100 x 64B in one call", {
            mem_arena_alloc_batch(&arena, 100, 64, 8);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 100'000);
        
        BENCH("1K x 64B in one call", {
            mem_arena_alloc_batch(&arena, 1000, 64, 8);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 100'000);
        
        cout << "\n";
    }

    // ========================================================================
    // TEST 4: Variable alignment patterns
    // ========================================================================
    {
        cout << "─────────────────────────────────────────────────────────────────\n";
        cout << "TEST 4: Different alignment requirements [1M iterations]\n";
        cout << "─────────────────────────────────────────────────────────────────\n";
        
        static unsigned char buffer[200 * 1024 * 1024];
        mem_arena_t arena;
        mem_arena_init(&arena, buffer, sizeof(buffer));
        
        BENCH("128B alloc, 1-byte aligned", {
            mem_arena_alloc(&arena, 128, 1);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 1'000'000);
        
        BENCH("128B alloc, 8-byte aligned", {
            mem_arena_alloc(&arena, 128, 8);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 1'000'000);
        
        BENCH("128B alloc, 64-byte aligned", {
            mem_arena_alloc(&arena, 128, 64);
            if (arena.offset > 100*1024*1024) mem_arena_reset(&arena);
        }, 1'000'000);
        
        cout << "\n";
    }

    // ========================================================================
    // TEST 5: Reset performance
    // ========================================================================
    {
        cout << "─────────────────────────────────────────────────────────────────\n";
        cout << "TEST 5: Reset performance [10M iterations]\n";
        cout << "─────────────────────────────────────────────────────────────────\n";
        
        static unsigned char buffer[200 * 1024 * 1024];
        mem_arena_t arena;
        mem_arena_init(&arena, buffer, sizeof(buffer));
        
        BENCH("mem_arena_reset()", {
            mem_arena_reset(&arena);
        }, 10'000'000);
        
        cout << "\n";
    }

    // ========================================================================
    // TEST 6: Mixed workload (realistic)
    // ========================================================================
    {
        cout << "─────────────────────────────────────────────────────────────────\n";
        cout << "TEST 6: Realistic mixed workload [100K frame simulation]\n";
        cout << "─────────────────────────────────────────────────────────────────\n";
        
        static unsigned char buffer[200 * 1024 * 1024];
        mem_arena_t arena;
        mem_arena_init(&arena, buffer, sizeof(buffer));
        
        auto start = Clock::now();
        
        for (int frame = 0; frame < 100'000; frame++) {
            // Simulate game frame: allocate particles, bullets, temp data
            mem_arena_alloc(&arena, 10000 * 64, 64);  // 10k particles
            mem_arena_alloc(&arena, 500 * 128, 64);   // 500 bullets
            mem_arena_alloc(&arena, 1024, 8);         // temp buffer
            mem_arena_reset(&arena);
        }
        
        auto end = Clock::now();
        long long ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        double ns_per_frame = (double)ns / 100'000;
        
        cout << "  " << setw(40) << left << "100K frame cycles (alloc+reset)"
             << ": " << setw(10) << right << ns_per_frame << " ns/frame\n";
        cout << "  " << setw(40) << left << "  (micro-ops per frame)"
             << ": " << setw(10) << right << (ns_per_frame / 1000) << " μs/frame\n";
        
        cout << "\n";
    }

    // ========================================================================
    // COMPARATIVE ANALYSIS
    // ========================================================================
    cout << "╔════════════════════════════════════════════════════════════════════╗\n";
    cout << "║                    OPTIMIZATION SUMMARY                             ║\n";
    cout << "╚════════════════════════════════════════════════════════════════════╝\n\n";
    
    cout << "KEY IMPROVEMENTS IMPLEMENTED:\n\n";
    
    cout << "1. mem_arena_alloc_unaligned()\n";
    cout << "   • Skips alignment computation when not needed\n";
    cout << "   • Use for: raw byte buffers, pre-aligned data\n";
    cout << "   • Expected gain: ~0.5-1 ns/op (one bitwise operation saved)\n\n";
    
    cout << "2. mem_arena_alloc_batch()\n";
    cout << "   • Single alignment check for N identical objects\n";
    cout << "   • Replaces N separate function calls\n";
    cout << "   • Use for: game entities, particle systems, pools\n";
    cout << "   • Expected gain: ~50-100x speedup for large batches\n\n";
    
    cout << "3. Branch prediction hints (MEM_LIKELY/UNLIKELY)\n";
    cout << "   • __builtin_expect() guides CPU branch prediction\n";
    cout << "   • Reduces branch mispredicts on capacity checks\n";
    cout << "   • Expected gain: ~1-5% in typical workloads\n\n";
    
    cout << "4. MEM_FORCE_INLINE\n";
    cout << "   • Compiler always inlines header functions\n";
    cout << "   • Reduces function call overhead\n";
    cout << "   • Expected gain: ~1-2 ns/op on small functions\n\n";
    
    cout << "5. Debug utilities (mem_debug.h)\n";
    cout << "   • Track fragmentation patterns\n";
    cout << "   • Profile allocation efficiency\n";
    cout << "   • Helps optimize alignment strategies\n\n";
    
    cout << "COMPILATION RECOMMENDATION:\n";
    cout << "  g++ -O3 -march=native benchmark.cpp -o benchmark\n";
    cout << "  ./benchmark\n\n";
    
    return 0;
}
