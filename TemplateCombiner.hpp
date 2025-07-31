// Сохряняем в этом классе типы, инстанциируя его ими
#include <type_traits>

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

template <template <typename...> class Target, typename TypeList>
struct map_type_list;

template <template <typename...> class Target, typename... Ts>
struct map_type_list<Target, type_list<Ts...>> {
    using type = Target<Ts...>;
};

// Передаем type_list с type_list'ами, проходимся по ним и заменяем type_list на другую структуру
template <template <typename...> class Template, typename TypeListList>
struct map_to_results;

template <template <typename...> class Template, typename... Lists>
struct map_to_results<Template, type_list<Lists...>> {
    using type = type_list<typename map_type_list<Template, Lists>::type...>;
};

// Filter out type_list<>
template <typename List>
struct is_non_empty {
    static constexpr bool value = true;
};

template <>
struct is_non_empty<type_list<>> {
    static constexpr bool value = false;
};

template <typename... Lists>
struct filter_non_empty;

template <>
struct filter_non_empty<type_list<>> {
    using type = type_list<>;
};

template <typename Head, typename... Tail>
struct filter_non_empty<type_list<Head, Tail...>> {
private:
    using rest = typename filter_non_empty<type_list<Tail...>>::type;

public:
    using type = std::conditional_t<
        is_non_empty<Head>::value,
        typename prepend<Head, rest>::type,
        rest
    >;
};

// Обертка для всего этого дела
template <template <typename...> class Template, typename... Params>
struct TemplateCombiner {
    using all_combinations = typename combinations<type_list<Params...>>::type;
    using filtered = typename filter_non_empty<all_combinations>::type;
    using result_types = typename map_to_results<Template, filtered>::type;
};