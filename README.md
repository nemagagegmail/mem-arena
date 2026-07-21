# mem-arena
**Fast, zero-dependency, header-only memory utilities for C/C++.**

Clean alignment helpers + high-performance linear arena allocator.

### Features

- Blazing fast bitwise alignment functions
- Simple and cache-friendly linear arena allocator
- No allocations, no fragmentation, predictable performance
- Single header files, zero dependencies

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
| `mem_arena_reset()` | Resets the arena to the beginning. |
| `mem_arena_remaining()` | Returns remaining free space. |
| `mem_arena_pop()` | Moves the allocation offset backwards. |

## Scope (`mem_scope.h`)

| Function / Type | Description |
|-----------------|-------------|
| `mem_scope_t` | Structure storing the arena pointer and saved offset. |
| `mem_scope_begin()` | Enters a temporary memory scope and saves the current offset. |
| `mem_scope_end()` | Exits the scope and automatically restores the arena offset. |
> **Note**
>
> `alignment` must always be a power of two.
