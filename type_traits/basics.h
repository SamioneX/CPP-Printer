#ifndef SO_BASICS_H
#define SO_BASICS_H

#include <type_traits>

namespace so {
    template <bool B>
    using bool_constant = std::integral_constant<bool, B>;

    template<typename... Ts> struct make_void { typedef void type;};
    template<typename... Ts> using void_t = typename make_void<Ts...>::type;

    template< class T >
    struct remove_cvref {
        typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
    };
}

#endif //SO_BASICS_H
