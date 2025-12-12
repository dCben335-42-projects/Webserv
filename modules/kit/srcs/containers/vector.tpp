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

    // non-const version
    template <typename Functor>
    void foreach(Functor f)
    {
        for (typename base::iterator it = this->begin(); it != this->end(); ++it)
            f(*it);
    }

    // const version
    template <typename Functor>
    void foreach(Functor f) const
    {
        for (typename base::const_iterator it = this->begin(); it != this->end(); ++it)
            f(*it);
    }
};
// clang-format on

#endif