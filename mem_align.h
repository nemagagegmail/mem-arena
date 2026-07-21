#ifndef MEM_ALIGN_H
#define MEM_ALIGN_H

#include <stddef.h>

typedef unsigned long long u64;

/* ========================================================================== */
/* Core Alignment Operations                                                  */
/* ========================================================================== */

/* 
 * Aligns a value UP to the nearest multiple of 'alignment'.
 * Uses the common bitwise trick. Alignment must be a power of two.
 */
static inline u64 align_forward(u64 value, u64 alignment) {
    return (value + alignment - 1) & -alignment;
}

/* 
 * Aligns a value DOWN to the nearest multiple of 'alignment'.
 * Also requires alignment to be a power of two.
 */
static inline u64 align_backward(u64 value, u64 alignment) {
    return value & -alignment;
}

/* 
 * Returns non-zero if 'value' is a power of two (1, 2, 4, 8, 16...).
 * Used to validate alignment values.
 */
static inline int is_power_of_two(u64 value) {
    return value && !(value & (value - 1));
}

/* 
 * Checks if a value is already aligned to the given alignment.
 */
static inline int is_aligned(u64 value, u64 alignment) {
    return (value & (alignment - 1)) == 0;
}

/* ========================================================================== */
/* Additional Utility Functions                                               */
/* ========================================================================== */

/* 
 * Returns the smallest power of two that is greater than or equal to 'value'.
 * Example: 13 -> 16, 17 -> 32, 7 -> 8
 */
static inline u64 align_next_pow2(u64 value) {
    if (value == 0) return 1;
    value--;
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return value + 1;
}

/* 
 * Calculates how many bytes of padding are needed to align 'value'.
 * Returns the padding amount (0 if already aligned).
 */
static inline u64 align_padding(u64 value, u64 alignment) {
    return (-value) & (alignment - 1);
}

/* 
 * Aligns a pointer forward (convenience wrapper).
 */
static inline void* align_ptr_forward(const void* ptr, u64 alignment) {
    return (void*)align_forward((u64)ptr, alignment);
}

/* Convenience macros for cleaner code */
#define ALIGN_FORWARD(value, alignment)   align_forward((value), (alignment))
#define ALIGN_PTR(ptr, alignment)         align_ptr_forward((ptr), (alignment))

#endif /* MEM_ALIGN_H */
