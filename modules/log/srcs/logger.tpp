#ifndef UTILS_TPP
#define UTILS_TPP

#include "./Log/Log.hpp"
# include <vector> 
# include <map>
# include <iostream>
# include <IsType.hpp>

template<typename V>
void log(const V &v)
{
    std::cout << v;
}

template<typename V>
void log(V *ptr)
{
    if (ptr)
        log(*ptr);
    else
        std::cout << Log::indent() << "null";
}

template <typename V>
void log(const std::vector<V> &vec)
{
    std::cout << Log::indent() << "[\n";
    Log::incrementIndent();
    for (size_t i = 0; i < vec.size(); ++i)
    {
        log(vec[i]);
        if (i + 1 < vec.size())
            std::cout << ",\n";
    }
    Log::decrementIndent();
    std::cout << "\n" << Log::indent() << "]";
}

template<typename M, typename V>
void log(const std::map<M,V>& m)
{
    std::cout << Log::indent() << "{\n";
    Log::incrementIndent();
    typename std::map<M,V>::const_iterator it = m.begin();
    typename std::map<M,V>::const_iterator end = m.end();
    
    while(it != end)
    {
        std::cout << Log::indent();
        log(it->first);
        std::cout << ": ";
        if (!is_string<V>::value)
        {
            Log::incrementIndent();
            std::cout << "\n";
            log(it->second);
            Log::decrementIndent();
        }
        else 
            log(it->second);
        if (++it != end) std::cout << ",\n";
    }
    Log::decrementIndent();
    std::cout << "\n" << Log::indent() << "}";
}

#endif
