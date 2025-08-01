#include "TemplateCombiner.hpp"
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>


template<typename... TKeys>
struct Tag {};

template
<
    class TStruct,
    template<typename...> class TUniqueIndexType = boost::multi_index::hashed_unique,
    template<typename...> class TNonUniqueIndexType = boost::multi_index::hashed_non_unique,
    typename... Params
>
class ContainerGenerator {

    // Метафункция для преобразования Tag<...> → std::tuple<...> с параметром TStruct
    template<typename TStructInternal, typename T>
    struct tag_to_index;

    // Специализация для Tag<Keys...>
    template<typename TStructInternal, typename... Keys>
    struct tag_to_index<TStructInternal, Tag<Keys...>> {
        using type = TNonUniqueIndexType
        <
            boost::multi_index::tag<Tag<Keys...>>,
            boost::multi_index::composite_key<TStructInternal, Keys...>
        >;
    };

    // Метафункция для преобразования type_list<Tag<...>...> → type_list<index<...>...>
    template<typename TStructInternal, typename TypeList>
    struct convert_tags_to_index;

    template<typename TStructInternal, typename... Tags>
    struct convert_tags_to_index<TStructInternal, type_list<Tags...>> {
        using type = type_list<typename tag_to_index<TStructInternal, Tags>::type...>;
    };


    template<typename TStructInternal, typename T>
    struct tag_to_unique_index;

    template<typename TStructInternal, typename... Keys>
    struct tag_to_unique_index<TStructInternal, Tag<Keys...>> {
        using type = TUniqueIndexType
        <
            boost::multi_index::tag<Tag<Keys...>>,
            boost::multi_index::composite_key<TStructInternal, Keys...>
        >;
    };

    using TFilteredTags = typename TemplateCombiner<Tag, Params...>::result_types;
    using TNonUniqueIndexes = typename convert_tags_to_index<TStruct, TFilteredTags>::type;
    using TUniqueIndex = typename tag_to_unique_index<TStruct, Tag<Params...>>::type;
    using TFullIndexPack = typename append<TNonUniqueIndexes, TUniqueIndex>::type;
    using TIndexdBy = typename map_type_list<boost::multi_index::indexed_by, TFullIndexPack>::type;

public:
    template<typename... Types>
    using TTag = Tag<Types...>;

    using TContainer = boost::multi_index::multi_index_container
    <
        TStruct,
        TIndexdBy
    >;
};
