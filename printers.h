#ifndef SO_PRINTER_H
#define SO_PRINTER_H

#include <vector>
#include <set>
#include <tuple>
#include <type_traits>
#include <array>
#include <iostream>
#if __has_include(<optional>)
#include <optional>
#endif
#if __has_include(<variant>)
#include <variant>
#endif
#if __has_include(<any>)
#include <any>
#include <unordered_map>
#include <typeindex>
#endif


namespace so {
#define print(T) so::write(std::cout, T)
#define println(T) (so::write(std::cout, T), std::cout << '\n')
#define writeln(is, T) (write(is, T), is << '\n')

    namespace details {
#ifdef __cpp_lib_void_t
        using std::void_t;
#else
        template<typename... Ts> struct make_void { typedef void type;};
    template<typename... Ts> using void_t = typename make_void<Ts...>::type;
#endif

#ifdef __cpp_lib_remove_cvref
        using std::remove_cvref;
#else
        template< class T >
struct remove_cvref {
    typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
};
#endif

#ifdef __cpp_lib_logical_traits
        using std::conjunction;
#else
        template<class...> struct conjunction : std::true_type { };
template<class B1> struct conjunction<B1> : B1 { };
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
        : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};
#endif

#define BOOL(v) std::integral_constant<bool, v>

#define CONJ(...) details::conjunction<__VA_ARGS__>::value
#define ENABLE_IF(b) typename std::enable_if<b>::type* = nullptr
#define ENABLE_IF_V(...) ENABLE_IF(CONJ(__VA_ARGS__))

        struct dont_care {};

        template <class T> struct dont_care2 : std::true_type {};

#define MAKE_BINARY_OP_CHECKER(NAME, OP) \
    template <typename LHS, typename RHS, typename Ret = dont_care, typename = void> \
    struct NAME : std::false_type {}; \
    \
    template <typename LHS, typename RHS, typename Ret> \
    struct NAME<LHS, RHS, Ret, void_t<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>> \
            : std::is_same<Ret, typename remove_cvref<decltype(std::declval<LHS>() != std::declval<RHS&>())>::type > {}; \
    \
    template <typename LHS, typename RHS> \
    struct NAME<LHS, RHS, dont_care, void_t<decltype(std::declval<LHS&>() OP std::declval<RHS&>())>> \
            : std::true_type {};

#define MAKE_UNARY_OP_CHECKER(NAME, OP) \
    template <typename T, typename Ret = dont_care, typename = void> \
    struct NAME : std::false_type {}; \
    \
    template <typename T, typename Ret> \
    struct NAME <T, Ret, void_t<decltype(OP std::declval<T&>())>> \
            : std::is_same<Ret, typename remove_cvref<decltype(OP std::declval<T&>())>::type > {}; \
    \
    template <typename T> \
    struct NAME <T, dont_care, void_t<decltype(OP std::declval<T&>())>> \
            : std::true_type {};

        MAKE_BINARY_OP_CHECKER(has_not_equal_to, !=)
        MAKE_UNARY_OP_CHECKER(has_pre_increment, ++)
        MAKE_UNARY_OP_CHECKER(has_deref, *)

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

        template <typename T, typename = void>
        struct is_tuple_like : std::false_type {};

        template <typename T>
        struct is_tuple_like<T, void_t<
                decltype(std::get<std::tuple_size<T>::value - 1>(std::declval<T&>()))>>
                : std::true_type{};

        template<class T> std::ostream& write_impl(std::ostream& os, const T&);

        template <typename T, typename = void>
        struct is_basic_printable : std::false_type {};

        template <typename T>
        struct is_basic_printable<T, void_t<
                decltype(std::declval<std::ostream&>() << std::declval<T&>())>>
                : std::true_type{};

        template<typename T, typename = void> struct is_printable : std::false_type {};

    }

    // prints type which have the ostream operator
    template<class T, ENABLE_IF_V(details::is_basic_printable<T>,
                                  BOOL(!std::is_array<T>::value))>
    inline std::ostream& write(std::ostream& os, const T& val) {
        return os << val;
    }

// specialization string literals, so that the overload below is not called.
    inline std::ostream& write(std::ostream& os, const char* val) {
        return os << val;
    }

    namespace details {
#define NO_BASIC_T(T, ...) ENABLE_IF_V(__VA_ARGS__, BOOL(!details::is_basic_printable<T>::value))
#define NO_BASIC(...) NO_BASIC_T(T, __VA_ARGS__)

        template<typename T, bool is_tuple = is_tuple_like<T>::value>
        struct is_printable_tuple : std::false_type {
        };

        template<size_t I, size_t N, typename T>
        struct tuple_printer {
            static inline void do_print(std::ostream &os, const T &val) {
                os << ", ";
                write_impl(os, std::get<I>(val));
                tuple_printer<I + 1, N, T>::do_print(os, val);
            }
        };

        template<size_t N, typename T>
        struct tuple_printer<0, N, T> {
            static inline void do_print(std::ostream &os, const T &val) {
                write_impl(os, std::get<0>(val));
                tuple_printer<1, N, T>::do_print(os, val);
            }
        };

        template<size_t N, typename T>
        struct tuple_printer<N, N, T> {
            static inline void do_print(std::ostream &os, const T &val) {
                os << ')';
            }
        };
    }

//overload fpr tuple-like types
    template <class T, NO_BASIC(details::is_printable_tuple<T>)>
    inline std::ostream& write(std::ostream& os, const T& val) {
        os << '(';
        details::tuple_printer<0, std::tuple_size<T>::value, T>::do_print(os, val);
        return os;
    }

#if __cpp_lib_optional
    template<class T, NO_BASIC_T(std::optional<T>,  details::is_printable<T>)>
    inline std::ostream& write(std::ostream& os, const std::optional<T>& opt) {
        if (opt.has_value())
            return details::write_impl(os << "Some(", opt.value()) << ')';
        else
            return os << "None";
    }
#endif

#if __cpp_lib_variant && __cpp_generic_lambdas
    template<class... Ts, NO_BASIC_T(std::variant<Ts...>,details::is_printable<Ts>...)>
    inline std::ostream& write(std::ostream& os, const std::variant<Ts...>& var) {
        std::visit([&](auto&& arg) { details::write_impl(os, arg); }, var);
        return os;
    }
#endif
    namespace details {
#if __cpp_lib_any
        template<class T> std::ostream& any_writer(std::ostream& os, const std::any& a) {
            return write_impl(os, std::any_cast<T>(a));
        }

#define WRITER_PAIR(T) {std::type_index(typeid(T)), &details::any_writer<T>}
#define WRITER_PAIR_U(T) WRITER_PAIR(T), WRITER_PAIR(unsigned T)

        inline std::unordered_map<std::type_index, std::ostream& (*)(std::ostream&, const std::any&)> any_writer_map = {
                WRITER_PAIR_U(char),
                WRITER_PAIR(bool),
                WRITER_PAIR_U(short),
                WRITER_PAIR_U(int),
                WRITER_PAIR_U(long),
                WRITER_PAIR_U(long long),
                WRITER_PAIR(float),
                WRITER_PAIR(double),
                WRITER_PAIR(long double),
                WRITER_PAIR(const char*),
                WRITER_PAIR(std::string),
                WRITER_PAIR(std::nullptr_t),
#if __cpp_lib_string_view
                WRITER_PAIR(std::string_view),
#endif
        };
#endif
    }

#if __cpp_lib_any
    template<class T, ENABLE_IF_V(details::is_printable<T>)>
    inline void add_any_writer() {
        details::any_writer_map.insert(WRITER_PAIR(T));
    }

    template<class T>
    inline void try_add_any_writer() {
#if __cpp_if_constexpr
        if constexpr (details::is_printable<T>::value)
            details::any_writer_map.insert(WRITER_PAIR(T));
#else
        if (is_printable<T>::value)
            details::any_writer_map.insert(WRITER_PAIR(T));
#endif
    }

    template<class T, ENABLE_IF_V(std::is_same<std::any, typename details::remove_cvref<T>::type>)>
    inline std::ostream& write(std::ostream& os, const T& a) {
        auto it = details::any_writer_map.find(std::type_index(a.type()));
        if (it == details::any_writer_map.end()) {
            return os << "(Unknown Type)";
        }else {
            return it->second(os, a);
        }
    }
#endif

    // we want to print only types that are iterable, including arrays and whose value_type is printable.
// we do not want to print types which have the ostream output operator defined or tuple-like types.
// Those types should use their output operator.
    template<class T, ENABLE_IF_V(details::is_iterable<T>, BOOL(std::is_array<T>::value ||
                                                                (!details::is_basic_printable<T>::value && !details::is_tuple_like<T>::value)),
                                  details::is_printable<decltype(*std::begin(std::declval<T&>()))>)>
    inline std::ostream& write(std::ostream& os, const T& coll) {
        auto start = std::begin(coll);
        auto stop = std::end(coll);
        os << '{';
        if (start != stop) {
            details::write_impl(os, *start);
            for (++start; start != stop; ++start) {
                os << ", "; details::write_impl(os, *start);
            }
        }
        return os << '}';
    }

    namespace details {
        template<class T>
        inline std::ostream& write_impl(std::ostream& os, const T& v) {
            return write(os, v);
        }

        template <typename T>
        struct is_printable<T, void_t<decltype(print(std::declval<T&>()))>> : std::true_type {};

        template <size_t N, typename T>
        struct is_printable_tuple_impl {
            static constexpr bool value = is_printable<decltype(std::get<N>(std::declval<T&>()))>::value
                                          && is_printable_tuple_impl<N-1, T>::value;
        };

        template <typename T>
        struct is_printable_tuple_impl<0, T> {
            static constexpr bool value = is_printable<decltype(std::get<0>(std::declval<T&>()))>::value;
        };

        template <typename T>
        struct is_printable_tuple<T, true> : is_printable_tuple_impl<std::tuple_size<T>::value-1, T> {};
    }

    template <class T, class Ch = char, class Tr = std::char_traits<Ch>>
    class ostream_iterator {
    public:
        using iterator_category = std::output_iterator_tag;
        using value_type        = void;
        using difference_type   = void;
        using pointer           = void;
        using reference         = void;

        using char_type    = Ch;
        using traits_type  = Tr;
        using ostream_type = std::basic_ostream<Ch, Tr>;

        explicit ostream_iterator(ostream_type& o) : os(std::addressof(o)) {}

        virtual ostream_iterator& operator=(const T& val) { // insert value into output stream, followed by delimiter
            write(*os, val);
            return *this;
        }

#if __has_cpp_attribute(nodiscard)
        [[nodiscard]]
#endif
        ostream_iterator& operator*() { return *this; }

        ostream_iterator& operator++() { return *this; }

        ostream_iterator<T, Ch, Tr>  operator++(int) { return *this; }

    protected:
        ostream_type* os; // pointer to output stream
    };

    template <class T, class Ch = char, class Tr = std::char_traits<Ch>>
    class ostream_iterator_delim : public ostream_iterator<T, Ch, Tr> {
        using base = ostream_iterator<T, Ch, Tr>;
    public:
        ostream_iterator_delim(typename base::ostream_type &o, const Ch *delim) : base(o), delim(delim) {}

        ostream_iterator_delim& operator=(const T& val) override { // insert value into output stream, followed by delimiter
            write(*this->os, val);
            *this->os << delim;
            return *this;
        }

    protected:
        const Ch* delim; // pointer to delimiter string
    };

    template <class T, class Ch = char, class Tr = std::char_traits<Ch>>
    class ostream_iterator_sep : public ostream_iterator<T, Ch, Tr> {
        using base = ostream_iterator<T, Ch, Tr>;
    public:
        ostream_iterator_sep(typename base::ostream_type &o, const Ch *sep)
        : base(o), sep(sep), done_first(false) {}

        ostream_iterator_sep& operator=(const T& val) override { // insert value into output stream, followed by delimiter
            if (done_first) {
                *this->os << sep;
            }else {
                done_first = true;
            }
            write(*this->os, val);
            return *this;
        }

    protected:
        const Ch* sep; // pointer to delimiter string
        bool done_first;
    };
};
using so::write;

#undef ENABLE_IF
#undef ENABLE_IF_V
#undef BOOL
#undef CONJ
#undef MAKE_BINARY_OP_CHECKER
#undef MAKE_UNARY_OP_CHECKER
#undef NO_BASIC
#undef NO_BASIC_T
#undef WRITER_PAIR
#undef WRITER_PAIR_U

#endif //SO_PRINTER_H
