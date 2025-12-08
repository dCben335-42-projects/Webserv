#ifndef IS_TYPE_HPP
#define IS_TYPE_HPP

// ANCHOR: GENERIC is_same (used by other type traits)
template<typename A, typename B>
struct is_same;

template<typename A>
struct is_same<A, A>;

// ANCHOR: String and Char type traits
template<typename V>
struct is_string;

template<typename V>
struct is_char;

template<typename V>
struct is_unsigned_char;

// ANCHOR: Numeric type traits
template<typename V>
struct is_int;

template<typename V>
struct is_float;

template<typename V>
struct is_double;

template<typename V>
struct is_long;

template<typename V>
struct is_long_long;

template<typename V>
struct is_short;

template<typename V>
struct is_unsigned_int;

template<typename V>
struct is_unsigned_long;

template<typename V>
struct is_unsigned_long_long;

template<typename V>
struct is_unsigned_short;

template<typename V>
struct is_number;


// ANCHOR: Container type traits
template<typename V>
struct is_vector;

template<typename V>
struct is_map;

template<typename V>
struct is_container;


# include "../srcs/is.tpp"

#endif