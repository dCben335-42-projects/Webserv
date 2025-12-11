#ifndef VECTOR_TPP
#define VECTOR_TPP

#include <vector>

// template <typename V>
// struct VecImpl {
//     typedef std::vector<V> vector_type;
// };

// template <typename V>
// struct Vec : public VecImpl<V>::vector_type
// {
//     typedef typename VecImpl<V>::vector_type base;
//     Vec() : base() {}
//     Vec(const base& b) : base(b) {}
// };

template <typename V>
struct Vec : public std::vector<V>
{
    typedef std::vector<V> base;

    Vec() : base() {}
    Vec(const base &b) : base(b) {}
};


#endif