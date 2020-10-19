
#ifndef SO_LOGICAL_H
#define SO_LOGICAL_H

#include <type_traits>
#include "basics.h"

namespace so {
    template<class...> struct conjunction : std::true_type { };
    template<class B1> struct conjunction<B1> : B1 { };
    template<class B1, class... Bn>
    struct conjunction<B1, Bn...>
            : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};

    template<class...> struct disjunction : std::false_type { };
    template<class B1> struct disjunction<B1> : B1 { };
    template<class B1, class... Bn>
    struct disjunction<B1, Bn...>
        : std::conditional<bool(B1::value), B1, disjunction<Bn...>>::type {};

    template<class B>
    struct negation : bool_constant<!bool(B::value)> {};
}

#endif //SO_LOGICAL_H
