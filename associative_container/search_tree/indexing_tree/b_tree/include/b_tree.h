#ifndef MP_OS_B_TREE_H
#define MP_OS_B_TREE_H

#include <iterator>
#include <utility>
#include <boost/container/static_vector.hpp>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class B_tree final : private logger_guardant, private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<const tkey, tvalue>;
    using value_type = tree_data_type_const;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;
    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration


    struct btree_node
    {
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<btree_node*, maximum_keys_in_node + 2> _pointers;
        btree_node() noexcept;
//        std::vector<tree_data_type, pp_allocator<tree_data_type>> _keys;
//        std::vector<btree_node*, pp_allocator<btree_node*>> _pointers;

//        btree_node(pp_allocator<value_type> al);
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    btree_node* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit B_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit B_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit B_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    B_tree(const B_tree& other);

    B_tree(B_tree&& other) noexcept;

    B_tree& operator=(const B_tree& other);

    B_tree& operator=(B_tree&& other) noexcept;

    ~B_tree() noexcept override;

    // endregion five declaration

    // region iterators declaration

    class btree_iterator;
    class btree_reverse_iterator;
    class btree_const_iterator;
    class btree_const_reverse_iterator;

    class btree_iterator final
    {
        //Путь хранится в следующем формате:
        //<указатель на ноду, номер этой ноды в _pointers ее родителя>
        //у корня номер = -1
        //index таким образом показывает номер ноды в _path.top()->_pointers
        //вряд ли вы что-то поняли из этого, этот комментарий временный и его надо переписать
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);

    };

    class btree_const_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_iterator;
        friend class btree_const_reverse_iterator;

        btree_const_iterator(const btree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    class btree_reverse_iterator final
    {
        std::stack<std::pair<btree_node**, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_reverse_iterator;

        friend class B_tree;
        friend class btree_iterator;
        friend class btree_const_iterator;
        friend class btree_const_reverse_iterator;

        btree_reverse_iterator(const btree_iterator& it) noexcept;
        operator btree_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_reverse_iterator(const std::stack<std::pair<btree_node**, size_t>>& path = std::stack<std::pair<btree_node**, size_t>>(), size_t index = 0);
    };

    class btree_const_reverse_iterator final
    {
        std::stack<std::pair<btree_node* const*, size_t>> _path;
        size_t _index;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = btree_const_reverse_iterator;

        friend class B_tree;
        friend class btree_reverse_iterator;
        friend class btree_const_iterator;
        friend class btree_iterator;

        btree_const_reverse_iterator(const btree_reverse_iterator& it) noexcept;
        operator btree_const_iterator() const noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t depth() const noexcept;
        size_t current_node_keys_count() const noexcept;
        bool is_terminate_node() const noexcept;
        size_t index() const noexcept;

        explicit btree_const_reverse_iterator(const std::stack<std::pair<btree_node* const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;
    friend class btree_reverse_iterator;
    friend class btree_const_reverse_iterator;

    // endregion iterators declaration

    // region element access declaration

    /*
     * Returns a reference to the mapped value of the element with specified key. If no such element exists, an exception of type std::out_of_range is thrown.
     */
    tvalue& at(const tkey&);
    const tvalue& at(const tkey&) const;

    /*
     * If key not exists, makes default initialization of value
     */
    tvalue& operator[](const tkey& key);
    tvalue& operator[](tkey&& key);

    // endregion element access declaration
    // region iterator begins declaration

    btree_iterator begin();
    btree_iterator end();

    btree_const_iterator begin() const;
    btree_const_iterator end() const;

    btree_const_iterator cbegin() const;
    btree_const_iterator cend() const;

    btree_reverse_iterator rbegin();
    btree_reverse_iterator rend();

    btree_const_reverse_iterator rbegin() const;
    btree_const_reverse_iterator rend() const;

    btree_const_reverse_iterator crbegin() const;
    btree_const_reverse_iterator crend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    btree_iterator find(const tkey& key);
    btree_const_iterator find(const tkey& key) const;

    btree_iterator lower_bound(const tkey& key);
    btree_const_iterator lower_bound(const tkey& key) const;

    btree_iterator upper_bound(const tkey& key);
    btree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<btree_iterator, bool> insert(const tree_data_type& data);
    std::pair<btree_iterator, bool> insert(tree_data_type&& data);

    template <typename ...Args>
    std::pair<btree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    btree_iterator insert_or_assign(const tree_data_type& data);
    btree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    btree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    btree_iterator erase(btree_iterator pos);
    btree_iterator erase(btree_const_iterator pos);

    btree_iterator erase(btree_iterator beg, btree_iterator en);
    btree_iterator erase(btree_const_iterator beg, btree_const_iterator en);


    btree_iterator erase(const tkey& key);

    void delete_key_from_leaf(size_t, B_tree::btree_node*, std::stack<std::pair<btree_node**, size_t>>);
    std::pair<btree_node*, btree_node*> split(btree_node*, btree_node*);
    void merge(B_tree::btree_node*, B_tree::btree_node*, B_tree::btree_node*, size_t);

    // endregion modifiers declaration
};

template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
B_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
B_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
       logger *logger = nullptr) -> B_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_pairs(const B_tree::tree_data_type &lhs,
                                                     const B_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_node::btree_node() noexcept : _keys(), _pointers()
{

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger* B_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename B_tree<tkey, tvalue, compare, t>::value_type> B_tree<tkey, tvalue, compare, t>::get_allocator() const noexcept
{
    return _allocator;
}

// region constructors implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
{
    _root = nullptr;
    _allocator = alloc;
    _logger = logger;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        pp_allocator<value_type> alloc,\
        const compare& comp,
        logger* logger)
{
    _root = nullptr;
    _allocator = alloc;
    _logger = logger;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
B_tree<tkey, tvalue, compare, t>::B_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
{
    _root = nullptr;
    _allocator = alloc;
    _logger = logger;
    _size = 0;
    for(auto it = begin; it != end; it++){
        insert(it);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* logger)
{
    _root = nullptr;
    _allocator = alloc;
    _logger = logger;
    _size = 0;
    for (std::pair<tkey, tvalue> p: data) {
        insert(p.second);
    }
}

// endregion constructors implementation

// region five implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::~B_tree() noexcept
{
    clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(const B_tree& other)
{
    clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept", "your code should be here...");
}

// endregion five implementation

// region iterators implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)
{
    _path = path;
    _index = index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator& B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(\n"
                          "const std::stack<std::pair<const btree_node**, size_t>>& path, size_t index)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept
{

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(\n"
                          "const std::stack<std::pair<btree_node**, size_t>>& path, size_t index)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const btree_iterator& it) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(\n"
                          "const btree_iterator& it) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator btree_iterator() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(\n"
                          "const std::stack<std::pair<const btree_node**, size_t>>& path, size_t index)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const btree_reverse_iterator& it) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(\n"
                          "const btree_reverse_iterator& it) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator btree_const_iterator() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept", "your code should be here...");
}

// endregion iterators implementation

// region element access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)", "your code should be here...");
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const", "your code should be here...");
}

// endregion iterator begins implementation

// region lookup implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{
    size_t i = 0;
    auto cur_pointer = _root;
    auto cur_key = _root->_keys[i];
    std::stack<std::pair<btree_node**, size_t>> st = std::stack<std::pair<btree_node**, size_t>>();
    st.push(std::pair<btree_node**, size_t>(&_root, 0));
    while(cur_pointer != nullptr){
        i = 0;
        cur_key = cur_pointer->_keys[i];
        while(compare_keys(cur_key, key)){
            cur_key = cur_pointer->_keys[++i];
        }
        if (cur_key == key){
            return btree_iterator(st, i);
        }
        st.push(std::pair<btree_node**, size_t>(&cur_pointer, i));
        cur_pointer = cur_pointer->_pointers[i];
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    size_t i = 0;
    auto cur_pointer = _root;
    auto cur_key = _root->_keys[i];
    std::stack<std::pair<btree_node**, size_t>> st = std::stack<std::pair<btree_node**, size_t>>();
    st.push(std::pair<btree_node**, size_t>(&_root, 0));
    while(cur_pointer != nullptr){
        i = 0;
        cur_key = cur_pointer->_keys[i];
        while(compare_keys(cur_key, key)){
            cur_key = cur_pointer->_keys[++i];
        }
        if (cur_key == key){
            return btree_iterator(st, i);
        }
        st.push(std::pair<btree_node**, size_t>(&cur_pointer, i));
        cur_pointer = cur_pointer->_pointers[i];
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const", "your code should be here...");
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    erase(begin(), end());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_node*, typename B_tree<tkey, tvalue, compare, t>::btree_node*>
        B_tree<tkey, tvalue, compare, t>::split(B_tree::btree_node * node_to_split, B_tree::btree_node * parent_node) {
    tkey middle_key = node_to_split->_keys[maximum_keys_in_node / 2].first;
    tvalue middle_value = node_to_split->_keys[maximum_keys_in_node / 2].second;
    size_t middle_index;
    if (parent_node == nullptr) {
        _root = _allocator.template new_object<B_tree::btree_node>();
//        _root->_keys.push_back(node_to_split->_keys[maximum_keys_in_node / 2]);
        parent_node = _root;
        middle_index = 0;
    } else {
        size_t i;
        for(i = 0; i < parent_node->_keys.size(); i++){
            if (!compare_keys(parent_node->_keys[i].first, middle_key)){
//                parent_node->_keys.insert(parent_node->_keys.begin() + i, tree_data_type(middle_key, middle_value));
                break;
            }
        }
        middle_index = i;
    }
    B_tree::btree_node * left_part = _allocator.template new_object<B_tree::btree_node>();
    B_tree::btree_node * right_part = _allocator.template new_object<B_tree::btree_node>();
    if (left_part == nullptr || right_part == nullptr){
        if (left_part != nullptr){
            _allocator.template delete_object<B_tree::btree_node>(left_part);
        }
        if (right_part != nullptr){
            _allocator.template delete_object<B_tree::btree_node>(right_part);
        }
        return std::pair(nullptr, nullptr);
    }

    const auto it_keys_begin = node_to_split->_keys.begin();
    const auto it_keys_end = node_to_split->_keys.end();
    const auto it_point_begin = node_to_split->_pointers.begin();
    const auto it_point_end = node_to_split->_pointers.end();

    left_part->_keys.insert(left_part->_keys.begin(), it_keys_begin, it_keys_begin + maximum_keys_in_node / 2);
    left_part->_pointers.insert(left_part->_pointers.begin(), it_point_begin, it_point_begin + maximum_keys_in_node / 2);

    right_part->_keys.insert(right_part->_keys.begin(), it_keys_begin + maximum_keys_in_node / 2 + 1, it_keys_end);
    right_part->_pointers.insert(right_part->_pointers.begin(), it_point_begin + maximum_keys_in_node / 2 + 1, it_point_end);

    parent_node->_keys.insert(parent_node->_keys.begin() + middle_index, node_to_split->_keys[maximum_keys_in_node / 2]);
    parent_node->_pointers.insert(parent_node->_pointers.begin() + middle_index, left_part);
    parent_node->_pointers.insert(parent_node->_pointers.begin() + middle_index + 1, right_part);
    return std::pair(left_part, right_part);

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data)
{
    size_t prev_i;
    B_tree<tkey, tvalue, compare, t>::btree_node *prev = nullptr;
    B_tree<tkey, tvalue, compare, t>::btree_node *prev_parent = nullptr;
    auto cur_pointer = _root;
    std::stack<std::pair<btree_node**, size_t>> st = std::stack<std::pair<btree_node**, size_t>>();

    if (_root == nullptr){
        _root = _allocator.template new_object<B_tree::btree_node>();
        _root->_keys.insert(_root->_keys.begin(), data);
        _root->_pointers.insert(_root->_pointers.begin(), nullptr);
        prev_i = 0;
    } else {
        auto cur_key = _root->_keys[0].first;
        size_t i = 0;
        prev_i = 0;
        while(cur_pointer != nullptr){

            i = 0;
            prev_i = 0;
            cur_key = cur_pointer->_keys[prev_i].first;
            for(; i < cur_pointer->_keys.size() && compare_keys(cur_pointer->_keys[i].first, data.first); prev_i = i++){

            }

            cur_key = cur_pointer->_keys[prev_i].first;

            if (cur_key == data.first){
                return std::pair<B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>(end(), false);
            }
            prev_parent = prev;
            prev = cur_pointer;


            if (i < cur_pointer->_pointers.size()){
                cur_pointer = cur_pointer->_pointers[i];
            } else {
                cur_pointer = nullptr;
            }
            if (cur_pointer != nullptr){
                st.push(std::move(std::pair<btree_node**, size_t>(&prev, 0))); //fix
            }
        }
        if (prev->_keys.size() == maximum_keys_in_node - 1){
            prev->_keys.insert(prev->_keys.begin() + i, data);
            prev->_pointers.insert(prev->_pointers.begin() + i, nullptr);
            auto pair = split(prev, prev_parent);
            size_t new_index = i - pair.first->_keys.size() - 1;
        } else {
            prev->_keys.insert(prev->_keys.begin() + i, data);
            prev->_pointers.insert(prev->_pointers.begin() + i, nullptr);
        }
    }
    return std::pair<B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>(B_tree<tkey, tvalue, compare, t>::btree_iterator(st, prev_i), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    return insert(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    return this->insert(tree_data_type(args ...));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "template<typename... Args>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "typename B_tree<tkey, tvalue, compare, t>::btree_iterator\n"
                          "B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    size_t i = 0;
    size_t prev_i = -1;
    auto cur_node = _root;
    std::stack<std::pair<btree_node**, size_t>> st = std::stack<std::pair<btree_node**, size_t>>();
    //находим нужный ключ
    while(cur_node != nullptr){
        i = 0;
        for(; compare_keys(cur_node->_keys[i].first, key); i++){}
        if (cur_node->_keys[i].first == key){
            break;
        }
        st.push(std::pair<btree_node**, size_t>(&cur_node, prev_i));
        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    B_tree<tkey, tvalue, compare, t>::btree_node* parent = *st.top().first;

    //проверяем, является ли листом
    bool is_leaf = true;
    bool i_pointer_is_null = true;
    if (cur_node->_pointers.size() > i){
        is_leaf = cur_node->_pointers[i] == nullptr;
        i_pointer_is_null = cur_node->_pointers[i] == nullptr;
        if (cur_node->_pointers.size() > i + 1){
            is_leaf = cur_node->_pointers[i + 1] == nullptr;
        }
    }

    //в зависимости от того лист или нет обрабатываем удаление
    tree_data_type* new_data;
    if (is_leaf){
       delete_key_from_leaf(i, cur_node, st);
    } else {
        size_t index;

        B_tree<tkey, tvalue, compare, t>::btree_node* n;
        if (!i_pointer_is_null){
            n = cur_node->_pointers[i];
            while(n->_pointers.size() > 0 && n->_pointers[n->_pointers.size() - 1] != nullptr){
                n = n->_pointers[n->_pointers.size() - 1];
            }
            new_data = &n->_keys[n->_keys.size() - 1];
            index = n->_keys.size() - 1;
        } else {
            n = cur_node->_pointers[i + 1];
            while(n->_pointers.size() > 0 && n->_pointers[0] != nullptr){
                n = n->_pointers[0];
            }
            new_data = &n->_keys[0];
            index = 0;
        }

        cur_node->_keys[i] = std::move(*new_data);

        delete_key_from_leaf(index, n,st);
    }
    return B_tree<tkey, tvalue, compare, t>::btree_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::delete_key_from_leaf(size_t index, B_tree::btree_node * node,
                                                            std::stack<std::pair<btree_node**, size_t>> st) {
    if (node->_keys.size() > minimum_keys_in_node){
        node->_keys.erase(node->_keys.begin() + index);
        return;
    }

    while(!st.empty()) {
        B_tree::btree_node *cur_node = *st.top().first;
        size_t i = st.top().second; //index of node in parent node
        st.pop();
        B_tree::btree_node *parent = st.empty() ? nullptr : *st.top().first;

        if (parent == nullptr) {
            if (cur_node->_keys.size() > 1) {
                cur_node->_keys.erase(cur_node->_keys.begin() + index);
            } else {
                if (cur_node->_pointers.size() > 0) {
                    if (cur_node->_pointers[0] != nullptr) {
                        _root = cur_node->_pointers[0];
                    } else {
                        if (cur_node->_pointers.size() > 1) {
                            _root = cur_node->_pointers[1];
                        } else {
                            _root = nullptr;
                        }
                    }
                }
            }
            return;
        }

        size_t size_left_sibling = 0;
        size_t size_right_sibling = 0;

        if (i > 0 && parent->_pointers[i - 1] != nullptr) {
            size_left_sibling = parent->_pointers[i - 1]->_keys.size();
        }

        if (i < parent->_pointers.size() && parent->_pointers[i + 1] != nullptr) {
            size_right_sibling = parent->_pointers[i + 1]->_keys.size();
        }

        if (size_left_sibling > minimum_keys_in_node) {
            node->_keys[index] = parent->_keys[i];
            parent->_keys[i] = parent->_pointers[i - 1]->_keys[size_left_sibling - 1];
        } else if (size_right_sibling > minimum_keys_in_node) {
            node->_keys[index] = parent->_keys[i];
            parent->_keys[i] = parent->_pointers[i + 1]->_keys[0];
        } else {
            if (size_left_sibling > 0) {
                merge(parent->_pointers[i - 1], cur_node, parent, i - 1);
            } else if (size_right_sibling > 0) {
                merge(cur_node, parent->_pointers[i + 1], parent, i);
            } else {
                //should never happen
                throw std::logic_error("erase error");
            }
        }
        index = i;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::merge(B_tree::btree_node * left, B_tree::btree_node * right, B_tree::btree_node * parent, size_t split_key_index) {
    B_tree::btree_node *  new_node = _allocator.template new_object<B_tree::btree_node>();
    new_node->_keys = left->_keys;
    new_node->_pointers = left->_pointers;
    new_node->_keys.push_back(parent->_keys[split_key_index]);
    new_node->_pointers.push_back(parent->_pointers[split_key_index]);
    new_node->_keys.insert(new_node->_keys.end(), right->_keys.begin(), right->_keys.end());
    new_node->_pointers.insert(new_node->_pointers.end(), right->_pointers.begin(), right->_pointers.end());
    parent->_keys[split_key_index] = new_node->_keys.back();
    parent->_pointers[split_key_index] = new_node;
    parent->_pointers[split_key_index + 1] = nullptr;
    _allocator.delete_object(left);
    _allocator.delete_object(right);
}

// endregion modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>\n"
                          "bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,\n"
                          "const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey &lhs, const tkey &rhs)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t >\n"
                          "bool compare_keys(const tkey &lhs, const tkey &rhs)", "your code should be here...");
}


#endif