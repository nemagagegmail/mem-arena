#ifndef MEM_SCOPE_H
#define MEM_SCOPE_H

#include "mem_arena.h"

typedef struct {
    mem_arena_t *arena;
    u64 saved_offset;
} mem_scope_t;

/* 
 * Enters a temporary memory scope, saving the current arena offset.
 */
static inline mem_scope_t mem_scope_begin(mem_arena_t *arena) {
    mem_scope_t scope;
    scope.arena = arena;
    scope.saved_offset = arena->offset;
    return scope;
}

/* 
 * Exits the scope, automatically restoring the arena offset 
 * and freeing all memory allocated within this scope.
 */
static inline void mem_scope_end(mem_scope_t *scope) {
    scope->arena->offset = scope->saved_offset;
}

#endif /* MEM_SCOPE_H */
