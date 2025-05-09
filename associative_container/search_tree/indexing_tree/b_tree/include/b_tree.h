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
    static void go_to_next(std::stack<std::pair<btree_node*, size_t>>&, size_t&);
    static void go_to_next(std::stack<std::pair<btree_node const*, size_t>>&, size_t&);

    static void go_to_previous(std::stack<std::pair<btree_node*, size_t>>&, size_t&);
    static void go_to_previous(std::stack<std::pair<btree_node const*, size_t>>&, size_t&);

    class btree_iterator final
    {
        //Путь хранится в следующем формате:
        //<указатель на ноду, номер этой ноды в _pointers ее родителя>
        //у корня номер = 0
        //index таким образом показывает номер ноды в _path.top().first->_pointers
        //то есть например если путь выглядит следующим образом:
        //root->_pointers[i1]->_pointers[i2]->_keys[ind]
        //то стек должен выглядеть следующим образом:
        //<root, 0>
        //<root->_pointers[i1], i1>
        //<root->_pointers[i1]->_pointers[i2], i2>
        //и _index = ind
        std::stack<std::pair<btree_node*, size_t>> _path;
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

        explicit btree_iterator(const std::stack<std::pair<btree_node*, size_t>>& path = std::stack<std::pair<btree_node*, size_t>>(), size_t index = 0);

    };

    class btree_const_iterator final
    {
        std::stack<std::pair<btree_node const*, size_t>> _path;
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

        explicit btree_const_iterator(const std::stack<std::pair<btree_node const*, size_t>>& path = std::stack<std::pair<btree_node* const*, size_t>>(), size_t index = 0);
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

    void delete_key_from_leaf(size_t, B_tree::btree_node*, std::stack<std::pair<btree_node*, size_t>>);
    void split(btree_node*, btree_node*);
    void merge(B_tree::btree_node*, B_tree::btree_node*, B_tree::btree_node*, size_t);
    static bool check_if_leaf(B_tree::btree_node*, size_t);

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
        insert(p);
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
    std::vector<tree_data_type> data;
    for(auto it = begin(); it != end(); it++){
        insert(*it);
    }
    _size = other._size;
    _allocator = other._allocator;
    _logger = other._logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(const B_tree& other)
{
    clear();
    std::vector<tree_data_type> data;
    for(auto it = begin(); it != end(); it++){
        insert(*it);
    }
    _size = other._size;
    _allocator = other._allocator;
    _logger = other._logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::B_tree(B_tree&& other) noexcept
{
    _root = other._root;
    _size = other._size;
    _allocator = other._allocator;
    _logger = other._logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>& B_tree<tkey, tvalue, compare, t>::operator=(B_tree&& other) noexcept
{
    clear();
    _root = other._root;
    _size = other._size;
    _allocator = other._allocator;
    _logger = other._logger;
}

// endregion five implementation

// region iterators implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::go_to_previous(std::stack<std::pair<btree_node *, size_t>> & _path, size_t & _index) {
    auto* cur_node = const_cast<B_tree::btree_node*>(_path.top().first);
    if (cur_node->_pointers.size() > _index && cur_node->_pointers[_index] != nullptr){
        //переходим в предыдущее поддерево
        _path.push(std::pair(cur_node->_pointers[_index], _index));
        _index = cur_node->_pointers[_index]->_pointers.size() - 1;
    } else if (_index > 0){
        //переходим к предыдущему ключу в листе
        _index--;
    } else {
        //поднимаемся наверх, так как мы находимся в начале ноды и предыдущий элемент обхода - ее родитель
        _index = _path.top().second;
        _path.pop();
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::go_to_next(std::stack<std::pair<btree_node *, size_t>> & _path, size_t & _index) {
    if (_path.size() == 0){
        return;
    }
    B_tree::btree_node *cur_node = _path.top().first;

    if (cur_node->_pointers.size() > _index + 1 && cur_node->_pointers[_index + 1] != nullptr){
        //переходим в следующее поддерево
        _path.push(std::pair(cur_node->_pointers[_index + 1], _index + 1));
        _index = 0;
    } else if (cur_node->_keys.size() > _index + 1){
        //переходим к следующему ключу в листе
        _index++;
    } else {
        //закончили обход ноды, поднимаемся наверх, пока не попадем в ноду, которую обошли не до конца
        while(_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            cur_node = _path.top().first;
            if (cur_node->_keys.size() > _index){
                return;
            }
        }
        //если мы попали сюда, то значит мы обошли все ноды и сейчас находимся в корне
        _index = _path.top().first->_keys.size(); // делаем индекс таким, чтобы итератор указывал за последний элемент
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::go_to_previous(std::stack<std::pair<btree_node const*, size_t>> & _path, size_t & _index) {
    //same as non const version
    auto* cur_node = const_cast<B_tree::btree_node*>(_path.top().first);
    if (cur_node->_pointers.size() > _index && cur_node->_pointers[_index] != nullptr){
        _path.push(std::pair(cur_node->_pointers[_index], _index));
        _index = cur_node->_pointers[_index]->_pointers.size() - 1;
    } else if (_index > 0){
        _index--;
    } else {
        _index = _path.top().second;
        _path.pop();
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::go_to_next(std::stack<std::pair<btree_node const*, size_t>> & _path, size_t & _index) {
    //same as non const version
    if (_path.size() == 0){
        return;
    }
    B_tree::btree_node const* cur_node = _path.top().first;

    if (cur_node->_pointers.size() > _index + 1 && cur_node->_pointers[_index + 1] != nullptr){
        _path.push(std::pair(cur_node->_pointers[_index + 1], _index + 1));
        _index = 0;
    } else if (cur_node->_keys.size() > _index + 1){
        _index++;
    } else {
        while(_path.size() > 1) {
            _index = _path.top().second;
            _path.pop();
            cur_node = _path.top().first;
            if (cur_node->_keys.size() > _index){
                return;
            }
        }
        _index = _path.top().first->_keys.size(); // делаем индекс таким, чтобы итератор указывал за последний элемент
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_iterator::btree_iterator(
        const std::stack<std::pair<btree_node*, size_t>>& path, size_t index)
{
    _path = path;
    _index = index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator*() const noexcept
{
    return reinterpret_cast<reference>(_path.top().first->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator->() const noexcept
{
    return reinterpret_cast<pointer>(&(_path.top().first->_keys[_index]));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++()
{
    go_to_next(_path, _index);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator++(int)
{
    auto copy = *this;
    ++(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator&
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--()
{
    go_to_previous(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::btree_iterator::operator--(int)
{
    auto copy = *this;
    --(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::operator!=(const self& other) const noexcept
{
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::current_node_keys_count() const noexcept
{
    return _path.top().first->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_iterator::is_terminate_node() const noexcept
{
    auto cur_node = _path.top().first;
    return check_if_leaf(cur_node, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const std::stack<std::pair<btree_node const*, size_t>>& path, size_t index) : _path(path), _index(index)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::btree_const_iterator(
        const btree_iterator& it) noexcept : _path(it._path), _index(it._index)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator*() const noexcept
{
    return reinterpret_cast<reference>(_path.top().first->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator->() const noexcept
{
    return reinterpret_cast<pointer>(&(_path.top().first->_keys[_index]));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++()
{
    go_to_next(_path, _index);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator++(int)
{
    auto copy = *this;
    ++(*this);
    return copy;
}

//Не покрывается тестами, лучше не использовать
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--()
{
    go_to_prev(const_cast<std::stack<std::pair<btree_node*, size_t>>&>(_path), _index);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator--(int)
{
    auto copy = *this;
    --(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::operator!=(const self& other) const noexcept
{
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::current_node_keys_count() const noexcept
{
    return _path.top().first->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_iterator::is_terminate_node() const noexcept
{
    auto cur_node = _path.top().first;
    return check_if_leaf(cur_node, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const std::stack<std::pair<btree_node**, size_t>>& path, size_t index) : _path(path), _index(index)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::btree_reverse_iterator(
        const btree_iterator& it) noexcept : _path(it._path), _index(it._index)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_iterator() const noexcept
{
    return btree_iterator(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator*() const noexcept
{
    return reinterpret_cast<reference>(_path.top().first->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator->() const noexcept
{
    return reinterpret_cast<pointer>(&(_path.top().first->_keys[_index]));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++()
{
    go_to_prev(_path, _index);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator++(int)
{
    auto copy = *this;
    ++(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--()
{
    go_to_next(_path, _index);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator--(int)
{
    auto copy = *this;
    --(*this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::current_node_keys_count() const noexcept
{
    return _path.top().first->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::is_terminate_node() const noexcept
{
    auto cur_node = _path.top().first;
    return check_if_leaf(cur_node, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const std::stack<std::pair<btree_node* const*, size_t>>& path, size_t index) : _path(path), _index(index)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::btree_const_reverse_iterator(
        const btree_reverse_iterator& it) noexcept : _path(it.path), _index(it.index)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator B_tree<tkey, tvalue, compare, t>::btree_const_iterator() const noexcept
{
    return btree_const_iterator(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::reference
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator*() const noexcept
{
    return reinterpret_cast<reference>(_path.top().first->_keys[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::pointer
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator->() const noexcept
{
    return reinterpret_cast<pointer>(&(_path.top().first->_keys[_index]));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++()
{
    go_to_prev(const_cast<std::stack<std::pair<btree_node*, size_t>>&>(_path), _index);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator++(int)
{
    auto copy = *this;
    ++(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator&
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--()
{
    go_to_next(_path, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator
B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator--(int)
{
    auto copy = *this;
    --(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator==(const self& other) const noexcept
{
    return _path == other._path && _index == other._index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::operator!=(const self& other) const noexcept
{
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::depth() const noexcept
{
    return _path.size() - 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::current_node_keys_count() const noexcept
{
    return _path.top().first->_keys.size();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::is_terminate_node() const noexcept
{
    auto cur_node = _path.top().first;
    return check_if_leaf(cur_node, _index);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator::index() const noexcept
{
    return _index;
}

// endregion iterators implementation

// region element access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key)
{
    auto it = find(key);
    if (it == end()){
        throw std::out_of_range("incorrect key for at");
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue& B_tree<tkey, tvalue, compare, t>::at(const tkey& key) const
{
    auto it = find(key);
    if (it._path.empty()){
        throw std::out_of_range("incorrect key for at");
    }
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](const tkey& key)
{
    auto it = find(key);
    return it->second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue& B_tree<tkey, tvalue, compare, t>::operator[](tkey&& key)
{
    auto it = find(key);
    return it->second;
}

// endregion element access implementation

// region iterator begins implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::begin()
{
    //наименьшее значение, то есть самое левое
    if (_root == nullptr){
        return btree_iterator(std::stack<std::pair<btree_node*, size_t>>(),  0);
    }
    B_tree::btree_node * cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st;
    st.push(std::pair(_root, 0));
    while(cur_node->_pointers.size() > 0 && cur_node->_pointers[0] != nullptr){
        cur_node = cur_node->_pointers[0];
        st.push(std::pair(cur_node, 0));
    }

    return btree_iterator(st, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::end()
{
    //наибольшее значение, то есть самое правое
    if (_root == nullptr){
        return btree_iterator(std::stack<std::pair<btree_node*, size_t>>(),  0);
    }
    B_tree::btree_node * cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st;
    st.push(std::pair(_root, 0));
    return btree_iterator(st, _root->_keys.size());
}
//Дальнейшие функции являются копипастой двух предыдущих, отличаясь только константностью или возвращаемым типом
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::begin() const
{
    if (_root == nullptr){
        return btree_const_iterator(std::stack<std::pair<btree_node const*, size_t>>(),  0);
    }
    B_tree::btree_node * cur_node = _root;
    std::stack<std::pair<btree_node const*, size_t>> st;
    st.push(std::pair(_root, 0));
    while(cur_node->_pointers.size() > 0 && cur_node->_pointers[0] != nullptr){
        cur_node = cur_node->_pointers[0];
        st.push(std::pair(cur_node, 0));
    }

    return btree_const_iterator(st, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::end() const
{
    if (_root == nullptr){
        return btree_const_iterator(std::stack<std::pair<btree_node const*, size_t>>(),  0);
    }
    B_tree::btree_node * cur_node = _root;
    std::stack<std::pair<btree_node const*, size_t>> st;
    st.push(std::pair(_root, 0));
    size_t ind = 0;
    while(cur_node->_pointers.size() > 0 && cur_node->_pointers[cur_node->_pointers.size() - 1] != nullptr){
        ind = cur_node->_pointers.size() - 1;
        cur_node = cur_node->_pointers[ind];
        st.push(std::pair(cur_node, ind));
    }
    return btree_const_iterator(st, cur_node->_pointers.size());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cbegin() const
{
    if (_root == nullptr){
        return btree_const_iterator(std::stack<std::pair<btree_node const*, size_t>>(),  0);
    }
    B_tree::btree_node * cur_node = _root;
    std::stack<std::pair<btree_node const*, size_t>> st;
    st.push(std::pair(_root, 0));
    while(cur_node->_pointers.size() > 0 && cur_node->_pointers[0] != nullptr){
        cur_node = cur_node->_pointers[0];
        st.push(std::pair(cur_node, 0));
    }

    return btree_const_iterator(st, 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::cend() const
{
    if (_root == nullptr){
        return btree_const_iterator(std::stack<std::pair<btree_node const*, size_t>>(),  0);
    }
    B_tree::btree_node * cur_node = _root;
    std::stack<std::pair<btree_node const*, size_t>> st;
    st.push(std::pair(_root, 0));
    size_t ind = 0;
    while(cur_node->_pointers.size() > 0 && cur_node->_pointers[cur_node->_pointers.size() - 1] != nullptr){
        ind = cur_node->_pointers.size() - 1;
        cur_node = cur_node->_pointers[ind];
        st.push(std::pair(cur_node, ind));
    }
    return btree_const_iterator(st, cur_node->_pointers.size());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin()
{
    return std::prev(end());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend()
{
    return std::prev(begin());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rbegin() const
{
    return std::prev(end());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::rend() const
{
    return std::prev(begin());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crbegin() const
{
    return std::prev(cend());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_reverse_iterator B_tree<tkey, tvalue, compare, t>::crend() const
{
    return std::prev(cbegin());
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
    size_t prev_i = 0;
    auto cur_node = _root;
    auto cur_key = _root->_keys[i];
    std::stack<std::pair<btree_node*, size_t>> st = std::stack<std::pair<btree_node*, size_t>>();
    st.push(std::pair<btree_node*, size_t>(_root, 0));
    while(cur_node != nullptr){
        i = 0;
        for(; i < cur_node->_keys.size() && compare_keys(cur_node->_keys[i].first, key); i++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_i));
        if (i < cur_node->_keys.size() && cur_node->_keys[i].first == key){
            return btree_iterator(st, i);
        }

        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = _root;
    auto cur_key = _root->_keys[i];
    std::stack<std::pair<btree_node**, size_t>> st = std::stack<std::pair<btree_node**, size_t>>();
    st.push(std::pair<btree_node**, size_t>(&_root, 0));
    while(cur_node != nullptr){
        i = 0;
        for(; i < cur_node->_keys.size() && compare_keys(cur_node->_keys[i].first, key); i++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_i));
        if (i < cur_node->_keys.size() && cur_node->_keys[i].first == key){
            break;
        }

        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    return btree_iterator(st, i);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st = std::stack<std::pair<btree_node*, size_t>>();
    st.push(std::pair<btree_node*, size_t>(_root, 0));
    while(cur_node != nullptr){
        i = 0;
        for(; i < cur_node->_keys.size() && compare_keys(cur_node->_keys[i].first, key); i++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_i));
        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    if (st.size() == 1 && i == st.top().first->_keys.size()){
        return end();
    } else {
        //сейчас в st хранится путь до nullptr. Удаляем последний элемент, чтобы путь был до ноды, а не до nullptr
        st.pop();
        return btree_iterator(st, prev_i);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st = std::stack<std::pair<btree_node*, size_t>>();
    st.push(std::pair<btree_node*, size_t>(_root, 0));
    while(cur_node != nullptr){
        i = 0;
        for(; i < cur_node->_keys.size() && compare_keys(cur_node->_keys[i].first, key); i++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_i));
        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    if (st.size() == 1 && i == st.top().first->_keys.size()){
        return cend();
    } else {
        //сейчас в st хранится путь до nullptr. Удаляем последний элемент, чтобы путь был до ноды, а не до nullptr
        st.pop();
        return btree_const_iterator(st, prev_i);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st = std::stack<std::pair<btree_node*, size_t>>();
    st.push(std::pair<btree_node*, size_t>(_root, 0));
    while(cur_node != nullptr){
        i = 0;
        for(; i < cur_node->_keys.size() && !compare_keys(key, cur_node->_keys[i].first); i++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_i));
        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    if (st.size() == 1 && i == st.top().first->_keys.size()){
        return end();
    } else {
        //сейчас в st хранится путь до nullptr. Удаляем последний элемент, чтобы путь был до ноды, а не до nullptr
        st.pop();
        return btree_iterator(st, prev_i);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_const_iterator B_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st = std::stack<std::pair<btree_node*, size_t>>();
    st.push(std::pair<btree_node*, size_t>(_root, 0));
    while(cur_node != nullptr){
        i = 0;
        for(; i < cur_node->_keys.size() && !compare_keys(key, cur_node->_keys[i].first); i++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_i));
        cur_node = cur_node->_pointers[i];
        prev_i = i;
    }
    if (st.size() == 1 && i == st.top().first->_keys.size()){
        return end();
    } else {
        //сейчас в st хранится путь до nullptr. Удаляем последний элемент, чтобы путь был до ноды, а не до nullptr
        st.pop();
        return btree_const_iterator(st, prev_i);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    auto it = find(key);
    return it != end();
}

// endregion lookup implementation

// region modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    std::vector<tkey> keys;
    auto it = begin();
    while(it != end()){
        keys.push_back(it->first);
        it++;
    }

    for(auto key : keys){
        erase(key);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::split(B_tree::btree_node * node_to_split, B_tree::btree_node * parent_node) {
    size_t middle_ind = (maximum_keys_in_node+1) / 2;
    tkey middle_key = node_to_split->_keys[middle_ind].first;
    size_t split_index_in_parent;
    if (parent_node == nullptr) {
        //если сплитим корень, то нужно создать новый
        _root = _allocator.template new_object<B_tree::btree_node>();
        parent_node = _root;
        split_index_in_parent = 0;
    } else {
        size_t i;
        for(i = 0; i < parent_node->_keys.size(); i++){
            if (!compare_keys(parent_node->_keys[i].first, middle_key)){
                break;
            }
        }
        split_index_in_parent = i;
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
    }

    const auto it_keys_begin = node_to_split->_keys.begin();
    const auto it_keys_end = node_to_split->_keys.end();
    const auto it_point_begin = node_to_split->_pointers.begin();
    const auto it_point_end = node_to_split->_pointers.end();

    left_part->_keys.insert(left_part->_keys.begin(), it_keys_begin, it_keys_begin + middle_ind);
    left_part->_pointers.insert(left_part->_pointers.begin(), it_point_begin, it_point_begin + middle_ind);

    right_part->_keys.insert(right_part->_keys.begin(), it_keys_begin + middle_ind + 1, it_keys_end);
    right_part->_pointers.insert(right_part->_pointers.begin(), it_point_begin + middle_ind + 1, it_point_end);

    parent_node->_keys.insert(parent_node->_keys.begin() + split_index_in_parent, node_to_split->_keys[middle_ind]);
    parent_node->_pointers.insert(parent_node->_pointers.begin() + split_index_in_parent, left_part);
    parent_node->_pointers.insert(parent_node->_pointers.begin() + split_index_in_parent + 1, right_part);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>
B_tree<tkey, tvalue, compare, t>::insert(const tree_data_type& data)
{
    size_t index_of_node_in_parent_node = 0;
    B_tree<tkey, tvalue, compare, t>::btree_node *node = nullptr;
    B_tree<tkey, tvalue, compare, t>::btree_node *parent_node = nullptr;
    auto cur_pointer = _root;
    std::stack<std::pair<btree_node*, size_t>> path = std::stack<std::pair<btree_node*, size_t>>();

    if (_root == nullptr){
        _root = _allocator.template new_object<B_tree::btree_node>();
        _root->_keys.insert(_root->_keys.begin(), data);
        _root->_pointers.insert(_root->_pointers.begin(), nullptr);
    } else {
        size_t i = 0;
        while(cur_pointer != nullptr){
            i = 0;
            index_of_node_in_parent_node = 0;
            for(; i < cur_pointer->_keys.size() &&
                    compare_keys(cur_pointer->_keys[i].first, data.first); index_of_node_in_parent_node = i++){}

            auto cur_key = cur_pointer->_keys[index_of_node_in_parent_node].first;

            if (cur_key == data.first){
                // уже есть в дереве
                return std::pair<B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>(end(), false);
            }

            parent_node = node;
            node = cur_pointer;

            if (i < cur_pointer->_pointers.size()){
                cur_pointer = cur_pointer->_pointers[i];
            } else {
                cur_pointer = nullptr;
            }
            path.push(std::pair<btree_node*, size_t>(node, index_of_node_in_parent_node));
        }
        if (node->_keys.size() == maximum_keys_in_node){
            node->_keys.insert(node->_keys.begin() + i, data);
            node->_pointers.insert(node->_pointers.begin() + i, nullptr);
            split(node, parent_node);
        } else {
            node->_keys.insert(node->_keys.begin() + i, data);
            node->_pointers.insert(node->_pointers.begin() + i, nullptr);
        }
    }
    _size++;
    return std::pair<B_tree<tkey, tvalue, compare, t>::btree_iterator, bool>(B_tree<tkey, tvalue, compare, t>::btree_iterator(path, index_of_node_in_parent_node), true);
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
    auto it = find(data.first);
    if (it != end()){
        it->second = data.second;
    } else {
        insert(data);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    auto it = find(data.first);
    if (it != end()){
        it->second = data.second;
    } else {
        insert(std::forward<tree_data_type&&>(data));
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<typename... Args>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    tree_data_type data(args ...);
    insert_or_assign(std::move(data));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator pos)
{
    return erase((*pos).first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator pos)
{
    return erase((*pos).first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_iterator beg, btree_iterator en)
{
    // В теории erase возвращает указатель на следующий элемент, поэтому его можно использовать подобным образом.
    // На практике... эээ... да...
    auto it = beg;
    while(it != en){
        it = erase(it);
    }
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(btree_const_iterator beg, btree_const_iterator en)
{
    // В теории erase возвращает указатель на следующий элемент, поэтому его можно использовать подобным образом.
    // На практике... эээ... да...
    auto it = beg;
    while(it != en){
        it = erase(it);
    }
    return it;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename B_tree<tkey, tvalue, compare, t>::btree_iterator
B_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    size_t ind = 0;
    size_t prev_index = 0;
    auto cur_node = _root;
    std::stack<std::pair<btree_node*, size_t>> st = std::stack<std::pair<btree_node*, size_t>>();
    B_tree<tkey, tvalue, compare, t>::btree_node* parent = nullptr;
    //находим нужный ключ
    while(cur_node != nullptr){
        ind = 0;
        for(; ind < cur_node->_keys.size() && compare_keys(cur_node->_keys[ind].first, key); ind++){}
        st.push(std::pair<btree_node*, size_t>(cur_node, prev_index));
        if (ind < cur_node->_keys.size() && cur_node->_keys[ind].first == key){
            break;
        }

        parent = cur_node;
        if (ind >= cur_node->_pointers.size()){
            return end();
        }
        cur_node = cur_node->_pointers[ind];
        prev_index = ind;
    }

    if (cur_node == nullptr){
        return end();
    }

    btree_iterator next(st, ind);
    next++;

    //проверяем, является ли листом
    bool is_leaf = check_if_leaf(cur_node, ind);

    //в зависимости от того лист или нет обрабатываем удаление
    tree_data_type* new_data;
    if (is_leaf){
       delete_key_from_leaf(ind, cur_node, st);
    } else {
        size_t index;
        B_tree<tkey, tvalue, compare, t>::btree_node* n;
        // Заменяем самым правым элементом из левого поддерева
        // ВРОДЕ БЫ если элемент не лист, то левое поддерево у него должно быть всегда
        n = cur_node->_pointers[index];
        st.push(std::pair(n, index));
        while(n->_pointers.size() > 0 && n->_pointers[n->_pointers.size() - 1] != nullptr){
            size_t tmp = n->_pointers.size() - 1;
            n = n->_pointers[n->_pointers.size() - 1];
            st.push(std::pair(n, tmp));
        }
        new_data = &n->_keys[n->_keys.size() - 1];
        index = n->_keys.size() - 1;

        cur_node->_keys[index] = std::move(*new_data);
        delete_key_from_leaf(index, n,st);
    }
    return next;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::delete_key_from_leaf(size_t index_of_key, B_tree::btree_node * node,
                                                            std::stack<std::pair<btree_node*, size_t>> st) {
    if (node->_keys.size() > minimum_keys_in_node){
        node->_keys.erase(node->_keys.begin() + index_of_key);
        return;
    }
    B_tree::btree_node *cur_node = st.top().first;
    size_t i = st.top().second;
    st.pop();
    while(cur_node != nullptr) {
        B_tree::btree_node *parent = st.empty() ? nullptr : st.top().first;
        if (parent == nullptr) {
            if (cur_node->_keys.size() > 1) {
                // в корне может быть < mininmum_keys_in_node ключей, спокойно удаляем
                cur_node->_keys.erase(cur_node->_keys.begin() + index_of_key);
            } else {
                // в корне один элемент, удаляем его и делаем корнем одного из его потомков
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
            node->_keys[index_of_key] = parent->_keys[i];
            parent->_keys[i] = parent->_pointers[i - 1]->_keys[size_left_sibling - 1];
        } else if (size_right_sibling > minimum_keys_in_node) {
            node->_keys[index_of_key] = parent->_keys[i];
            parent->_keys[i] = parent->_pointers[i + 1]->_keys[0];
        } else {
            //если у обоих братьев мало ключей, то просто сливаемся с одним из них
            node->_keys.erase(node->_keys.begin() + index_of_key);
            if (size_left_sibling > 0) {
                merge(parent->_pointers[i - 1], cur_node, parent, i - 1);
            } else if (size_right_sibling > 0) {
                merge(cur_node, parent->_pointers[i + 1], parent, i);
            }
        }
        index_of_key = i;
        cur_node = st.empty() ? nullptr : st.top().first;
        i = cur_node ? st.top().second : 0;
        st.pop();
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void B_tree<tkey, tvalue, compare, t>::merge(B_tree::btree_node * left, B_tree::btree_node * right, B_tree::btree_node * parent, size_t split_key_index) {
    B_tree::btree_node *  new_node = _allocator.template new_object<B_tree::btree_node>();
    new_node->_keys = left->_keys;
    new_node->_pointers = left->_pointers;
    new_node->_keys.push_back(parent->_keys[split_key_index]);
    new_node->_pointers.push_back(parent->_pointers[split_key_index]);
    new_node->_keys.insert(new_node->_keys.end(), right->_keys.begin(), right->_keys.end() - 1);
    new_node->_pointers.insert(new_node->_pointers.end(), right->_pointers.begin(), right->_pointers.end() - 1);
    parent->_keys[split_key_index] = new_node->_keys[new_node->_keys.size() - 1];
    parent->_pointers[split_key_index] = new_node;
    _allocator.delete_object(left);
    _allocator.delete_object(right);
}

//Проверяет, есть ли у ключа правый или левый потомок
template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool B_tree<tkey, tvalue, compare, t>::check_if_leaf(B_tree::btree_node * cur_node, size_t key_index) {
    bool is_leaf = true;
    if (cur_node->_pointers.size() > key_index){
        is_leaf = cur_node->_pointers[key_index] == nullptr;
        if (cur_node->_pointers.size() > key_index + 1){
            is_leaf = cur_node->_pointers[key_index + 1] == nullptr;
        }
    }
    return is_leaf;
}


// endregion modifiers implementation

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_pairs(const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &lhs,
                   const typename B_tree<tkey, tvalue, compare, t>::tree_data_type &rhs)
{
    return compare::operator()(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool compare_keys(const tkey &lhs, const tkey &rhs)
{
    return compare::operator()(lhs, rhs);
}


#endif