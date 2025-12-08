#ifndef VECTOR_TPP
#define VECTOR_TPP

#include <vector>

template <typename V>
struct Vec {
    typedef std::vector<V> type;
    typedef typename type::iterator iterator;
    typedef typename type::const_iterator const_iterator;
};

#endif