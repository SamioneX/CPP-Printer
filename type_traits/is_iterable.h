#ifndef SO_IS_ITERABLE_H
#define SO_IS_ITERABLE_H

#include "has_op.h"
#include "logical.h"

namespace so {
    template <class T>
    struct is_input_iterator : conjunction<has_pre_increment<T>, has_deref<T>> {};

    template <typename T, typename = void>
    struct is_iterable : std::false_type {};

    template <typename T>
    struct is_iterable<T, void_t<decltype(std::begin(std::declval<T&>())), decltype(std::end(std::declval<T&>()))>>
            :conjunction<
                    is_input_iterator<decltype(std::begin(std::declval<T&>()))>,
                    has_not_equal_to<decltype(std::begin(std::declval<T&>())), decltype(std::end(std::declval<T&>())), bool>
            > {};
}


#endif //SO_IS_ITERABLE_H
