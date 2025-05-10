#ifndef HASH_H
#define HASH_H

#include <stdint.h>
#include <stddef.h>

constexpr uint32_t fnv1a_32(const char *s, size_t count)
{
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < count; i++)
    {
        hash ^= static_cast<unsigned char>(s[i]);
        hash *= 16777619u;
    }
    return hash;
}
constexpr uint32_t operator"" _i(const char *str, size_t len)
{
    return fnv1a_32(str, len);
}

#endif