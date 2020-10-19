#ifndef SO_MACROS_H
#define SO_MACROS_H

#include "util.h"
#include "basics.h"
#include <type_traits>

#define MAKE_BINARY_OP_CHECKER(NAME, OP) \
    namespace so {                                     \
        template <typename LHS, typename RHS, typename Ret = dont_care, typename = void> \
        struct NAME : std::false_type {}; \
        \
        template <typename LHS, typename RHS, typename Ret> \
        struct NAME<LHS, RHS, Ret, void_t<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>> \
                : std::is_same<Ret, typename remove_cvref<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>::type > {}; \
        \
        template <typename LHS, typename RHS> \
        struct NAME<LHS, RHS, dont_care, void_t<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>> \
                : std::true_type {};    \
    }

#define MAKE_UNARY_PRE_OP_CHECKER(NAME, OP) \
    namespace so {                                    \
        template <typename T, typename Ret = dont_care, typename = void> \
        struct NAME : std::false_type {}; \
        \
        template <typename T, typename Ret> \
        struct NAME <T, Ret, void_t<decltype(OP std::declval<T&>())>> \
                : std::is_same<Ret, typename remove_cvref<decltype(OP std::declval<T&>())>::type > {}; \
        \
        template <typename T> \
        struct NAME <T, dont_care, void_t<decltype(OP std::declval<T&>())>> \
                : std::true_type {};        \
    }

#define MAKE_UNARY_POST_OP_CHECKER(NAME, OP) \
    namespace so {                                    \
        template <typename T, typename Ret = dont_care, typename = void> \
        struct NAME : std::false_type {}; \
        \
        template <typename T, typename Ret> \
        struct NAME <T, Ret, void_t<decltype(std::declval<T&>() OP)>> \
                : std::is_same<Ret, typename remove_cvref<decltype(std::declval<T&>() OP)>::type > {}; \
        \
        template <typename T> \
        struct NAME <T, dont_care, void_t<decltype(std::declval<T&>() OP)>> \
                : std::true_type {};        \
    }

#define MAKE_OP_ASSIGN_CHECKER(NAME, OP) \
    namespace so {  \
        template<typename LHS, typename RHS, typename Ret=LHS, typename=void>   \
        struct NAME : std::false_type {};   \
        \
        template<typename LHS, typename RHS, typename Ret>  \
        struct NAME <LHS, RHS, Ret, void_t<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>>   \
                : std::is_same<typename remove_cvref<Ret>::type,                \
                typename remove_cvref<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>::type> {  \
        };  \
    }


#define MAKE_BINARY_OP_CHECKER_WITH_ASSIGN(NAME, OP) \
    MAKE_BINARY_OP_CHECKER(NAME, OP)                 \
    MAKE_OP_ASSIGN_CHECKER(NAME ## _assign, OP ## =)

#define ENABLE_IF(...) typename std::enable_if<__VA_ARGS__>::type* = nullptr
#define ENABLE_IF_V(...) ENABLE_IF(so::conjunction<__VA_ARGS__>::value)

#endif //SO_MACROS_H
