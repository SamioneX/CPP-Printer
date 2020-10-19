#ifndef SO_PRINTER_H
#define SO_PRINTER_H

#include <iostream>
#include "type_traits/has_op.h"
#include "type_traits/logical.h"
#include "type_traits/is_tuple_like.h"
#include "type_traits/is_iterable.h"
#include "type_traits/is_in.h"
#include "type_traits/index_of.h"
#include "type_traits/all.h"
#include "type_traits/split_at.h"

#include <tuple>

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

#if __cpp_if_constexpr
#define IFC if constexpr
#else
#define IFC if
#endif

#ifndef SO_ANY_REGISTRY_STORAGE
#define SO_ANY_REGISTRY_STORAGE
#endif

#ifndef SO_PRINT_STORAGE
#define SO_PRINT_STORAGE
#endif

#if __cpp_inline_variables
#define INLINE_PRINT SO_PRINT_STORAGE inline
#define INLINE_REG SO_ANY_REGISTRY_STORAGE inline
#else
#define INLINE_PRINT SO_PRINT_STORAGE
#define INLINE_REG SO_ANY_REGISTRY_STORAGE
#endif

namespace so {
    namespace details {
        template<class T> void write_impl(std::ostream& os, const T&);

        template<typename T, bool is_tuple = is_tuple_like<T>::value>
        struct is_printable_tuple : std::false_type {};

        template<std::size_t> struct Index{};

        template <size_t END, class Tuple, size_t Idx, ENABLE_IF(Idx != END)>
        void print_tuple(std::ostream& out, const Tuple& t, Index<Idx>, const char* sep) {
            write_impl(out, std::get<Idx>(t));
            out << sep;
            print_tuple<END>(out, t, Index<Idx + 1>{}, sep);
        }

        template <size_t END, class Tuple>
        inline void print_tuple(std::ostream& out, const Tuple& t, Index<END-1>, const char*) {
            write_impl(out, std::get<END-1>(t));
        }

        template <size_t END, class Tuple>
        inline void print_tuple(std::ostream&, const Tuple&, Index<END>, const char*) {}

#if __cpp_lib_any
        template<class T> void any_writer(std::ostream& os, const std::any& a) {
            write_impl(os, std::any_cast<T>(a));
        }

#define WRITER_PAIR(T) {std::type_index(typeid(T)), &details::any_writer<T>}
#define WRITER_PAIR_U(T) WRITER_PAIR(T), WRITER_PAIR(unsigned T)

        INLINE_REG std::unordered_map<std::type_index, void(*)(std::ostream&, const std::any&)> any_writer_map = {
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

    template<typename T, typename = void> struct is_printable : std::false_type {};

#define NOT(...) negation<__VA_ARGS__>
#define AND(...) conjunction<__VA_ARGS__>
#define OR(...) disjunction<__VA_ARGS__>
#define DPR(...) has_left_shift<std::ostream&, __VA_ARGS__>     //default printer
#define PR(...) is_printable<__VA_ARGS__>
#define ARR(...) std::is_array<__VA_ARGS__>

    // prints type which have the ostream operator
    template<class T, ENABLE_IF_V(DPR(T), NOT(ARR(T)))>
    inline void write(std::ostream& os, const T& val) {
        os << val;
    }

    // specialization string literals, so that the overload below is not called.
    inline void write(std::ostream& os, const char* val) {
        os << val;
    }

    //overload fpr tuple-like types including.
    template <class T, ENABLE_IF_V(NOT(DPR(T)), details::is_printable_tuple<T>)>
    inline void write(std::ostream& os, const T& val) {
        os << '(';
        details::print_tuple<std::tuple_size<T>::value>(os, val, details::Index<0>{}, ", ");
        os << ')';
    }

#if __cpp_lib_optional
    template<class T, ENABLE_IF_V(NOT(DPR(std::optional<T>)), PR(T))>
    inline void write(std::ostream& os, const std::optional<T>& opt) {
        if (opt.has_value()) {
            details::write_impl(os << "Some(", opt.value());
            os << ')';
        }
        else os << "None";
    }
#endif

#if __cpp_lib_variant && __cpp_generic_lambdas
    template<class... Ts, ENABLE_IF_V(NOT(DPR(std::variant<Ts...>)), PR(Ts)...)>
    inline void write(std::ostream& val, const std::variant<Ts...>& var) {
        std::visit([&](auto&& arg) { details::write_impl(val, arg); }, var);
    }
#endif

#if __cpp_lib_any
    template<class T, ENABLE_IF_V(PR(T))>
    inline void add_any_writer() {
        details::any_writer_map.insert(WRITER_PAIR(T));
    }

    template<class T>
    inline void try_add_any_writer() {
#if __cpp_if_constexpr
        if constexpr (PR(T)::value)
            details::any_writer_map.insert(WRITER_PAIR(T));
#else
        if (is_printable<T>::value)
            details::any_writer_map.insert(WRITER_PAIR(T));
#endif
    }
#undef WRITER_PAIR
#undef WRITER_PAIR_U

    template<class T, ENABLE_IF_V(std::is_same<std::any, typename remove_cvref<T>::type>)>
    inline void write(std::ostream& os, const T& a) {
        auto it = details::any_writer_map.find(std::type_index(a.type()));
        if (it == details::any_writer_map.end()) {
            os << "(Unknown Type)";
        }else {
            it->second(os, a);
        }
    }
#endif

    // we want to print only types that are iterable, not tuple-like,
    // is an array or is not DPR. Arrays in c++ are implicitly convertible to pointers
    // which are DPR. So we have to check for this.
    template<class T, ENABLE_IF_V(is_iterable<T>, OR(ARR(T), NOT(DPR(T))), NOT(is_tuple_like<T>))>
    inline void write(std::ostream& os, const T& coll) {
        auto start = std::begin(coll);
        auto stop = std::end(coll);
        os << '{';
        if (start != stop) {
            details::write_impl(os, *start);
            for (++start; start != stop; ++start) {
                os << ", "; details::write_impl(os, *start);
            }
        }
        os << '}';
    }

    template <typename T>
    struct is_printable<T, void_t<decltype(write(std::cout, std::declval<T&>()))>> : std::true_type {};

    namespace details {
        template<class T>
        inline void write_impl(std::ostream& os, const T& v) {
            return write(os, v);
        }

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

    template <class T = void, class Ch = char, class Tr = std::char_traits<Ch>>
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

        virtual void write(const T& val) {
            so::write(*os, val);
        }

        explicit ostream_iterator(ostream_type& o) : os(std::addressof(o)) {}

        inline ostream_iterator& operator=(const T& val) { // insert value into output stream, followed by delimiter
            write(val);
            return *this;
        }

#if __has_cpp_attribute(nodiscard)
        [[nodiscard]]
#endif
        inline ostream_iterator& operator*() { return *this; }

        inline ostream_iterator& operator++() { return *this; }

        inline ostream_iterator<T, Ch, Tr>  operator++(int) { return *this; }

    protected:
        ostream_type* os; // pointer to output stream
    };

    template <class T, class Ch = char, class Tr = std::char_traits<Ch>>
    class ostream_iterator_delim : public ostream_iterator<T, Ch, Tr> {
        using base = ostream_iterator<T, Ch, Tr>;
    public:
        ostream_iterator_delim(typename base::ostream_type &o, const Ch *delim) : base(o), delim(delim) {}

        void write(const T& val) override {
            so::write(*this->os, val);
            *this->os << delim;
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

        void write(const T& val) override {
            if (done_first) {
                *this->os << sep;
            }else {
                done_first = true;
            }
            so::write(*this->os, val);
        }

    protected:
        const Ch* sep; // pointer to delimiter string
        bool done_first;
    };

    namespace details {
        template <class T, typename = void>
        struct is_str_type : std::false_type {};

        template <class T>
        struct is_str_type<T, void_t<decltype(std::declval<T&>().data())>>
                : std::is_convertible<decltype(std::declval<T&>().data()), const char*> {};
    }

    struct flush_t {
        bool val;
        flush_t operator=(bool v) const { return flush_t{v}; }
    };

    struct sep_t {
        const char* val;
        sep_t operator=(const char* s) const { return sep_t{s}; }

        template<class T, ENABLE_IF_V(details::is_str_type<T>)>
        sep_t operator=(const T& s) const { return sep_t{s.data()}; }
    };

    struct prefix_t {
        const char* val;
        prefix_t operator=(const char* s) const { return prefix_t{s}; }

        template<class T, ENABLE_IF_V(details::is_str_type<T>)>
        prefix_t operator=(const T& s) const { return prefix_t{s.data()}; }
    };

    struct suffix_t {
        const char* val;
        suffix_t operator=(const char* s) const { return suffix_t{s}; }

        template<class T, ENABLE_IF_V(details::is_str_type<T>)>
        suffix_t operator=(const T& s) const { return suffix_t{s.data()}; }
    };

    struct stream_t {
        std::ostream* val;
        stream_t operator=(std::ostream& v) const { return stream_t{&v}; }
    };

    namespace details {
        struct has_kwarg {
            template<class T>
            using apply = is_in<typename remove_cvref<T>::type , sep_t, suffix_t, prefix_t, stream_t, flush_t>;
        };

        struct is_printable_fun {
            template<class T>
            using apply = is_printable<T>;
        };
    }

    INLINE_PRINT flush_t flush{false};
    INLINE_PRINT sep_t sep{" "};
    INLINE_PRINT suffix_t suffix{"\n"};
    INLINE_PRINT prefix_t prefix{""};
    INLINE_PRINT stream_t stream{&std::cout};

    template<class T, class Tpl, size_t I, size_t N>
    constexpr inline T get_arg(const Tpl& tpl, T, details::Index<I>, details::Index<N>) {
        return std::get<I>(tpl).val;
    }

    template<class T, class Tpl, size_t N>
    constexpr inline T get_arg(const Tpl& tpl, T kwarg, details::Index<N>, details::Index<N>) {
        return kwarg;
    }

    template <class... Ts,
            size_t I = index_of_fun<details::has_kwarg, Ts...>::value,
            size_t N = sizeof...(Ts),
            class SP = split_at<I, Ts...>, ENABLE_IF_V(
                    all_of_in<details::has_kwarg, typename SP::right>,
                    all_unique_in<typename SP::right>,
                    all_of_in<details::is_printable_fun, typename SP::left>)>
    constexpr inline void print(Ts&&... ts) {
        auto tpl = std::forward_as_tuple(ts...);
#define IKWARG(t) I + index_of_in<t, typename SP::right>::value
#define IDX(v) details::Index<v>{}

        auto& _stream = *get_arg(tpl, stream.val, IDX(IKWARG(stream_t)), IDX(N));

        details::print_tuple<std::tuple_size<typename SP::left>::value>(
                _stream << get_arg(tpl, prefix.val, IDX(IKWARG(prefix_t)), IDX(N)),
                tpl, IDX(0), get_arg(tpl, sep.val, IDX(IKWARG(sep_t)), IDX(N)));

        _stream << get_arg(tpl, suffix.val, IDX(IKWARG(suffix_t)), IDX(N));
        if (get_arg(tpl, flush.val, IDX(IKWARG(flush_t)), IDX(N)))
            _stream.flush();

#undef IKWARG
#undef IDX
    }

}


#endif //SO_PRINTER_H
