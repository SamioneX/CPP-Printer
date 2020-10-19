#ifndef SO_IS_IN_H
#define SO_IS_IN_H

#include <type_traits>
#include "util.h"

namespace so {
    // is_in : searches for T in Ts returns true if found else false
    template<typename T, typename... Ts> struct is_in;

    template<typename T, typename... Ts>
    struct is_in<T, T, Ts...> : std::true_type {};

    template<typename T, typename U, typename... Ts>
    struct is_in<T, U, Ts...> : is_in<T, Ts...> {};

    template<typename T>
    struct is_in<T> : std::false_type {};

    template <typename, typename> struct is_in_seq;

    template <typename T, typename... Ts>
    struct is_in_seq<T, type_sequence<Ts...>> : is_in<T, Ts...> {};
}

#endif //SO_IS_IN_H
