#include <tuple>
#include <type_traits>
#include <iostream>
#include <cxxabi.h>

// Сохряняем в этом классе типы, инстанциируя его ими
template <typename... Ts>
struct type_list {};

// Append a type T at the end of a type_list
template <typename List, typename T>
struct append;

template <typename... Ts, typename T>
struct append<type_list<Ts...>, T> {
    using type = type_list<Ts..., T>;
};

// Prepend a type T at the front of a type_list
template <typename T, typename List>
struct prepend;

template <typename T, typename... Ts>
struct prepend<T, type_list<Ts...>> {
    using type = type_list<T, Ts...>;
};

// Concat two type_lists
template <typename List1, typename List2>
struct concat;

template <typename... Ts1, typename... Ts2>
struct concat<type_list<Ts1...>, type_list<Ts2...>> {
    using type = type_list<Ts1..., Ts2...>;
};

// Extract last type from a type_list
template <typename List>
struct last_type;

template <typename T>
struct last_type<type_list<T>> {
    using type = T;
};

template <typename T, typename... Ts>
struct last_type<type_list<T, Ts...>> {
    using type = typename last_type<type_list<Ts...>>::type;
};

// pop_back_helper: recursively removes the last element, preserving order
template <typename... Ts>
struct pop_back_helper;

template <>
struct pop_back_helper<> {
    using type = type_list<>;
};

template <typename T>
struct pop_back_helper<T> {
    using type = type_list<>;
};

template <typename T, typename U, typename... Ts>
struct pop_back_helper<T, U, Ts...> {
    using type = typename prepend<
        T,
        typename pop_back_helper<U, Ts...>::type
    >::type;
};

// pop_back: removes last element from a type_list
template <typename List>
struct pop_back;

template <typename... Ts>
struct pop_back<type_list<Ts...>> {
    using type = typename pop_back_helper<Ts...>::type;
};

// Combinations - process from right to left to preserve original order
template <typename List>
struct combinations;

template <>
struct combinations<type_list<>> {
    using type = type_list<type_list<>>;
};

template <typename... Ts>
struct combinations<type_list<Ts...>> {
private:
    using last = typename last_type<type_list<Ts...>>::type;
    using rest = typename pop_back<type_list<Ts...>>::type;
    using tail_combos = typename combinations<rest>::type;

    template <typename Combo>
    struct append_last {
        using type = typename append<Combo, last>::type;
    };

    template <typename... Combos>
    static type_list<typename append_last<Combos>::type...> add_last(type_list<Combos...>);

public:
    using type = typename concat<
        tail_combos,
        decltype(add_last(tail_combos{}))
    >::type;
};

// Convert type_list<Ts...> to std::tuple<Ts...>
template <typename TypeList>
struct to_tuple;

template<typename... Types>
struct cnt {};

template <typename... Ts>
struct to_tuple<type_list<Ts...>> {
    using type = cnt<Ts...>;
};

// Map list of type_lists to list of tuples
template <typename TypeListList>
struct map_to_tuples;

template <typename... Lists>
struct map_to_tuples<type_list<Lists...>> {
    using type = type_list<typename to_tuple<Lists>::type...>;
};

// TemplateCombiner wrapper
template <typename... Params>
struct TemplateCombiner {
    using all_combinations = typename map_to_tuples<
        typename combinations<type_list<Params...>>::type
    >::type;
};










// Demangle helper to print types nicely
template <typename T>
void print_type() {
    int status = 0;
    char* name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
    std::cout << (status == 0 ? name : typeid(T).name()) << '\n';
    std::free(name);
}

template <typename... Ts>
void print_all(type_list<Ts...>) {
    (print_type<Ts>(), ...);
}

// Demo structs
struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

int main() {
    using combos = TemplateCombiner<A, B, C, D, E>::all_combinations;
    print_all(combos{});
}
