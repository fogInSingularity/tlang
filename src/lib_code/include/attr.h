#ifndef ATTR_H_
#define ATTR_H_

#ifndef DEBUG
#define OPT_ON
#endif // DEBUG

#if defined (__clang__) || defined (__GNUC__)
  #if __STDC_VERSION__ > 201710L
    #define UNUSED [[maybe_unused]]
  #else
    #define UNUSED
  #endif // __STDC_VERSION__
  #define ALIGN(n) __attribute__((aligned(n)))
#else
  #define UNUSED
  #define ALIGN(n)
#endif

#if defined (OPT_ON)
  // check for c2x clang
  #if defined (__clang__)
    #if __STDC_VERSION__ > 201710L
      #define LIKELY [[clang::likely]]
      #define UNLIKELY [[clang::unlikely]]
    #else
      #define LIKELY
      #define UNLIKELY
    #endif
    #define HOT __attribute__((hot))
    #define INLINE __attribute__((always_inline))
    #define NOINLINE __attribute__((noinline))
  #elif defined (__GNUC__)
    #define LIKELY
    #define UNLIKELY
    #define HOT __attribute__((hot))
    #define INLINE __attribute__((always_inline))
    #define NOINLINE __attribute__((noinline))
  #else
    #define LIKELY
    #define UNLIKELY
    #define HOT
    #define INLINE
    #define NOINLINE
  #endif
#else
  #define LIKELY
  #define UNLIKELY
  #define HOT
  #define INLINE
  #define NOINLINE
#endif // OPT_ON

#endif // ATTR_H_
