#ifndef SO_INDEX_OF_H
#define SO_INDEX_OF_H

#include <type_traits>

namespace so {
    // index_of : returns the first index (0-based) of T in Ts.
    // Returns sizeof...(Ts) if not found
    template<size_t I, typename T, typename... Ts> struct index_of_impl;

    template<size_t I, typename T, typename U, typename... Ts>
    struct index_of_impl<I, T, U, Ts...> : index_of_impl<I+1, T, Ts...> {};

    template<size_t I, typename T, typename... Ts>
    struct index_of_impl<I, T, T, Ts...> : std::integral_constant<size_t, I> {};

    template<size_t I, typename T>
    struct index_of_impl<I, T> : std::integral_constant<size_t, I> {};

    template<typename T, typename... Ts> struct index_of : index_of_impl<0, T, Ts...> {};

    template<class, class> struct index_of_in;

    template<class T, class... Ts>
    struct index_of_in<T, type_sequence<Ts...>> : index_of<T, Ts...> {};

    template <class T, class... Ts>
    using not_in = bool_constant<index_of<T, Ts...>::value == sizeof...(Ts)>;

    // index_of : returns the first index (0-based) of T in Ts.
    // Returns sizeof...(Ts) if not found
    template<size_t I, typename F, typename... Ts> struct index_of_fun_impl {
        static constexpr size_t value = I;
    };

    template<size_t I, typename F, typename T, typename... Ts>
    struct index_of_fun_impl<I, F, T, Ts...> {
        static constexpr size_t value = F::template apply<T>::value? I : index_of_fun_impl<I+1, F, Ts...>::value;
    };

    template<typename F, typename... Ts> struct index_of_fun : index_of_fun_impl<0, F, Ts...> {};
}


#endif //SO_INDEX_OF_H
