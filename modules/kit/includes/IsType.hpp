#ifndef KIT_IS_TYPE_HPP
#define KIT_IS_TYPE_HPP

template <typename A, typename B>
struct is_same;

template <typename A>
struct is_same<A, A>;




template <typename V>
struct is_string;

template <typename V>
struct is_char;

template <typename V>
struct is_unsigned_char;



template <typename V>
struct is_int;

template <typename V>
struct is_float;

template <typename V>
struct is_double;

template <typename V>
struct is_long;

template <typename V>
struct is_short;

template <typename V>
struct is_unsigned_int;

template <typename V>
struct is_unsigned_long;

template <typename V>
struct is_unsigned_short;

template <typename V>
struct is_number;



template <typename V>
struct is_vector;

template <typename V>
struct is_map;

template <typename V>
struct is_container;


#include "../srcs/is.tpp"

#endif
