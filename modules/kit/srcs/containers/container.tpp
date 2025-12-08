#ifndef CONTAINER_TPP
#define CONTAINER_TPP

#include <algorithm>

template <typename C, typename V>
bool isInContainer(const C &c, const V &value)
{
    return std::find(c.begin(), c.end(), value) != c.end();
}

#endif