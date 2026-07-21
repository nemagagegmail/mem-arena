#ifndef MEM_DEBUG_H
#define MEM_DEBUG_H

#include "mem_arena.h"
#include <stdio.h>

typedef struct {
    u64 total_allocations;
    u64 total_bytes_requested;
    u64 total_bytes_allocated;
    u64 total_padding_wasted;
    u64 peak_usage;
} mem_arena_stats_t;

typedef struct {
    mem_arena_t *arena;
    mem_arena_stats_t stats;
    u64 last_offset;
} mem_arena_debug_t;

/* 
 * Initialize debug tracking for an arena.
 */
static inline void mem_arena_debug_init(mem_arena_debug_t *debug, mem_arena_t *arena) {
    debug->arena = arena;
    debug->stats.total_allocations = 0;
    debug->stats.total_bytes_requested = 0;
    debug->stats.total_bytes_allocated = 0;
    debug->stats.total_padding_wasted = 0;
    debug->stats.peak_usage = 0;
    debug->last_offset = 0;
}

/* 
 * Track allocation with statistics (call this after mem_arena_alloc).
 */
static inline void mem_arena_debug_track_alloc(mem_arena_debug_t *debug, u64 requested_size, u64 alignment) {
    u64 current_offset = debug->arena->offset;
    u64 padding = current_offset - debug->last_offset - requested_size;
    
    debug->stats.total_allocations++;
    debug->stats.total_bytes_requested += requested_size;
    debug->stats.total_bytes_allocated += (current_offset - debug->last_offset);
    debug->stats.total_padding_wasted += padding;
    
    if (current_offset > debug->stats.peak_usage) {
        debug->stats.peak_usage = current_offset;
    }
    
    debug->last_offset = current_offset;
}

/* 
 * Reset debug stats (usually called with arena reset).
 */
static inline void mem_arena_debug_reset(mem_arena_debug_t *debug) {
    debug->last_offset = 0;
    debug->arena->offset = 0;
}

/* 
 * Print arena statistics.
 */
static inline void mem_arena_debug_print(mem_arena_debug_t *debug) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║           ARENA DEBUG STATISTICS                           ║\n");
    printf("╚════════════════════════════════════════════════════════════╝\n\n");
    
    printf("Total Allocations:      %llu\n", debug->stats.total_allocations);
    printf("Total Requested:        %llu bytes\n", debug->stats.total_bytes_requested);
    printf("Total Allocated:        %llu bytes\n", debug->stats.total_bytes_allocated);
    printf("Total Padding Wasted:   %llu bytes\n", debug->stats.total_padding_wasted);
    printf("Peak Usage:             %llu bytes (%.2f MB)\n", 
           debug->stats.peak_usage, 
           (double)debug->stats.peak_usage / (1024 * 1024));
    printf("Fragmentation Ratio:    %.2f%%\n", 
           debug->stats.total_bytes_allocated > 0 
           ? ((double)debug->stats.total_padding_wasted / debug->stats.total_bytes_allocated) * 100 
           : 0.0);
    printf("Efficiency:             %.2f%%\n",
           debug->stats.total_bytes_allocated > 0 
           ? ((double)debug->stats.total_bytes_requested / debug->stats.total_bytes_allocated) * 100 
           : 0.0);
    printf("\n");
}

/* 
 * Print brief arena status.
 */
static inline void mem_arena_debug_status(const mem_arena_t *arena) {
    u64 used = arena->offset;
    u64 remaining = mem_arena_remaining(arena);
    u64 total = arena->capacity;
    
    printf("[Arena] Used: %llu / %llu bytes (%.1f%%), Remaining: %llu bytes\n",
           used, total, (double)used / total * 100, remaining);
}

#endif /* MEM_DEBUG_H */
