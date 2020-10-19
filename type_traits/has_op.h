#ifndef SO_HAS_OP_H
#define SO_HAS_OP_H

#include "macros.h"

MAKE_BINARY_OP_CHECKER(has_equal_to, ==)
MAKE_BINARY_OP_CHECKER(has_not_equal_to, !=)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_plus, +)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_minus, -)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_times, *)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_divide, /)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_bit_and, &)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_bit_or, |)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_bit_xor, ^)
MAKE_UNARY_PRE_OP_CHECKER(has_bit_not, ~)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_left_shift, <<)
MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(has_right_shift, >>)
MAKE_UNARY_PRE_OP_CHECKER(has_pre_increment, ++)
MAKE_UNARY_PRE_OP_CHECKER(has_pre_decrement, --)
MAKE_UNARY_PRE_OP_CHECKER(has_deref, *)
MAKE_UNARY_POST_OP_CHECKER(has_post_increment, ++)
MAKE_UNARY_POST_OP_CHECKER(has_post_decrement, --)

namespace so {
    template<typename T, typename Ret=dont_care, typename=void>
    struct has_arrow : std::false_type {
    };
    template<typename T, typename Ret>
    struct has_arrow<T, Ret, void_t<decltype(std::declval<T &>().operator->())>>
            : std::is_same<Ret, typename remove_cvref<decltype(std::declval<T &>().operator->())>::type> {
    };
    template<typename T>
    struct has_arrow<T, dont_care, void_t<decltype(std::declval<T &>().operator->())>> : std::true_type {
    };
}

#endif //SO_HAS_OP_H
