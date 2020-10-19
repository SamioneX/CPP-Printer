#ifndef SO_UTIL_H
#define SO_UTIL_H

#include <tuple>

namespace so {
    struct dont_care {};

    template <typename... Ts>
    struct type_sequence {
        using type = type_sequence<Ts...>;
    };


    // add an index to a current sequence of indices
    template <typename T, class sequence>
    struct cat_type_sequence;

    //specialization
    template <typename T, typename... Ts>
    struct cat_type_sequence<T, type_sequence<Ts...>>
            : type_sequence<Ts..., T> {};
}

namespace std {
    template <class... Ts>
    struct tuple_size<so::type_sequence<Ts...>> : std::integral_constant<size_t, sizeof...(Ts)> {};
}


#endif //SO_UTIL_H
