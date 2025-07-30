#include <iostream>
#include <cxxabi.h>

// Сохряняем в этом классе типы, инстанциируя его ими
template <typename... Ts>
struct type_list {};

// Добавляем в type_list последним типом тип T
template <typename List, typename T>
struct append;

template <typename... Ts, typename T>
struct append<type_list<Ts...>, T> {
    using type = type_list<Ts..., T>;
};

// Добавляем тип T первым типом в начало type_list
template <typename T, typename List>
struct prepend;

template <typename T, typename... Ts>
struct prepend<T, type_list<Ts...>> {
    using type = type_list<T, Ts...>;
};

// Объединяем два type_lists
template <typename List1, typename List2>
struct concat;

template <typename... Ts1, typename... Ts2>
struct concat<type_list<Ts1...>, type_list<Ts2...>> {
    using type = type_list<Ts1..., Ts2...>;
};

// Получаем последний тип в type_list
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

// pop_back_helper: реализует удаление последниего элемента из type_list для pop_back
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

// pop_back: удаляем последний элемент из type_list
template <typename List>
struct pop_back;

template <typename... Ts>
struct pop_back<type_list<Ts...>> {
    using type = typename pop_back_helper<Ts...>::type;
};

// Получаем на выходе type_list содержащий type_list'ы со всеми комбинациями шаблонных типов, переданных в него (типы передаются на вход завернутые в type_list)
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

// Заменяем type_list в type_list<Ts...> на структуру result<Ts...>
template<typename... Types>
struct xxx {};

template <typename TypeList>
struct to_result;

template <typename... Ts>
struct to_result<type_list<Ts...>> {
    using type = xxx<Ts...>;
};

// Передаем type_list с type_list'ами, проходимся по ним и заменяем type_list на другую структуру
template <typename TypeListList>
struct map_to_results;

template <typename... Lists>
struct map_to_results<type_list<Lists...>> {
    using type = type_list<typename to_result<Lists>::type...>;
};

// Обертка для всего этого дела
template <typename... Params>
struct TemplateCombiner {
    using all_combinations = typename map_to_results<typename combinations<type_list<Params...>>::type>::type;
};



// Вывод в консоль типа с после деманглинга
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

// Структуры для комбинирования
struct A {};
struct B {};
struct C {};
struct D {};
struct E {};

int main() {
    // Проверка
    using combos = TemplateCombiner<A, B, C, D, E>::all_combinations;
    print_all(combos{});
}
