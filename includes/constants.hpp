#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define KB 1024
#define MB (1024 * KB)
#define GB (1024 * MB)

// Fowler-Noll-Vo magic constants
#define FNV_PRIME_32 16777619u
#define FNV_OFFSET_BASIS_32 2166136261u
// Same but for 64bit encryption
#define FNV_OFFSET_BASIS_64 14695981039346656037ul
#define FNV_PRIME_64 1099511628211ul

// MurmurHash constant (that happens to work well apparently)
#define MURMUR_HASH 1540483477u
#define MURMUR_FIRST_SHIFT 13u
#define MURMUR_LAST_SHIFT 15u

#endif