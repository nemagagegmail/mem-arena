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
MEM_FORCE_INLINE void mem_arena_init(mem_arena_t *arena, void *buffer, u64 capacity) {
    arena->buffer   = (unsigned char *)buffer;
    arena->capacity = capacity;
    arena->offset   = 0;
}

/* 
 * Allocates 'size' bytes from the arena with specified alignment.
 * Returns NULL if there's not enough remaining space.
 * Optimized with branch hints for common case.
 */
MEM_FORCE_INLINE void* mem_arena_alloc(mem_arena_t *arena, u64 size, u64 alignment) {
    if (MEM_UNLIKELY(size == 0)) return NULL;

    // Step 1: Align current offset
    u64 aligned_offset = align_forward(arena->offset, alignment);

    // Step 2: Check if we have enough space left (branch hint: success is common)
    if (MEM_UNLIKELY(aligned_offset + size > arena->capacity)) {
        return NULL;  // arena is full
    }

    // Step 3: Advance the offset and return the pointer
    arena->offset = aligned_offset + size;
    return arena->buffer + aligned_offset;
}

/* 
 * Fast-path allocation for already-aligned data (no alignment check).
 * Use when you know data is already properly aligned or alignment=1.
 * Saves one bitwise operation per allocation.
 */
MEM_FORCE_INLINE void* mem_arena_alloc_unaligned(mem_arena_t *arena, u64 size) {
    if (MEM_UNLIKELY(size == 0)) return NULL;

    // No alignment computation needed
    u64 new_offset = arena->offset + size;
    
    if (MEM_UNLIKELY(new_offset > arena->capacity)) {
        return NULL;
    }

    void *ptr = arena->buffer + arena->offset;
    arena->offset = new_offset;
    return ptr;
}

/* 
 * Batch allocation for N identical objects.
 * More efficient than calling mem_arena_alloc() N times:
 * - Single alignment check
 * - Single bounds check
 * - Better branch prediction
 */
MEM_FORCE_INLINE void* mem_arena_alloc_batch(mem_arena_t *arena, u64 count, u64 item_size, u64 alignment) {
    if (MEM_UNLIKELY(count == 0 || item_size == 0)) return NULL;

    u64 total_size = count * item_size;
    if (MEM_UNLIKELY(total_size < count)) return NULL;  // overflow check

    // Single alignment + bounds check for all items
    u64 aligned_offset = align_forward(arena->offset, alignment);
    
    if (MEM_UNLIKELY(aligned_offset + total_size > arena->capacity)) {
        return NULL;
    }

    arena->offset = aligned_offset + total_size;
    return arena->buffer + aligned_offset;
}

/* 
 * Resets the arena, allowing it to be reused from the beginning.
 * Does NOT zero the memory.
 * O(1) operation: single assignment.
 */
MEM_FORCE_INLINE void mem_arena_reset(mem_arena_t *arena) {
    arena->offset = 0;
}

/* 
 * Returns how many bytes are still available in the arena.
 */
MEM_FORCE_INLINE u64 mem_arena_remaining(const mem_arena_t *arena) {
    return arena->capacity - arena->offset;
}

/* 
 * "Pops" the last 'size' bytes from the arena (reverse allocation).
 */
MEM_FORCE_INLINE void mem_arena_pop(mem_arena_t *arena, u64 size) {
    if (MEM_UNLIKELY(size >= arena->offset)) {
        arena->offset = 0;
    } else {
        arena->offset -= size;
    }
}

/* 
 * Returns the amount of wasted padding due to alignment.
 * Useful for profiling fragmentation in debug builds.
 * This gives you: (actual_used - logical_allocations).
 */
MEM_FORCE_INLINE u64 mem_arena_wasted_padding(mem_arena_t *arena) {
    // Note: This is approximate without tracking individual allocations.
    // For precise tracking, you'd need to log each align_forward() result.
    // This function serves as a placeholder for the pattern.
    return 0;
}

#endif /* MEM_ARENA_H */
