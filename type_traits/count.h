#ifndef SO_COUNT_H
#define SO_COUNT_H

#include <type_traits>
#include "basics.h"
#include "util.h"

namespace so {
    template<typename T, typename... Ts> struct count : std::integral_constant<size_t, 0> {};

    template<typename T, typename U, typename... Rest>
    struct count<T, U, Rest...>
    : std::integral_constant<size_t, count<T, Rest...>::value + std::is_same<T, U>::value> {};

    template <class, class> struct count_in;

    template <class T, typename... Ts>
    struct count_in<T, type_sequence<Ts...>> : count<T, Ts...>{};

    template<typename T, typename... Ts>
    using exactly_once = bool_constant<count<T, Ts...>::value == 1>;

    template <class T, class Seq>
    using exactly_once_in = bool_constant<count_in<T, Seq>::value == 1>;

    template<typename T, typename... Ts>
    using atmost_once = bool_constant<count<T, Ts...>::value <= 1>;

    template <class T, class Seq>
    using atmost_once_in = bool_constant<count_in<T, Seq>::value <= 1>;
}

#endif //SO_COUNT_H
