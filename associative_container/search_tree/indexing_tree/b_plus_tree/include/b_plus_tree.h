#include <iterator>
#include <utility>
#include <vector>
#include <boost/container/static_vector.hpp>
#include <concepts>
#include <stack>
#include <pp_allocator.h>
#include <search_tree.h>
#include <initializer_list>
#include <logger_guardant.h>

#ifndef MP_OS_B_PLUS_TREE_H
#define MP_OS_B_PLUS_TREE_H

template <typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5>
class BP_tree final : private logger_guardant, private compare
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

    struct bptree_node_base
    {
        bool _is_terminate;

        bptree_node_base() noexcept;
        virtual ~bptree_node_base() =default;
        size_t size();
    };

    struct bptree_node_term : public bptree_node_base
    {
        bptree_node_term* _next;
        boost::container::static_vector<tree_data_type, maximum_keys_in_node + 1> _data;
        bptree_node_term() noexcept;
    };

    struct bptree_node_middle : public bptree_node_base
    {
        boost::container::static_vector<tkey, maximum_keys_in_node + 1> _keys;
        boost::container::static_vector<bptree_node_base*, maximum_keys_in_node + 2> _pointers;
        bptree_node_middle() noexcept;
    };

    pp_allocator<value_type> _allocator;
    logger* _logger;
    bptree_node_base* _root;
    size_t _size;

    logger* get_logger() const noexcept override;
    pp_allocator<value_type> get_allocator() const noexcept;

public:

    // region constructors declaration

    explicit BP_tree(const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    explicit BP_tree(pp_allocator<value_type> alloc, const compare& comp = compare(), logger *logger = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit BP_tree(iterator begin, iterator end, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(), pp_allocator<value_type> = pp_allocator<value_type>(), logger* logger = nullptr);

    // endregion constructors declaration

    // region five declaration

    BP_tree(const BP_tree& other);

    BP_tree(BP_tree&& other) noexcept;

    BP_tree& operator=(const BP_tree& other);

    BP_tree& operator=(BP_tree&& other) noexcept;

    ~BP_tree() noexcept override;

    // endregion five declaration

    // region iterators declaration

    class bptree_iterator;
    class bptree_const_iterator;

    class bptree_iterator final
    {
        bptree_node_term* _node;
        size_t _index;

    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = bptree_iterator;

        friend class BP_tree;
        friend class bptree_const_iterator;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t current_node_keys_count() const noexcept;
        size_t index() const noexcept;

        explicit bptree_iterator(bptree_node_term* node = nullptr, size_t index = 0);

    };

    class bptree_const_iterator final
    {
        const bptree_node_term* _node;
        size_t _index;
        bool _is_first_node;

    public:

        using value_type = tree_data_type_const;
        using reference = const value_type&;
        using pointer = const value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;
        using self = bptree_const_iterator;

        friend class BP_tree;
        friend class bptree_iterator;

        bptree_const_iterator(const bptree_iterator& it) noexcept;

        reference operator*() const noexcept;
        pointer operator->() const noexcept;

        self& operator++();
        self operator++(int);

        bool operator==(const self& other) const noexcept;
        bool operator!=(const self& other) const noexcept;

        size_t current_node_keys_count() const noexcept;
        size_t index() const noexcept;

        explicit bptree_const_iterator(const bptree_node_term* node = nullptr, size_t index = 0);
    };

    friend class btree_iterator;
    friend class btree_const_iterator;

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

    bptree_iterator begin();
    bptree_iterator end();

    bptree_const_iterator begin() const;
    bptree_const_iterator end() const;

    bptree_const_iterator cbegin() const;
    bptree_const_iterator cend() const;

    // endregion iterator begins declaration

    // region lookup declaration

    size_t size() const noexcept;
    bool empty() const noexcept;

    /*
     * Returns end() if not exist
     */

    bptree_iterator find(const tkey& key);
    bptree_const_iterator find(const tkey& key) const;

    bptree_iterator lower_bound(const tkey& key);
    bptree_const_iterator lower_bound(const tkey& key) const;

    bptree_iterator upper_bound(const tkey& key);
    bptree_const_iterator upper_bound(const tkey& key) const;

    bool contains(const tkey& key) const;

    // endregion lookup declaration

    // region modifiers declaration

    void clear() noexcept;

    /*
     * Does nothing if key exists, delegates to emplace.
     * Second return value is true, when inserted
     */
    std::pair<bptree_iterator, bool> insert(const tree_data_type& data);
    std::pair<bptree_iterator, bool> insert(tree_data_type&& data);

    void split(bptree_node_base*, bptree_node_middle*);

    template <typename ...Args>
    std::pair<bptree_iterator, bool> emplace(Args&&... args);

    /*
     * Updates value if key exists, delegates to emplace.
     */
    bptree_iterator insert_or_assign(const tree_data_type& data);
    bptree_iterator insert_or_assign(tree_data_type&& data);

    template <typename ...Args>
    bptree_iterator emplace_or_assign(Args&&... args);

    /*
     * Return iterator to node next ro removed or end() if key not exists
     */
    bptree_iterator erase(bptree_iterator pos);
    bptree_iterator erase(bptree_const_iterator pos);

    bptree_iterator erase(bptree_iterator beg, bptree_iterator en);
    bptree_iterator erase(bptree_const_iterator beg, bptree_const_iterator en);


    bptree_iterator erase(const tkey& key);
    void delete_key_from_leaf(size_t, bptree_node_term*, std::stack<std::pair<bptree_node_base*, size_t>>);
    void merge(bptree_node_base*, bptree_node_base*, bptree_node_middle*, size_t);
    void merge_terminate(bptree_node_term*, bptree_node_term*, bptree_node_middle*, size_t);
    void merge_middle(bptree_node_middle*, bptree_node_middle*, bptree_node_middle*, size_t);

    // endregion modifiers declaration
};



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_node_base::size() {
   if (_is_terminate){
       return static_cast<bptree_node_term*>(this)->_data.size();
   } else {
       return static_cast<bptree_node_middle*>(this)->_keys.size();
   }
}


template<std::input_iterator iterator, compator<typename std::iterator_traits<iterator>::value_type::first_type> compare = std::less<typename std::iterator_traits<iterator>::value_type::first_type>,
        std::size_t t = 5, typename U>
BP_tree(iterator begin, iterator end, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
        logger *logger = nullptr) -> BP_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 5, typename U>
BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(), pp_allocator<U> = pp_allocator<U>(),
        logger *logger = nullptr) -> BP_tree<tkey, tvalue, compare, t>;

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::compare_pairs(const BP_tree::tree_data_type &lhs,
                                                     const BP_tree::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_node_base::bptree_node_base() noexcept
{

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_node_term::bptree_node_term() noexcept : _next(nullptr), _data()
{
    bptree_node_base::_is_terminate = true;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_node_middle::bptree_node_middle() noexcept : _keys(), _pointers()
{
    bptree_node_base::_is_terminate = true;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
logger * BP_tree<tkey, tvalue, compare, t>::get_logger() const noexcept
{
    return _logger;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
pp_allocator<typename BP_tree<tkey, tvalue, compare, t>::value_type> BP_tree<tkey, tvalue, compare, t>::
get_allocator() const noexcept
{
    return _allocator;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::reference BP_tree<tkey, tvalue, compare, t>::
bptree_iterator::operator*() const noexcept
{
    if (_node == nullptr){
        throw std::runtime_error("incorrect iterator for *");
    }
    return reinterpret_cast<reference>(_node->_data[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::pointer BP_tree<tkey, tvalue, compare, t>::bptree_iterator
::operator->() const noexcept
{
    if (_node == nullptr){
        throw std::runtime_error("incorrect iterator for *");
    }
    return reinterpret_cast<pointer>(&(_node->_data[_index]));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::self & BP_tree<tkey, tvalue, compare, t>::bptree_iterator::
operator++()
{
    ++_index;
    if (_index == _node->_data.size() && _node->_next != nullptr){
        _index = 0;
        _node = _node->_next;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator::self BP_tree<tkey, tvalue, compare, t>::bptree_iterator::
operator++(int)
{
    auto copy = *this;
    ++(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_iterator::operator==(const self &other) const noexcept
{
    return _index == other._index && _node == other._node;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_iterator::operator!=(const self &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_iterator::current_node_keys_count() const noexcept
{
    throw not_implemented("too laazyy", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_iterator::index() const noexcept
{
    return _index;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_iterator::bptree_iterator(bptree_node_term *node, size_t index) : _index(index), _node(node)
{

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::bptree_const_iterator(const bptree_iterator &it) noexcept
{
    throw not_implemented("too laazyy", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::reference BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator*() const noexcept
{
    if (_node == nullptr){
        throw std::runtime_error("incorrect iterator for *");
    }
    return reinterpret_cast<reference>(_node->_data[_index]);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::pointer BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator->() const noexcept
{
    if (_node == nullptr){
        throw std::runtime_error("incorrect iterator for *");
    }
    return reinterpret_cast<pointer>(&(_node->_data[_index]));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::self & BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator++()
{
    ++_index;
    if (_index == _node->_data.size() && _node->_next != nullptr){
        _index = 0;
        _node = _node->_next;
        _is_first_node = false;
    }
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::self BP_tree<tkey, tvalue, compare, t>::
bptree_const_iterator::operator++(int)
{
    auto copy = *this;
    ++(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::operator==(const self &other) const noexcept
{
    return _index == other._index && _node == other._node;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::operator!=(const self &other) const noexcept
{
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::current_node_keys_count() const noexcept
{
    throw not_implemented("too laazyy", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::index() const noexcept {
    return _index;
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator::bptree_const_iterator(const bptree_node_term *node,
                                                                                size_t index) : _index(index),
                                                                                                _node(node), _is_first_node(false)
{

}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue & BP_tree<tkey, tvalue, compare, t>::at(const tkey & key)
{
    auto it = find(key);
    if (it == end()){
        throw std::out_of_range("out of range");
    } else {
        return it->second;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
const tvalue & BP_tree<tkey, tvalue, compare, t>::at(const tkey &) const
{
    throw not_implemented("too laazyy", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue & BP_tree<tkey, tvalue, compare, t>::operator[](const tkey &key)
{
    throw not_implemented("too laazyy", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
tvalue & BP_tree<tkey, tvalue, compare, t>::operator[](tkey &&key)
{
    throw not_implemented("too laazyy", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool> BP_tree<tkey, tvalue, compare, t>::insert(
    const tree_data_type &data)
{
    size_t index_of_node_in_parent_node = 0;

    BP_tree<tkey, tvalue, compare, t>::bptree_node_base *parent_node = nullptr;
    auto cur_pointer = _root;
    std::stack<std::pair<bptree_node_base*, size_t>> path = std::stack<std::pair<bptree_node_base*, size_t>>();
    if (_root == nullptr){
        _root = _allocator.template new_object<BP_tree<tkey, tvalue, compare, t>::bptree_node_term>();
        auto root_casted = static_cast<BP_tree<tkey, tvalue, compare, t>::bptree_node_term*>(_root);
        root_casted->_next = nullptr;
        root_casted->_data.push_back(data);
    } else {
        size_t i = 0;
        while(!cur_pointer->_is_terminate){
            i = 0;
            index_of_node_in_parent_node = 0;
            auto cur_pointer_casted = static_cast<BP_tree<tkey, tvalue, compare, t>::bptree_node_middle*>(cur_pointer);
            for (; i < cur_pointer_casted->_keys.size() &&
                   compare_keys(cur_pointer_casted->_keys[i], data.first); index_of_node_in_parent_node = i++) {}

            auto cur_key = cur_pointer_casted->_keys[index_of_node_in_parent_node];

            if (cur_key == data.first) {
                // уже есть в дереве
                return std::pair<BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool>(end(), false);
            }

            parent_node = cur_pointer;

            if (i < cur_pointer_casted->_pointers.size()) {
                cur_pointer = cur_pointer_casted->_pointers[i];
            } else {
                cur_pointer = nullptr;
            }
            path.push(std::pair(cur_pointer, index_of_node_in_parent_node));
        }

        auto pointer_terminate = static_cast<bptree_node_term*>(cur_pointer);
        for (i = 0; i < pointer_terminate->_data.size() && compare_keys(pointer_terminate->_data[i].first, data.first); i++) {}

        auto node_casted = pointer_terminate;
        if (node_casted->_data.size() == maximum_keys_in_node){
            node_casted->_data.insert(node_casted->_data.begin() + i, data);
            split(cur_pointer, static_cast<bptree_node_middle*>(parent_node));
//            node->_pointers.insert(node->_d.begin() + i, nullptr);
//            while (node != nullptr && node->_data.size() > maximum_keys_in_node) {
//                split(node, parent_node);
//                node = parent_node;
//                parent_node = path.empty() ? nullptr : path.top().first;
//                if (!path.empty()){
//                    path.pop();
//                }
//            }
        } else {
            node_casted->_data.insert(node_casted->_data.begin() + i, data);
//            node->_pointers.insert(node->_pointers.begin() + i, nullptr);
        }
    }
    _size++;
    return std::pair(bptree_iterator(), true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::split(BP_tree::bptree_node_base * node_to_split_base, BP_tree::bptree_node_middle * parent_node) {
    size_t middle_ind = (maximum_keys_in_node+1) / 2;
    auto node_to_split = static_cast<bptree_node_term*>(node_to_split_base);
    tkey middle_key = node_to_split->_data[middle_ind].first;
    size_t split_index_in_parent;
    bool parent_is_null = parent_node == nullptr;
    if (parent_is_null) {
        //если сплитим корень, то нужно создать новый
        _root = _allocator.template new_object<bptree_node_middle>();
        parent_node = static_cast<bptree_node_middle*>(_root);
        _root->_is_terminate = false;
        split_index_in_parent = 0;
    } else {
        size_t i;
        for(i = 0; i < parent_node->_keys.size(); i++){
            if (!compare_keys(parent_node->_keys[i], middle_key)){
                break;
            }
        }
        split_index_in_parent = i;
    }
    bptree_node_term * left_part = _allocator.template new_object<bptree_node_term>();
    bptree_node_term * right_part = _allocator.template new_object<bptree_node_term>();
    if (left_part == nullptr || right_part == nullptr){
        if (left_part != nullptr){
            _allocator.template delete_object<bptree_node_term >(left_part);
        }
        if (right_part != nullptr){
            _allocator.template delete_object<bptree_node_term >(right_part);
        }
    }

    const auto it_keys_begin = node_to_split->_data.begin();
    const auto it_keys_end = node_to_split->_data.end();
//    const auto it_point_begin = node_to_split->_pointers.begin();
//    const auto it_point_end = node_to_split->_pointers.end();

//    left_part->_data.insert(left_part->_data.begin(), it_keys_begin, it_keys_begin + middle_ind);


//    left_part->_pointers.insert(left_part->_pointers.begin(), it_point_begin, it_point_begin + middle_ind);

    right_part->_data.insert(right_part->_data.begin(), it_keys_begin + middle_ind, it_keys_end);
//    right_part->_pointers.insert(right_part->_pointers.begin(), it_point_begin + middle_ind + 1, it_point_end);


    auto tmp = node_to_split->_next;
    node_to_split->_next = right_part;
    right_part->_next = tmp;


    parent_node->_keys.insert(parent_node->_keys.begin() + split_index_in_parent, node_to_split->_data[middle_ind].first);

    if (parent_is_null){
        parent_node->_pointers.insert(parent_node->_pointers.begin() + split_index_in_parent, node_to_split);
    }
    parent_node->_pointers.insert(parent_node->_pointers.begin() + split_index_in_parent + 1, right_part);

    for(size_t i = parent_node->_keys.size() + 1; i < parent_node->_pointers.size(); i++){
        parent_node->_pointers[i] = nullptr;
    }

    node_to_split->_data.assign(it_keys_begin, it_keys_begin + middle_ind);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare::operator()(lhs, rhs);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
{
    _allocator = alloc;
    _logger = logger;
    _root = nullptr;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(pp_allocator<value_type> alloc, const compare& cmp, logger* logger)
{
    _allocator = alloc;
    _logger = logger;
    _root = nullptr;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template<input_iterator_for_pair<tkey, tvalue> iterator>
BP_tree<tkey, tvalue, compare, t>::BP_tree(iterator begin, iterator end, const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> template<input_iterator_for_pair<tkey, tvalue> iterator> BP_tree<tkey, tvalue, compare, t>::BP_tree(iterator begin, iterator end, const compare& cmp, pp_allocator<value_type> alloc, logger* logger)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp, pp_allocator<value_type> alloc, logger* logger)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> BP_tree<tkey, tvalue, compare, t>::BP_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp, pp_allocator<value_type> alloc, logger* logger)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(const BP_tree& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> BP_tree<tkey, tvalue, compare, t>::BP_tree(const BP_tree& other)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::BP_tree(BP_tree&& other) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> BP_tree<tkey, tvalue, compare, t>::BP_tree(BP_tree&& other) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>& BP_tree<tkey, tvalue, compare, t>::operator=(const BP_tree& other)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> BP_tree<tkey, tvalue, compare, t>& BP_tree<tkey, tvalue, compare, t>::operator=(const BP_tree& other)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>& BP_tree<tkey, tvalue, compare, t>::operator=(BP_tree&& other) noexcept
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> BP_tree<tkey, tvalue, compare, t>& BP_tree<tkey, tvalue, compare, t>::operator=(BP_tree&& other) noexcept", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
BP_tree<tkey, tvalue, compare, t>::~BP_tree() noexcept
{
    clear();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::begin()
{
    //наименьшее значение, то есть самое левое
    if (_root == nullptr) {
        return bptree_iterator(nullptr, 0);
    }
    bptree_node_base *cur_node = _root;
    while (!cur_node->_is_terminate
           && static_cast<bptree_node_middle*>(cur_node)->_pointers.size() > 0
           && static_cast<bptree_node_middle*>(cur_node)->_pointers[0] != nullptr) {
        cur_node = static_cast<bptree_node_middle*>(cur_node)->_pointers[0];
    }

    return bptree_iterator(static_cast<bptree_node_term*>(cur_node), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::end()
{
    //наибольшее значение, то есть самое правое
    if (_root == nullptr){
        return bptree_iterator(nullptr,  0);
    }
    bptree_node_base * cur_node = _root;

    while (!cur_node->_is_terminate
           && static_cast<bptree_node_middle*>(cur_node)->_pointers.size() > 0) {
        auto node_casted = static_cast<bptree_node_middle*>(cur_node);
        size_t ind = node_casted->_pointers.size() - 1;
        if (node_casted->_pointers[ind] != nullptr){
            cur_node = node_casted->_pointers[ind];
        } else {
            break;
        }
    }
    auto res_term = static_cast<bptree_node_term*>(cur_node);
    return bptree_iterator(res_term, res_term->_data.size());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::begin() const
{
    //наименьшее значение, то есть самое левое
    if (_root == nullptr) {
        return bptree_iterator(nullptr, 0);
    }
    bptree_node_base *cur_node = _root;
    while (!cur_node->_is_terminate
           && static_cast<bptree_node_middle*>(cur_node)->_pointers.size() > 0
           && static_cast<bptree_node_middle*>(cur_node)->_pointers[0] != nullptr) {
        cur_node = cur_node->_pointers[0];
    }

    return bptree_iterator(static_cast<bptree_node_term*>(cur_node), 0);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::end() const
{
    //наибольшее значение, то есть самое правое
    if (_root == nullptr){
        return bptree_iterator(nullptr,  0);
    }
    bptree_node_base * cur_node = _root;

    while (!cur_node->_is_terminate
           && static_cast<bptree_node_middle*>(cur_node)->_pointers.size() > 0) {
        size_t ind = static_cast<bptree_node_middle*>(cur_node)->_pointers.size() - 1;
        if (cur_node->_pointers[ind] != nullptr){
            cur_node = cur_node->_pointers[ind];
        } else {
            break;
        }
    }
    auto res_term = static_cast<bptree_node_term*>(cur_node);
    return bptree_iterator(res_term, res_term->_data.size());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::cbegin() const
{
    //наименьшее значение, то есть самое левое
    if (_root == nullptr) {
        return bptree_const_iterator(nullptr, 0);
    }
    bptree_node_base *cur_node = _root;
    while (!cur_node->_is_terminate
           && static_cast<bptree_node_middle*>(cur_node)->_pointers.size() > 0
           && static_cast<bptree_node_middle*>(cur_node)->_pointers[0] != nullptr) {
        cur_node = static_cast<bptree_node_middle*>(cur_node)->_pointers[0];
    }
    auto res = bptree_const_iterator(static_cast<bptree_node_term*>(cur_node), 0);
    res._is_first_node = true;
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::cend() const
{
    //наибольшее значение, то есть самое правое
    if (_root == nullptr){
        return bptree_const_iterator(nullptr,  0);
    }
    bptree_node_base * cur_node = _root;

    while (!cur_node->_is_terminate
           && static_cast<bptree_node_middle*>(cur_node)->_pointers.size() > 0) {
        auto node_casted = static_cast<bptree_node_middle*>(cur_node);
        size_t ind = node_casted->_pointers.size() - 1;
        if (node_casted->_pointers[ind] != nullptr){
            cur_node = node_casted->_pointers[ind];
        } else {
            break;
        }
    }
    auto res_term = static_cast<bptree_node_term*>(cur_node);
    return bptree_const_iterator(res_term, res_term->_data.size());
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
size_t BP_tree<tkey, tvalue, compare, t>::size() const noexcept
{
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::empty() const noexcept
{
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::find(const tkey& key)
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = _root;
    while(cur_node != nullptr && !cur_node->_is_terminate){
        i = 0;
        auto cur_node_casted = static_cast<bptree_node_middle*>(cur_node);
        for(; i < cur_node_casted->_keys.size() && compare_keys(cur_node_casted->_keys[i], key); i++){}

        if (i == cur_node_casted->_keys.size() - 1 && key == cur_node_casted->_keys[i]){
            cur_node = cur_node_casted->_pointers[i + 1];
        } else {
            cur_node = cur_node_casted->_pointers[i];
        }
        prev_i = i;
    }

    if (cur_node == nullptr){
        return end();
    } else {
        auto cur_node_casted = static_cast<bptree_node_term*>(cur_node);
        for(i = 0; i < cur_node_casted->_data.size() && cur_node_casted->_data[i].first != key; i++){}
        if (i != cur_node_casted->_data.size()){
            return bptree_iterator(cur_node_casted, i);
        }
    }

    return end();
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::find(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::find(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::lower_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_const_iterator BP_tree<tkey, tvalue, compare, t>::upper_bound(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
bool BP_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> bool BP_tree<tkey, tvalue, compare, t>::contains(const tkey& key) const", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::clear() noexcept
{
    while(!empty()){
        erase(begin());
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool> BP_tree<tkey, tvalue, compare, t>::insert(tree_data_type&& data)
{
    return insert(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <typename ...Args>
std::pair<typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator, bool> BP_tree<tkey, tvalue, compare, t>::emplace(Args&&... args)
{
    return this->insert(tree_data_type(args ...));
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::insert_or_assign(const tree_data_type& data)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::insert_or_assign(tree_data_type&& data)
{
    return insert(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
template <typename ...Args>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> template <typename ...Args> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::emplace_or_assign(Args&&... args)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator pos)
{
    return erase(pos->first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator pos)
{
    return erase(pos->first);
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator beg, bptree_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_iterator beg, bptree_iterator en)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator beg, bptree_const_iterator en)
{
    throw not_implemented("template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t> typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(bptree_const_iterator beg, bptree_const_iterator en)", "your code should be here...");
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
typename BP_tree<tkey, tvalue, compare, t>::bptree_iterator BP_tree<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    if (_root == nullptr){
        throw std::runtime_error("incorrect erase");
    }
    size_t index = 0;
    size_t prev_index = 0;
    auto cur_node = _root;
    std::stack<std::pair<bptree_node_base *, size_t>> st = std::stack<std::pair<bptree_node_base *, size_t>>();
    bptree_node_base *parent = nullptr;
    bool found = false;
    //находим нужный ключ
    while(!cur_node->_is_terminate){

        auto cur_node_casted = static_cast<bptree_node_middle*>(cur_node);
        for(index = 0; index < cur_node_casted->_keys.size() && compare_keys(cur_node_casted->_keys[index], key); index++){}
        st.push(std::pair(cur_node, prev_index));
        if (index < cur_node_casted->_keys.size() && cur_node_casted->_keys[index] == key){
            found = true;
            break;
        }

        parent = cur_node;
        if (index >= cur_node_casted->_pointers.size()){
            return end();
        }
        cur_node = cur_node_casted->_pointers[index];
        prev_index = index;
    }

    st.push(std::pair(cur_node, prev_index));
    auto cur_node_casted = static_cast<bptree_node_term*>(cur_node);
    for(index = 0; index < cur_node_casted->_data.size() && cur_node_casted->_data[index].first != key; index++){}

    if (index == cur_node_casted->_data.size()){
        return end();
    }



    delete_key_from_leaf(index, static_cast<bptree_node_term*>(cur_node), st);
    _size--;
    return bptree_iterator();
}



template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::delete_key_from_leaf(size_t index_of_key, BP_tree::bptree_node_term * node,
                                                             std::stack<std::pair<bptree_node_base*, size_t>> st)  {
    if (node->_data.size() > minimum_keys_in_node){
        node->_data.erase(node->_data.begin() + index_of_key);
        return;
    }
    bptree_node_base *cur_node = st.top().first;
    size_t i = st.top().second;
    st.pop();
    bptree_node_middle *parent = st.empty() ? nullptr : static_cast<bptree_node_middle*>(st.top().first);
    if (parent == nullptr) {
        if (node->_data.size() > 1) {
            // в корне может быть < mininmum_keys_in_node ключей, спокойно удаляем
            node->_data.erase(node->_data.begin() + index_of_key);
        } else {
            // в корне один элемент, удаляем его
            _root = nullptr;
            _allocator.delete_object(node);
        }
        return;
    }

    size_t size_left_sibling = 0;
    size_t size_right_sibling = 0;

    if (i > 0 && parent->_pointers[i - 1] != nullptr) {
        size_left_sibling = parent->_pointers[i - 1]->size();
    }

    if (i + 1 < parent->_pointers.size() && parent->_pointers[i + 1] != nullptr) {
        size_right_sibling = parent->_pointers[i + 1]->size();
    }

    bptree_node_term* left_sibling = i > 0 ? static_cast<bptree_node_term*>(parent->_pointers[i - 1]) : nullptr;
    bptree_node_term* right_sibling = i + 1 < parent->_pointers.size() ? static_cast<bptree_node_term*>(parent->_pointers[i + 1]) : nullptr;
    if (size_left_sibling > minimum_keys_in_node) {
        node->_data.erase(node->_data.begin() + index_of_key);

        node->_data.insert(node->_data.begin(), left_sibling->_data[size_left_sibling - 1]);

        parent->_keys[i - 1] = left_sibling->_data[size_left_sibling - 1].first;

        left_sibling->_data.erase(left_sibling->_data.begin() + size_left_sibling - 1);
    } else if (size_right_sibling > minimum_keys_in_node) {
        node->_data.erase(node->_data.begin() + index_of_key);

        node->_data.insert(node->_data.end(), right_sibling->_data[0]);

        parent->_keys[i] = right_sibling->_data[1].first;

        right_sibling->_data.erase(right_sibling->_data.begin());
    } else {
        node->_data.erase(node->_data.begin() + index_of_key);

        if (size_left_sibling > 0) {
            merge(left_sibling, cur_node, parent, i - 1);
        } else if (size_right_sibling > 0) {
            merge(cur_node, right_sibling, parent, i);
        }
        st.pop();

        //Если после мержа в родителе осталось сликшом мало ключей, продолжаем мержить
        while(!st.empty() && parent->_keys.size() < minimum_keys_in_node){
            cur_node = parent;
            parent = static_cast<bptree_node_middle*>(st.top().first);
            auto left_sibling_m = static_cast<bptree_node_middle*>(parent->_pointers[i - 1]);
            auto right_sibling_m = static_cast<bptree_node_middle*>(parent->_pointers[i + 1]);
            i = st.top().second;
            st.pop();

            if (i > 0 && left_sibling_m != nullptr) {
                size_left_sibling = left_sibling_m->_keys.size();
            }

            if (i + 1 < parent->_pointers.size() && right_sibling_m != nullptr) {
                size_right_sibling = right_sibling_m->_keys.size();
            }

            if (size_left_sibling > 0) {
                merge(left_sibling_m, cur_node, parent, i - 1);
            } else if (size_right_sibling > 0) {
                merge(cur_node, right_sibling_m, parent, i);
            } else {
                throw std::logic_error("no siblings to merge");
            }
        }

        //parent = _root, так как мы поднимаемся до самого верха
        // если вдруг в корне осталось 0 ключей, заменяем его
        if (parent->_keys.size() == 0){
            if (parent->_pointers.size() > 0) {
                if (parent->_pointers[0] != nullptr) {
                    _root = parent->_pointers[0];
                } else {
                    if (parent->_pointers.size() > 1) {
                        _root = parent->_pointers[1];
                    } else {
                        _root = nullptr;
                    }
                }
            } else {
                _root = nullptr;
            }
            _allocator.delete_object(parent);
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::merge(BP_tree::bptree_node_base * left, BP_tree::bptree_node_base * right,
                                              bptree_node_middle * parent, size_t split_key_index) {
    if (left->_is_terminate){
        merge_terminate(static_cast<bptree_node_term*>(left), static_cast<bptree_node_term*>(right), parent, split_key_index);
    } else {
        merge_middle(static_cast<bptree_node_middle*>(left), static_cast<bptree_node_middle*>(right), parent, split_key_index);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void
BP_tree<tkey, tvalue, compare, t>::merge_terminate(BP_tree::bptree_node_term *left, BP_tree::bptree_node_term *right,
                                                   BP_tree::bptree_node_middle *parent, size_t split_key_index) {
    bptree_node_term *  new_node = _allocator.template new_object<bptree_node_term>();
    new_node->_data = left->_data;

    new_node->_data.insert(new_node->_data.end(), right->_data.begin(), right->_data.end());

    parent->_keys.erase(parent->_keys.begin() + split_key_index);
    parent->_pointers.erase(parent->_pointers.begin() + split_key_index);

    if (parent->_pointers.size() > split_key_index) {
        parent->_pointers[split_key_index] = new_node;
    } else {
        parent->_pointers.push_back(new_node);
    }

    if (parent->_pointers.size() > split_key_index + 1) {
        parent->_pointers[split_key_index+1] = nullptr;
    } else {
        parent->_pointers.push_back(nullptr);
    }

    _allocator.delete_object(left);
    _allocator.delete_object(right);
}


template<typename tkey, typename tvalue, compator<tkey> compare, std::size_t t>
void BP_tree<tkey, tvalue, compare, t>::merge_middle(BP_tree::bptree_node_middle * left, BP_tree::bptree_node_middle * right,
                                                        bptree_node_middle * parent, size_t split_key_index) {
    bptree_node_middle *  new_node = _allocator.template new_object<bptree_node_middle>();
    new_node->_keys = left->_keys;
    new_node->_pointers = left->_pointers;
    new_node->_keys.push_back(parent->_keys[split_key_index]);
    new_node->_pointers.push_back(nullptr);

    new_node->_keys.insert(new_node->_keys.end(), right->_keys.begin(), right->_keys.end());
    new_node->_pointers.insert(new_node->_pointers.end(), right->_pointers.begin(), right->_pointers.end());

    parent->_keys.erase(parent->_keys.begin() + split_key_index);
    parent->_pointers.erase(parent->_pointers.begin() + split_key_index);

    if (parent->_pointers.size() > split_key_index) {
        parent->_pointers[split_key_index] = new_node;
    } else {
        parent->_pointers.push_back(new_node);
    }

    if (parent->_pointers.size() > split_key_index + 1) {
        parent->_pointers[split_key_index+1] = nullptr;
    } else {
        parent->_pointers.push_back(nullptr);
    }

    _allocator.delete_object(left);
    _allocator.delete_object(right);
}

#endif