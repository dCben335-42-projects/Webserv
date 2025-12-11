#ifndef VECTOR_TPP
#define VECTOR_TPP

#include <vector>

// clang-format off
template <typename V>
struct Vec : public std::vector<V>
{
    typedef std::vector<V> base;

    Vec() : base(){}
    Vec(const base &b) : base(b) {}
};
// clang-format on

#endif