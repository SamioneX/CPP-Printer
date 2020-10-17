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
#endif
#include <sstream>
#include "printer.h"


#define printer(obj) std::cout << #obj << " : "; println(obj); std::cout << '\n'

#define printer_t(type, obj) std::cout << #type << " : "; println(obj); std::cout << '\n'

struct A {};

using namespace std;

int main() {
    cout << boolalpha;

    cout << "============================= Basic Types =============================\n\n";
    // Prints with ostream operator

    printer(bool{});

    printer(int{});

    printer("hello");   //string literal

    const char* cstr = "hello";
    printer_t(const char*, cstr);

    printer(std::string{"world"});

    cout << "============================= Collections =============================\n\n";
    // Prints with the iterable overload of write.

    int arr[5] = {1, 2, 3, 4, 5};
    printer_t(int[5], arr);

    auto il = {5, 6, 7};
    printer_t(std::initializer_list<int>, il);

    printer(std::string{"world"});

    printer((std::unordered_map<char, size_t>{{'c', 3}, {'x', 5}}));

    cout << "2D structure:\n";
    printer((std::vector<std::vector<int>>{{2, 3}, {4, 5}}));

    cout << "3D structure:\n";
    printer((std::vector<std::set<std::vector<int>>>{{{1, 2}, {3, 4}}, {}}));

    cout << "List of tuples:\n";
    printer((std::vector<std::pair<double, double>>{{1.0, 1.5}, {3.4, 9.6}}));

    cout << "============================= Tuples =============================\n\n";
    printer((std::tuple<bool, double, std::string>{false, 4.5, "hello"}));

    printer((std::array<int, 3>{9, -13, 2}));

    printer((std::pair<size_t, std::vector<char>>{10, {'x', 'b', 'y'}}));

#if __cpp_lib_variant
    cout << "============================= Variant =============================\n\n";
    printer((std::variant<int, double, std::string>("my_variant")));
#endif

#if __cpp_lib_optional
    cout << "============================= Optional =============================\n\n";
    printer((std::optional<int>(6)));

    printer((std::optional<int>{}));
#endif

#if __cpp_lib_any
    cout << "============================= Any =============================\n\n";
    printer(std::any{49.7});

    // Join a vector of different elements
    std::vector<any> any_vec{9, "apple", "candle"s, "book"sv, 84.6, 'c', 3738092ull, nullptr};
    std::stringstream ss;
    std::copy(any_vec.begin(), any_vec.end(),
                     so::ostream_iterator_sep<any>(ss, ", "));


    cout << "Joined elems: \"" << ss.str() << "\"\n";


    // will fail: std::array<int, 2> has not been added to registry.
    // The types added to the registry by default are:
    // char    unsigned char    bool    short   unsigned short
    // int    unsigned int    long    unsigned long    long long
    // unsigned long long    float    double    long double
    // const char*    std::string    std::nullptr    std::string_view
    cout << "Before adding std::array<int, 2> to registry:\n";
    printer((std::any{std::array<int, 2>{3, 4}}));

    so::add_any_writer<std::array<int, 2>>();

    // should work now
    cout << "After adding std::array<int, 2> to registry:\n";
    printer((std::any{std::array<int, 2>{3, 4}}));

    // This gives a compiler error. A is not printable.
    // Use try_add_any_writer() if unsure if a type is printable.
    //---> add_any_writer<A>();

    so::try_add_any_writer<A>(); // No compiler error but nothing is added;
    printer(std::any{A{}});
#endif

}
