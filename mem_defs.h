#ifndef MEM_DEFS_H
#define MEM_DEFS_H

/* ========================================================================== */
/* COMPILER ATTRIBUTES & MACROS                                               */
/* ========================================================================== */

#if defined(__GNUC__) || defined(__clang__)
    #define MEM_FORCE_INLINE static __attribute__((always_inline)) inline
    #define MEM_RESTRICT __restrict__
    #define MEM_LIKELY(x)   __builtin_expect(!!(x), 1)
    #define MEM_UNLIKELY(x) __builtin_expect(!!(x), 0)
#elif defined(_MSC_VER)
    #define MEM_FORCE_INLINE static __forceinline
    #define MEM_RESTRICT __restrict
    #define MEM_LIKELY(x)   (x)
    #define MEM_UNLIKELY(x) (x)
#else
    #define MEM_FORCE_INLINE static inline
    #define MEM_RESTRICT
    #define MEM_LIKELY(x)   (x)
    #define MEM_UNLIKELY(x) (x)
#endif

#endif /* MEM_DEFS_H */
