#ifndef SO_SMALLEST_UINT_H
#define SO_SMALLEST_UINT_H

#include <type_traits>
#include <limits>

namespace so {
    template<unsigned long long N, typename int_t, typename... int_ts>
    struct smallest_uint_impl : std::conditional<(N <= std::numeric_limits<int_t>::max()),
            std::integral_constant<int_t, N>, smallest_uint_impl<N, int_ts...>> {};

    template<unsigned long long N>
    using smallest_uint = typename smallest_uint_impl<N,
            unsigned char,
            unsigned short,
            unsigned int,
            unsigned long,
            unsigned long long>::type;
}

#endif //SO_SMALLEST_UINT_H
