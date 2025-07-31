#include <iostream>
#include <cxxabi.h>
#include "TemplateCombiner.hpp"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <type_traits>
#include <tuple>

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

struct Key2 {};
struct Key3 {};
struct Key4 {};
struct Key5 {};

template<typename... Types>
struct Tag {};

struct ID {
    int key1;
};

struct Key1 {
    typedef int result_type;

    result_type operator()(const ID& aId) const
    {
        return aId.key1;
    }
};

using TAllTags = TemplateCombiner<Tag, Key1, Key2, Key3, Key4, Key5>::result_types;
using AllTags = typename map_type_list<std::tuple, TAllTags>::type;



/////////////////////////////////
/* // Шаблон, превращающий Tags<Keys...> в индекс
template <typename Tag>
struct make_index;

template <typename... Keys>
struct make_index<Tag<Keys...>> {
    using type = boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<Tag<Keys...>>,
        boost::multi_index::composite_key<
            ID,
            Keys...
        >
    >;
};
// Утилита для преобразования tuple в типовой список
template <typename Tuple, std::size_t... Is>
auto make_indexed_by_impl(std::index_sequence<Is...>) {
    return boost::multi_index::indexed_by<typename make_index<std::tuple_element_t<Is, Tuple>>::type...>{};
}

template <typename Tuple>
using make_indexed_by = decltype(make_indexed_by_impl<Tuple>(
    std::make_index_sequence<std::tuple_size_v<Tuple>>{}
));
 */

template <typename Tag>
struct make_index_from_tag;

 template <typename... Keys>
 struct make_index_from_tag<Tag<Keys...>> {
     using type = boost::multi_index::ordered_non_unique<
         boost::multi_index::tag<Tag<Keys...>>,
         boost::multi_index::composite_key<
             ID,
             Keys...
         >
     >;
 };

 template <typename List>
 struct make_index;

template <typename Tf, typename... Ts>
struct make_index<type_list<Tf, Ts...>> {
    using type = typename make_index_from_tag<Tf>::type;
};
/////////////////////////////////



using MyContainer = boost::multi_index::multi_index_container<
    ID,
    make_index<TAllTags>::type
>;

template <typename TKey1>
using Test = boost::multi_index::multi_index_container
<
    ID,
    boost::multi_index::ordered_non_unique
    <
        boost::multi_index::tag<Tag<TKey1>>,
        boost::multi_index::composite_key<ID, TKey1>
    >
>;

int main() {
    // Проверка
    print_all(TAllTags{});
    Test<Key1> xxx;
    //print_type<MyContainer>();
}