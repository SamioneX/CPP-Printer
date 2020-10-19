#ifndef SO_NTH_TYPE_H
#define SO_NTH_TYPE_H

#include <type_traits>

namespace so {
    template<size_t N, typename... Ts> struct nth_type;

    template<size_t N, typename T, typename... Rest>
    struct nth_type<N, T, Rest...> : nth_type<N-1, Rest...> {};

    template<typename T, typename... Rest>
    struct nth_type<0, T, Rest...> {
        using type = T;
    };

    template<size_t N, typename... Ts>
    using nth_type_t = typename nth_type<N, Ts...>::type;

    template<typename... Ts>
    using first = nth_type_t<0, Ts...>;

    template<typename... Ts>
    using last = nth_type_t<sizeof...(Ts)-1, Ts...>;
}

#endif //SO_NTH_TYPE_H
