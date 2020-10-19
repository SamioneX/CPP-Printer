#ifndef SO_IS_TUPLE_LIKE_H
#define SO_IS_TUPLE_LIKE_H

#include <tuple>
#include "basics.h"

namespace so {
    template <typename T, typename = void>
    struct is_tuple_like : std::false_type {};

    template <typename T>
    struct is_tuple_like<T, void_t<
            decltype(std::get<std::tuple_size<T>::value - 1>(std::declval<T&>()))>>
            : std::true_type{};
}

#endif //SO_IS_TUPLE_LIKE_H
