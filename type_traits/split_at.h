#ifndef SO_SPLIT_AT_H
#define SO_SPLIT_AT_H

#include <type_traits>
#include "util.h"

namespace so {


    template <size_t I, size_t split_index, typename left_seq, typename right_seq, typename... Ts>
    struct split_impl {
        using left = left_seq;
        using right = right_seq;
    };

    template <size_t I, size_t SI, typename... Ls, typename... Rs, typename T, typename... Ts>
    struct split_impl<I, SI, type_sequence<Ls...>, type_sequence<Rs...>, T, Ts...> {
        using Temp = typename std::conditional<(I < SI),
                split_impl<I+1, SI, type_sequence<Ls..., T>, type_sequence<Rs...>, Ts...>,
                split_impl<I+1, SI, type_sequence<Ls...>, type_sequence<Rs..., T>, Ts...>>::type;

        using left = typename Temp::left;
        using right = typename Temp::right;
    };

    template <size_t SI, typename... Ts>
    struct split_at : split_impl<0, SI, type_sequence<>, type_sequence<>, Ts...> {};
}

#endif //SO_SPLIT_AT_H
