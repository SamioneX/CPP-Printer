#ifndef SO_PARTITION_H
#define SO_PARTITION_H

#include "util.h"
#include <type_traits>

namespace so {
    template <typename F, typename left_seq, typename right_seq, typename... Ts>
    struct partition_impl {
        using left = left_seq;
        using right = right_seq;
    };

    template<typename F, typename... Ls, typename... Rs, typename T, typename... Ts>
    struct partition_impl<F, type_sequence<Ls...>, type_sequence<Rs...>, T, Ts...> {
        using Temp = typename std::conditional<F::template apply<T>::value,
                partition_impl<F, type_sequence<Ls..., T>, type_sequence<Rs...>, Ts...>,
                partition_impl<F, type_sequence<Ls...>, type_sequence<Rs..., T>, Ts...>>::type;

        using left = typename Temp::left;
        using right = typename Temp::right;
    };

    template<typename F, typename... Ts>
    struct partition : partition_impl<F, type_sequence<>, type_sequence<>, Ts...> {};
}

#endif //SO_PARTITION_H
