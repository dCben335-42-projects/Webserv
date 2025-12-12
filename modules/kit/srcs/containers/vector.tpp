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

    // insert(Vec<V> &vec, const V &value)
    // {
    //     vec.push_back(value);
    // }
};
// // clang-format on

// template <typename V>
// Vec<V> toVec(const std::vector<V> &vec)
// {
// 	return Vec<V>(vec);
// }

// template <typename V>
// std::vector<V> toStdVector(const Vec<V> &vec)
// {
// 	return std::vector<V>(vec.begin(), vec.end());
// }


#endif