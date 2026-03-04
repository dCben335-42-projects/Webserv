#ifndef KIT_IS_TPP
#define KIT_IS_TPP

#include <string>
#include <vector>
#include <map>

// clang-format off

template<typename A, typename B>
struct is_same {
    static const bool value = false;
};

template<typename A>
struct is_same<A, A> {
    static const bool value = true;
};



template<typename V>
struct is_string {
    static const bool value = is_same<V, std::string>::value || is_same<V, const std::string>::value;
};


template<typename V>
struct is_char {
    static const bool value = is_same<V, char>::value || is_same<V, const char>::value;
};

template<typename V>
struct is_char_ptr {
    static const bool value = is_same<V, char *>::value || is_same<V, const char *>::value;
};

template<typename V>
struct is_unsigned_char {
    static const bool value = is_same<V, unsigned char>::value;
};



template<typename V>
struct is_int {
    static const bool value = is_same<V, int>::value;
};

template<typename V>
struct is_float {
    static const bool value = is_same<V, float>::value;
};

template<typename V>
struct is_double {
    static const bool value = is_same<V, double>::value;
};

template<typename V>
struct is_long {
    static const bool value = is_same<V, long>::value;
};


template<typename V>
struct is_short {
    static const bool value = is_same<V, short>::value;
};

template<typename V>
struct is_unsigned_int {
    static const bool value = is_same<V, unsigned int>::value;
};

template<typename V>
struct is_unsigned_long {
    static const bool value = is_same<V, unsigned long>::value;
};

template<typename V>
struct is_unsigned_short {
    static const bool value = is_same<V, unsigned short>::value;
};

template<typename V>
struct is_bool {
    static const bool value = is_same<V, bool>::value;
};

template<typename V>
struct is_number {
    static const bool value =
        is_int<V>::value ||
        is_float<V>::value ||
        is_double<V>::value ||
        is_long<V>::value ||
        is_short<V>::value ||
        is_unsigned_int<V>::value ||
        is_unsigned_long<V>::value ||
        is_unsigned_short<V>::value ||
        is_bool<V>::value;
};



template<typename V>
struct is_vector {
    static const bool value = false;
};

template<typename V>
struct is_vector<std::vector<V> > {
    static const bool value = true;
};


template<typename V>
struct is_map {
    static const bool value = false;
};

template<typename K, typename V>
struct is_map<std::map<K, V> > {
    static const bool value = true;
};


template<typename V>
struct is_container {
    static const bool value =
        is_vector<V>::value ||
        is_map<V>::value;
};


template <typename T>
struct is_pointer
{
    enum { value = 0 };
};

template <typename T>
struct is_pointer<T*>
{
    enum { value = 1 };
};


#endif
