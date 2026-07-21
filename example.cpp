#include "mem_arena.h"
#include <iostream>

struct Vector3 { float x, y, z; };
struct Particle { Vector3 pos; Vector3 velocity; float life; };
struct Bullet { Vector3 pos; Vector3 dir; float speed; };

int main() {
    // Allocate a fast 16 MB static arena for the entire game frame
    static unsigned char frame_memory[16 * 1024 * 1024]; 

    mem_arena_t frame_arena;
    mem_arena_init(&frame_arena, frame_memory, sizeof(frame_memory));

    // Frame simulation: allocate batches of different objects with zero fragmentation and zero malloc overhead
    u64 particle_count = 10000;
    u64 bullet_count = 500;

    auto* particles = static_cast<Particle*>(mem_arena_alloc(&frame_arena, sizeof(Particle) * particle_count, alignof(Particle)));
    auto* bullets = static_cast<Bullet*>(mem_arena_alloc(&frame_arena, sizeof(Bullet) * bullet_count, alignof(Bullet)));

    // Fast data processing
    for (u64 i = 0; i < particle_count; i++) {
        particles[i].life = 1.0f;
    }

    for (u64 i = 0; i < bullet_count; i++) {
        bullets[i].speed = 150.0f;
    }

    std::cout << "[Frame Stats] Used memory: " << (frame_arena.offset / 1024) << " KB\n";
    std::cout << "[Frame Stats] Remaining: " << (mem_arena_remaining(&frame_arena) / 1024) << " KB\n";

    // At the end of the frame, reset the offset to zero in a single O(1) operation
    mem_arena_reset(&frame_arena);
    
    std::cout << "[Frame Stats] Arena reset. Offset is now: " << frame_arena.offset << " bytes\n";
    
    return 0;
}
