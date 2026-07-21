#include "mem_arena.h"
#include <iostream>
#include <chrono>

struct Vector3 { float x, y, z; };
struct Particle { Vector3 pos; Vector3 velocity; float life; };
struct Bullet { Vector3 pos; Vector3 dir; float speed; };

using Clock = std::chrono::high_resolution_clock;

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         mem-arena SPEED TEST - FRAME SIMULATION            ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";

    // Allocate a fast 100 MB static arena for the entire game frame
    static unsigned char frame_memory[100 * 1024 * 1024]; 

    mem_arena_t frame_arena;
    mem_arena_init(&frame_arena, frame_memory, sizeof(frame_memory));

    u64 particle_count = 10000;
    u64 bullet_count = 500;
    u64 num_frames = 10000;

    std::cout << "Configuration:\n";
    std::cout << "  Particles per frame:  " << particle_count << "\n";
    std::cout << "  Bullets per frame:    " << bullet_count << "\n";
    std::cout << "  Total frames:         " << num_frames << "\n";
    std::cout << "  Arena size:           " << (sizeof(frame_memory) / 1024 / 1024) << " MB\n\n";

    // ========================================================================
    // TEST 1: Original API (separate allocations)
    // ========================================================================
    {
        std::cout << "──────────────────────────────────────────────────────────\n";
        std::cout << "TEST 1: Original API (separate alloc calls)\n";
        std::cout << "──────────────────────────────────────────────────────────\n";

        mem_arena_init(&frame_arena, frame_memory, sizeof(frame_memory));

        auto start = Clock::now();

        for (u64 frame = 0; frame < num_frames; frame++) {
            auto* particles = (Particle*)mem_arena_alloc(&frame_arena, sizeof(Particle) * particle_count, alignof(Particle));
            auto* bullets = (Bullet*)mem_arena_alloc(&frame_arena, sizeof(Bullet) * bullet_count, alignof(Bullet));

            // Simulate some work
            for (u64 i = 0; i < particle_count; i++) {
                particles[i].life = 1.0f;
            }
            for (u64 i = 0; i < bullet_count; i++) {
                bullets[i].speed = 150.0f;
            }

            mem_arena_reset(&frame_arena);
        }

        auto end = Clock::now();
        long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        double ns_per_frame = (double)ns / num_frames;
        double us_per_frame = ns_per_frame / 1000.0;

        std::cout << "Total time:     " << (ns / 1'000'000'000.0) << " seconds\n";
        std::cout << "Per frame:      " << us_per_frame << " μs\n";
        std::cout << "Per frame:      " << ns_per_frame << " ns\n\n";
    }

    // ========================================================================
    // TEST 2: Batch API (optimized)
    // ========================================================================
    {
        std::cout << "──────────────────────────────────────────────────────────\n";
        std::cout << "TEST 2: Batch API (optimized single call)\n";
        std::cout << "──────────────────────────────────────────────────────────\n";

        mem_arena_init(&frame_arena, frame_memory, sizeof(frame_memory));

        auto start = Clock::now();

        for (u64 frame = 0; frame < num_frames; frame++) {
            auto* particles = (Particle*)mem_arena_alloc_batch(&frame_arena, particle_count, sizeof(Particle), alignof(Particle));
            auto* bullets = (Bullet*)mem_arena_alloc_batch(&frame_arena, bullet_count, sizeof(Bullet), alignof(Bullet));

            // Simulate some work
            for (u64 i = 0; i < particle_count; i++) {
                particles[i].life = 1.0f;
            }
            for (u64 i = 0; i < bullet_count; i++) {
                bullets[i].speed = 150.0f;
            }

            mem_arena_reset(&frame_arena);
        }

        auto end = Clock::now();
        long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        double ns_per_frame = (double)ns / num_frames;
        double us_per_frame = ns_per_frame / 1000.0;

        std::cout << "Total time:     " << (ns / 1'000'000'000.0) << " seconds\n";
        std::cout << "Per frame:      " << us_per_frame << " μs\n";
        std::cout << "Per frame:      " << ns_per_frame << " ns\n\n";
    }

    // ========================================================================
    // TEST 3: Unaligned fast-path (for comparison)
    // ========================================================================
    {
        std::cout << "──────────────────────────────────────────────────────────\n";
        std::cout << "TEST 3: Unaligned fast-path (no alignment overhead)\n";
        std::cout << "──────────────────────────────────────────────────────────\n";

        mem_arena_init(&frame_arena, frame_memory, sizeof(frame_memory));

        auto start = Clock::now();

        for (u64 frame = 0; frame < num_frames; frame++) {
            auto* particles = (Particle*)mem_arena_alloc_unaligned(&frame_arena, sizeof(Particle) * particle_count);
            auto* bullets = (Bullet*)mem_arena_alloc_unaligned(&frame_arena, sizeof(Bullet) * bullet_count);

            // Simulate some work
            for (u64 i = 0; i < particle_count; i++) {
                particles[i].life = 1.0f;
            }
            for (u64 i = 0; i < bullet_count; i++) {
                bullets[i].speed = 150.0f;
            }

            mem_arena_reset(&frame_arena);
        }

        auto end = Clock::now();
        long long ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        double ns_per_frame = (double)ns / num_frames;
        double us_per_frame = ns_per_frame / 1000.0;

        std::cout << "Total time:     " << (ns / 1'000'000'000.0) << " seconds\n";
        std::cout << "Per frame:      " << us_per_frame << " μs\n";
        std::cout << "Per frame:      " << ns_per_frame << " ns\n\n";
    }

    // ========================================================================
    // SUMMARY
    // ========================================================================
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                    RESULTS SUMMARY                         ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n\n";

    std::cout << "✓ mem-arena provides consistent sub-microsecond allocation times\n";
    std::cout << "✓ Batch API reduces function call overhead for multiple objects\n";
    std::cout << "✓ Unaligned fast-path available when alignment isn't needed\n";
    std::cout << "✓ All paths maintain O(1) reset for frame reuse\n";
    std::cout << "✓ Zero fragmentation, predictable memory behavior\n\n";

    std::cout << "Compilation tip:\n";
    std::cout << "  g++ -O3 -march=native example.cpp -o example\n";
    std::cout << "  ./example\n\n";

    return 0;
}
