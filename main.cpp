#include <iostream>
#include <cxxabi.h>
#include "ContainerGenerator.hpp"


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



struct ID {
    int key1;
    int key2;
    int key3;
    int key4;
    int key5;

    // Для удобного вывода
    friend std::ostream& operator<<(std::ostream& os, const ID& id) {
        return os << "{" << id.key1 << ", " << id.key2 << ", " << id.key3
                  << ", " << id.key4 << ", " << id.key5 << "}";
    }
};

struct Key1 {
    using result_type = int;

    result_type operator()(const ID& aId) const
    {
        return aId.key1;
    }
};

struct Key2 {
    using result_type = int;

    result_type operator()(const ID& aId) const
    {
        return aId.key2;
    }
};

struct Key3 {
    using result_type = int;

    result_type operator()(const ID& aId) const
    {
        return aId.key3;
    }
};

struct Key4 {
    using result_type = int;

    result_type operator()(const ID& aId) const
    {
        return aId.key4;
    }
};

struct Key5 {
    using result_type = int;

    result_type operator()(const ID& aId) const
    {
        return aId.key5;
    }
};

using TContainer = typename ContainerGenerator
<
    ID,
    boost::multi_index::hashed_unique,
    boost::multi_index::hashed_non_unique,
    Key1, Key2, Key3, Key4
>::TContainer;

int main() {
    // Проверка
    TContainer xxx;
    // Добавим несколько элементов
    xxx.insert({1, 2, 3, 4, 5});
    xxx.insert({5, 3, 3, 2, 1});
    xxx.insert({3, 3, 3, 3, 3});

    const auto& index_by_key3 = xxx.get<Tag<Key1, Key2, Key3, Key4>>();
    std::cout << "Elements with key3 == 3:\n";
    auto range = index_by_key3.equal_range(std::tuple{5, 3, 3, 2});
    for (auto it = range.first; it != range.second; ++it) {
        std::cout << *it << "\n";
    }

    //print_type<TContainer>();
}