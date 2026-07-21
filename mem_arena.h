#ifndef MEM_ARENA_H
#define MEM_ARENA_H

#include "mem_align.h"
#include "mem_defs.h"

typedef struct {
    unsigned char *buffer;   // pointer to the memory block
    u64 capacity;            // total size of the buffer
    u64 offset;              // current allocation offset
} mem_arena_t;

/* 
 * Initializes the arena with a pre-allocated buffer.
 * This buffer can be on the stack, static, or from malloc.
 */
static inline void mem_arena_init(mem_arena_t *arena, void *buffer, u64 capacity) {
    arena->buffer   = (unsigned char *)buffer;
    arena->capacity = capacity;
    arena->offset   = 0;
}

/* 
 * Allocates 'size' bytes from the arena with specified alignment.
 * Returns NULL if there's not enough remaining space.
 */
static inline void* mem_arena_alloc(mem_arena_t *arena, u64 size, u64 alignment) {
    if (size == 0) return NULL;

    // Step 1: Align current offset
    u64 aligned_offset = align_forward(arena->offset, alignment);

    // Step 2: Check if we have enough space left
    if (aligned_offset + size > arena->capacity) {
        return NULL;  // arena is full
    }

    // Step 3: Advance the offset and return the pointer
    arena->offset = aligned_offset + size;
    return arena->buffer + aligned_offset;
}

/* 
 * Resets the arena, allowing it to be reused from the beginning.
 * Does NOT zero the memory.
 */
static inline void mem_arena_reset(mem_arena_t *arena) {
    arena->offset = 0;
}

/* 
 * Returns how many bytes are still available in the arena.
 */
static inline u64 mem_arena_remaining(const mem_arena_t *arena) {
    return arena->capacity - arena->offset;
}

/* 
 * "Pops" the last 'size' bytes from the arena (reverse allocation).
 */
static inline void mem_arena_pop(mem_arena_t *arena, u64 size) {
    if (size >= arena->offset) {
        arena->offset = 0;
    } else {
        arena->offset -= size;
    }
}

#endif /* MEM_ARENA_H */
