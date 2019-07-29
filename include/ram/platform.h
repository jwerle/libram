#ifndef RAM_PLATFORM_H
#define RAM_PLATFORM_H

#if defined(_WIN32)
#  define RAM_EXPORT __declspec(dllimport)
#elif defined(__GNUC__) && (__GNUC__ * 100 + __GNUC_MINOR) >= 303
#  define RAM_EXPORT __attribute__((visibility("default")))
#  define RAM_INLINE inline
#else
#  define RAM_EXPORT
#  define RAM_INLINE
#endif

#ifndef RAM_ALIGNMENT
#  define RAM_ALIGNMENT sizeof(unsigned long) // platform word
#endif

#ifndef RAM_MAX_ENUM
#  define RAM_MAX_ENUM 0x7FFFFFFF
#endif

#endif
