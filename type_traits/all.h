
#ifndef SO_ALL_H
#define SO_ALL_H

#include "is_in.h"
#include "logical.h"
#include "util.h"

namespace so {
    template<typename... Ts> struct all_unique;

    template<typename... Ts>
    using all_unique_t = typename all_unique<Ts...>::type;

    template<> struct all_unique<> : std::true_type {};

    template<typename T> struct all_unique<T> : std::true_type {};

    template<typename T, typename... Ts>
    struct all_unique<T, Ts...> {
        constexpr static const bool value = !is_in<T, Ts...>::value && all_unique<Ts...>::value;
        using type = bool;
    };

    template <class> struct all_unique_in;

    template <class... Ts>
    struct all_unique_in<type_sequence<Ts...>> : all_unique<Ts...> {};

    template<class... >
    struct all_same : std::true_type {};

    template <class T, class... Ts>
    struct all_same<T, Ts...> : conjunction<std::is_same<T, Ts>...> {};

    template <class> struct all_same_in;

    template <class... Ts>
    struct all_same_in<type_sequence<Ts...>> : all_same<Ts...> {};

    template <class F, class... Ts>
    struct all_of : std::true_type {};

    template <class F, class T, class... Ts>
    struct all_of<F, T, Ts...>
    : conjunction<typename F::template apply<T>, all_of<F, Ts...>> {};

    template <class, class> struct all_of_in;

    template <class F, class... Ts>
    struct all_of_in<F, type_sequence<Ts...>> : all_of<F, Ts...> {};

}

#endif //SO_ALL_H
