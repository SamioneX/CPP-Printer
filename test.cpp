#include <vector>
#include <set>
#include <tuple>
#include <array>
#include <iostream>
#include <string>
#if __has_include(<optional>)
#include <optional>
#endif
#if __has_include(<variant>)
#include <variant>
#endif
#if __has_include(<any>)
#include <any>
#endif
#include <sstream>
#include "printer.h"

struct A {};

#define printer(...) std::cout<< #__VA_ARGS__ << " : "; \
    so::write(std::cout, __VA_ARGS__); std::cout << std::endl

int main() {
    std::cout << std::boolalpha;

    std::cout << "============================= Basic Types =============================\n\n";
    // Prints with ostream operator

    printer(bool{});

    printer(int{});

    printer("hello");   //string literal

    const char* cstr = "hello";
    printer(cstr);

    printer(std::string{"world"});

    std::cout << "\n============================= Collections =============================\n\n";
    // Prints with the iterable overload of write.

    int arr[5] = {1, 2, 3, 4, 5};
    printer(arr);

    printer(std::initializer_list<int>{5, 6, 7});

    printer(std::string{"world"});

    printer(std::unordered_map<char, size_t>{{'c', 3}, {'x', 5}});

    std::cout << "2D structure:\n";
    printer(std::vector<std::vector<int>>{{2, 3}, {4, 5}});

    std::cout << "3D structure:\n";
    printer(std::vector<std::set<std::vector<int>>>{{{1, 2}, {3, 4}}, {}});

    std::cout << "List of tuples:\n";
    printer(std::vector<std::pair<double, double>>{{1.0, 1.5}, {3.4, 9.6}});

    std::cout << "\n============================= Tuples =============================\n\n";
    printer(std::tuple<bool, double, std::string>(false, 4.5, "hello"));

    printer(std::array<int, 3>{9, -13, 2});

    printer(std::pair<size_t, std::vector<char>>{10, {'x', 'b', 'y'}});

#if __cpp_lib_variant
    std::cout << "\n============================= Variant =============================\n\n";
    printer(std::variant<int, double, std::string>("my_variant"));
#endif

#if __cpp_lib_optional
    std::cout << "\n============================= Optional =============================\n\n";
    printer(std::optional<int>(6));

    printer(std::optional<int>{});
#endif

#if __cpp_lib_any
    std::cout << "\n============================= Any =============================\n\n";
    printer(std::any{49.7});

    // Join a vector of different elements
    std::vector<std::any> any_vec{9, "apple", std::string{"candle"}, 84.6, 'c', 3738092ull, nullptr};
    std::stringstream ss;
    std::copy(any_vec.begin(), any_vec.end(),
              so::ostream_iterator_sep<std::any>(ss, ", "));

    std::cout << "Joined elems: \"" << ss.str() << "\"\n";

    // will fail: std::array<int, 2> has not been added to registry.
    // The types added to the registry by default are:
    // char    unsigned char    bool    short   unsigned short
    // int    unsigned int    long    unsigned long    long long
    // unsigned long long    float    double    long double
    // const char*    std::string    std::nullptr    std::string_view
    std::cout << "Before adding std::array<int, 2> to registry:\n";
    printer(std::any{std::array<int, 2>{3, 4}});

    so::add_any_writer<std::array<int, 2>>();

    // should work now
    std::cout << "After adding std::array<int, 2> to registry:\n";
    printer(std::any{std::array<int, 2>{3, 4}});

    // This gives a compiler error. A is not printable.
    // Use try_add_any_writer() if unsure if a type is printable.
    //---> add_any_writer<A>();

    so::try_add_any_writer<A>(); // No compiler error but nothing is added;
    printer(std::any{A{}});

    // Note: adding to the registry using add_any_writer and try_add_any_writer is not thread safe.
    // Define SO_ANY_REGISTRY_STORAGE as thread_local to make the registry thread_local or
    // Add all types you need to print using only one thread or
    // Synchronize access to the registry.
#endif

    std::cout << "\n============================= print() =============================\n\n";

    using namespace so;

    // uses global separator(default = " "), prefix(default = ""),
    // suffix(default = "\n") and stream(default = std::cout)
    so::print(std::vector<int>{2, 3}, "hello");

    // change global separator. This does not affect the internal separator used by the arguments.
    // Only print's separator is affected.
    so::sep.val = "--";
    so::print(std::vector<int>{2, 3}, "hello", flush=true);

    // passing a local separator, prefix and suffix. This doesn't affect the global ones.
    // This is equivalent to:
    // so::print(1, 'a', "world", 4.5, sep_t{", "}, prefix_t{"**"}, suffix_t{std::string{"**\n"}.data());
    so::print(1, 'a', "world", 4.5, sep=", ", prefix="**", suffix=std::string{"**\n"});

    // separator remains unchanged
    so::print(std::vector<int>{2, 3}, "hello");

    // reset sep to default
    so::sep.val = " ";

    // Change the stream
    so::print(std::initializer_list<int>{6, 8}, stream=std::cerr);

    // Note changing the global sep, prefix, suffix and stream is not thread safe.
    // Define SO_PRINT_STORAGE as thread_local before including "printers.h" to make the
    // global variables thread_local, synchronize access to it or
    // avoid changing them altogether in a multi-threaded environment.

    // All keyword args must come before arguments to be printed.
    // Keyword args can appear in any order but must not be repeated.
    // Calling print without any arguments to print simply prints the suffix.
    // All arguments passed must of course be printable.

    // --> so::print(54.4, suffix="//\n", 1);    //compiler error
    //--> so::print(A{});
}
