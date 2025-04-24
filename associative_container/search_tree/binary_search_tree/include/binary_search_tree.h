#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H

#include <stack>
#include <vector>
#include <logger.h>
#include <not_implemented.h>
#include <search_tree.h>
#include <ranges>
#include <pp_allocator.h>

namespace __detail {
    template<typename tkey, typename tvalue, typename compare, typename tag>
    class bst_impl;

    class BST_TAG;
}


template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>, typename tag = __detail::BST_TAG>
class binary_search_tree : private compare {
public:

    using value_type = std::pair<const tkey, tvalue>;

    friend class __detail::bst_impl<tkey, tvalue, compare, tag>;

protected:


    struct node
    {

    public:

        value_type data;

        node *parent;
        node *left_subtree;
        node *right_subtree;

        template<class ...Args>
        explicit node(node *parent, Args &&...args);


        virtual ~node() = default;
    };

    inline bool compare_keys(const tkey &lhs, const tkey &rhs) const;

    inline bool compare_pairs(const value_type &lhs, const value_type &rhs) const;

public:


    // region iterators definition

    class prefix_iterator;

    class prefix_const_iterator;

    class prefix_reverse_iterator;

    class prefix_const_reverse_iterator;

    class infix_iterator;

    class infix_const_iterator;

    class infix_reverse_iterator;

    class infix_const_reverse_iterator;

    class postfix_iterator;

    class postfix_const_iterator;

    class postfix_reverse_iterator;

    class postfix_const_reverse_iterator;

    /** @brief Watch about behavior of reverse iterators.
     *
     * @example Arrow is common iterator
     *  1 2 3 -> 4 5 6 7
     *  *it == 4.
     *
     *  @example But reverse:
     *  1 2 3 \<- 4 5 6 7
     *  *rit == 3
     */


    class prefix_iterator
    {
        friend binary_search_tree;
    protected:

        node *_data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node *_backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_iterator(node *data = nullptr);

        virtual ~prefix_iterator() = default;

        bool operator==(
                prefix_iterator const &other) const noexcept;

        bool operator!=(
                prefix_iterator const &other) const noexcept;

        prefix_iterator &operator++() & noexcept;

        prefix_iterator operator++(int not_used) noexcept;

        prefix_iterator &operator--() & noexcept;

        prefix_iterator const operator--(int not_used) noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;


        node *get_node() const;
    };

    class prefix_const_iterator {
    protected:

        prefix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type &;
        using pointer = value_type *const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_const_iterator(const node *data = nullptr);


        explicit prefix_const_iterator(const prefix_iterator &) noexcept;

        virtual ~prefix_const_iterator() = default;

        bool operator==(
                prefix_const_iterator const &other) const noexcept;

        bool operator!=(
                prefix_const_iterator const &other) const noexcept;

        prefix_const_iterator &operator++() & noexcept;

        prefix_const_iterator operator++(int not_used) noexcept;

        prefix_const_iterator &operator--() & noexcept;

        prefix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };

    class prefix_reverse_iterator
    {
    protected:

        prefix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_reverse_iterator(node *data = nullptr);

        prefix_reverse_iterator(const prefix_iterator &) noexcept;

        operator prefix_iterator() const noexcept;

        prefix_iterator base() const noexcept;

        virtual ~prefix_reverse_iterator() = default;

        bool operator==(prefix_reverse_iterator const &other) const noexcept;

        bool operator!=(prefix_reverse_iterator const &other) const noexcept;

        prefix_reverse_iterator &operator++() & noexcept;

        prefix_reverse_iterator operator++(int not_used) noexcept;

        prefix_reverse_iterator &operator--() & noexcept;

        prefix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };


    class prefix_const_reverse_iterator
    {
    protected:

        prefix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type &;
        using pointer = value_type *const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit prefix_const_reverse_iterator(const node *data = nullptr);

        prefix_const_reverse_iterator(const prefix_const_iterator &) noexcept;

        operator prefix_const_iterator() const noexcept;

        prefix_const_iterator base() const noexcept;

        virtual ~prefix_const_reverse_iterator() = default;

        bool operator==(prefix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(prefix_const_reverse_iterator const &other) const noexcept;

        prefix_const_reverse_iterator &operator++() & noexcept;

        prefix_const_reverse_iterator operator++(int not_used) noexcept;

        prefix_const_reverse_iterator &operator--() & noexcept;

        prefix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };



    class infix_iterator
    {
        friend binary_search_tree;
    protected:

        node *_data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node *_backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;


        node* get_data() const { return _data; }

        explicit infix_iterator(node* data = nullptr);

        virtual ~infix_iterator() = default;

        bool operator==(
                infix_iterator const &other) const noexcept;

        bool operator!=(
                infix_iterator const &other) const noexcept;

        infix_iterator &operator++() & noexcept;

        infix_iterator operator++(int not_used) noexcept;

        infix_iterator &operator--() & noexcept;

        infix_iterator const operator--(int not_used) noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

        node *get_node() const;

    };

    class infix_const_iterator {
    protected:

        infix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type &;
        using pointer = value_type *const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_const_iterator(const node *data = nullptr);

        infix_const_iterator(const infix_iterator &) noexcept;

        virtual ~infix_const_iterator() = default;

        bool operator==(
                infix_const_iterator const &other) const noexcept;

        bool operator!=(
                infix_const_iterator const &other) const noexcept;

        infix_const_iterator &operator++() & noexcept;

        infix_const_iterator operator++(int not_used) noexcept;

        infix_const_iterator &operator--() & noexcept;

        infix_const_iterator operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };

    class infix_reverse_iterator {
    protected:

        infix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_reverse_iterator(node *data = nullptr);

        infix_reverse_iterator(const infix_iterator &) noexcept;

        operator infix_iterator() const noexcept;

        infix_iterator base() const noexcept;

        virtual ~infix_reverse_iterator() = default;

        bool operator==(infix_reverse_iterator const &other) const noexcept;

        bool operator!=(infix_reverse_iterator const &other) const noexcept;

        infix_reverse_iterator &operator++() & noexcept;

        infix_reverse_iterator operator++(int not_used) noexcept;

        infix_reverse_iterator &operator--() & noexcept;

        infix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };

    class infix_const_reverse_iterator {
    protected:

        infix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type &;
        using pointer = value_type *const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit infix_const_reverse_iterator(const node *data = nullptr);

        infix_const_reverse_iterator(const infix_const_iterator &) noexcept;

        operator infix_const_iterator() const noexcept;

        infix_const_iterator base() const noexcept;

        virtual ~infix_const_reverse_iterator() = default;

        bool operator==(infix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(infix_const_reverse_iterator const &other) const noexcept;

        infix_const_reverse_iterator &operator++() & noexcept;

        infix_const_reverse_iterator operator++(int not_used) noexcept;

        infix_const_reverse_iterator &operator--() & noexcept;

        infix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };



    class postfix_iterator
    {
        friend binary_search_tree;
    protected:

        node *_data;

        /** If iterator == end or before_begin _data points to nullptr, _backup to last node
         *
         */
        node *_backup;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_iterator(node *data = nullptr);

        virtual ~postfix_iterator() = default;

        bool operator==(
                postfix_iterator const &other) const noexcept;

        bool operator!=(
                postfix_iterator const &other) const noexcept;

        postfix_iterator &operator++() & noexcept;

        postfix_iterator operator++(int not_used) noexcept;

        postfix_iterator &operator--() & noexcept;

        postfix_iterator const operator--(int not_used) noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

        node *get_node() const;
    };

    class postfix_const_iterator {
    protected:

        postfix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type &;
        using pointer = value_type *const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_const_iterator(const node *data = nullptr);

        postfix_const_iterator(const postfix_iterator &) noexcept;

        virtual ~postfix_const_iterator() = default;

        bool operator==(
                postfix_const_iterator const &other) const noexcept;

        bool operator!=(
                postfix_const_iterator const &other) const noexcept;

        postfix_const_iterator &operator++() & noexcept;

        postfix_const_iterator operator++(int not_used) noexcept;

        postfix_const_iterator &operator--() & noexcept;

        postfix_const_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };

    class postfix_reverse_iterator {
    protected:

        postfix_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = value_type &;
        using pointer = value_type *;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_reverse_iterator(node *data = nullptr);

        postfix_reverse_iterator(const postfix_iterator &) noexcept;

        operator postfix_iterator() const noexcept;

        postfix_iterator base() const noexcept;

        virtual ~postfix_reverse_iterator() = default;

        bool operator==(postfix_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_reverse_iterator const &other) const noexcept;

        postfix_reverse_iterator &operator++() & noexcept;

        postfix_reverse_iterator operator++(int not_used) noexcept;

        postfix_reverse_iterator &operator--() & noexcept;

        postfix_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };

    class postfix_const_reverse_iterator {
    protected:

        postfix_const_iterator _base;

    public:

        using value_type = binary_search_tree<tkey, tvalue, compare>::value_type;
        using difference_type = ptrdiff_t;
        using reference = const value_type &;
        using pointer = value_type *const;
        using iterator_category = std::bidirectional_iterator_tag;

        explicit postfix_const_reverse_iterator(const node *data = nullptr);

        postfix_const_reverse_iterator(const postfix_const_iterator &) noexcept;

        operator postfix_const_iterator() const noexcept;

        postfix_const_iterator base() const noexcept;

        virtual ~postfix_const_reverse_iterator() = default;

        bool operator==(postfix_const_reverse_iterator const &other) const noexcept;

        bool operator!=(postfix_const_reverse_iterator const &other) const noexcept;

        postfix_const_reverse_iterator &operator++() & noexcept;

        postfix_const_reverse_iterator operator++(int not_used) noexcept;

        postfix_const_reverse_iterator &operator--() & noexcept;

        postfix_const_reverse_iterator const operator--(int not_used) const noexcept;

        /** Throws exception if end
         */
        reference operator*() const;

        /** UB if iterator points to end
         *
         */

        pointer operator->() noexcept;

        size_t depth() const noexcept;

    };


    friend class prefix_iterator;

    friend class prefix_const_iterator;

    friend class prefix_reverse_iterator;

    friend class prefix_const_reverse_iterator;

    friend class postfix_iterator;

    friend class postfix_const_iterator;

    friend class postfix_reverse_iterator;

    friend class postfix_const_reverse_iterator;

    friend class infix_iterator;

    friend class infix_const_iterator;

    friend class infix_reverse_iterator;

    friend class infix_const_reverse_iterator;

    // endregion iterators definition


protected:

    node *_root;
    logger *_logger;
    size_t _size;

    /** You should use coercion ctor or template methods of allocator
     */
    pp_allocator<value_type> _allocator;

public:
    explicit binary_search_tree(
            const compare &comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit binary_search_tree(
            pp_allocator<value_type> alloc,
            const compare &comp = compare(),
            logger *log = nullptr);


    template<input_iterator_for_pair<tkey, tvalue> iterator>

    explicit binary_search_tree(iterator begin, iterator end, const compare &cmp = compare(),
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                                logger *log = nullptr);

    template<std::ranges::input_range Range>
    explicit binary_search_tree(Range &&range, const compare &cmp = compare(),
                                pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                                logger *log = nullptr);


    binary_search_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(),
                       pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                       logger *log = nullptr);

public:

    binary_search_tree(const binary_search_tree &other);

    binary_search_tree(binary_search_tree &&other) noexcept;

    binary_search_tree &operator=(const binary_search_tree &other);

    binary_search_tree &operator=(binary_search_tree &&other) noexcept;

    virtual ~binary_search_tree();

public:

    tvalue &at(const tkey &key);

    const tvalue &at(const tkey &key) const;

    tvalue &operator[](const tkey &key);

    tvalue &operator[](tkey &&key);

    bool empty() const noexcept;

    size_t size() const noexcept;

    void clear() noexcept;

    std::pair<infix_iterator, bool> insert(const value_type &);

    std::pair<infix_iterator, bool> insert(value_type &&);

    template<std::input_iterator InputIt>
    void insert(InputIt first, InputIt last);

    template<std::ranges::input_range R>
    void insert_range(R &&rg);

    template<class ...Args>
    std::pair<infix_iterator, bool> emplace(Args &&...args);

    infix_iterator insert_or_assign(const value_type &);

    infix_iterator insert_or_assign(value_type &&);

    template<std::input_iterator InputIt>
    void insert_or_assign(InputIt first, InputIt last);


    template<class ...Args>
    infix_iterator emplace_or_assign(Args &&...args);

    virtual void swap(binary_search_tree &other) noexcept;

    bool contains(const tkey &key) const;

    infix_iterator find(const tkey &);

    infix_const_iterator find(const tkey &) const;


    infix_iterator lower_bound(const tkey &);

    infix_const_iterator lower_bound(const tkey &) const;

    infix_iterator upper_bound(const tkey &);

    infix_const_iterator upper_bound(const tkey &) const;

    infix_iterator erase(infix_iterator pos);

    infix_iterator erase(infix_const_iterator pos);

    infix_iterator erase(infix_iterator first, infix_iterator last);

    infix_iterator erase(infix_const_iterator first, infix_const_iterator last);

    size_t erase(const tkey &key);

public:

    // region iterators requests definition

    infix_iterator begin() noexcept;

    infix_iterator end() noexcept;

    infix_const_iterator begin() const noexcept;

    infix_const_iterator end() const noexcept;

    infix_const_iterator cbegin() const noexcept;

    infix_const_iterator cend() const noexcept;

    infix_reverse_iterator rbegin() noexcept;

    infix_reverse_iterator rend() noexcept;

    infix_const_reverse_iterator rbegin() const noexcept;

    infix_const_reverse_iterator rend() const noexcept;

    infix_const_reverse_iterator crbegin() const noexcept;

    infix_const_reverse_iterator crend() const noexcept;


    prefix_iterator begin_prefix() noexcept;

    prefix_iterator end_prefix() noexcept;

    prefix_const_iterator begin_prefix() const noexcept;

    prefix_const_iterator end_prefix() const noexcept;

    prefix_const_iterator cbegin_prefix() const noexcept;

    prefix_const_iterator cend_prefix() const noexcept;

    prefix_reverse_iterator rbegin_prefix() noexcept;

    prefix_reverse_iterator rend_prefix() noexcept;

    prefix_const_reverse_iterator rbegin_prefix() const noexcept;

    prefix_const_reverse_iterator rend_prefix() const noexcept;

    prefix_const_reverse_iterator crbegin_prefix() const noexcept;

    prefix_const_reverse_iterator crend_prefix() const noexcept;


    infix_iterator begin_infix() noexcept;

    infix_iterator end_infix() noexcept;

    infix_const_iterator begin_infix() const noexcept;

    infix_const_iterator end_infix() const noexcept;

    infix_const_iterator cbegin_infix() const noexcept;

    infix_const_iterator cend_infix() const noexcept;

    infix_reverse_iterator rbegin_infix() noexcept;

    infix_reverse_iterator rend_infix() noexcept;

    infix_const_reverse_iterator rbegin_infix() const noexcept;

    infix_const_reverse_iterator rend_infix() const noexcept;

    infix_const_reverse_iterator crbegin_infix() const noexcept;

    infix_const_reverse_iterator crend_infix() const noexcept;


    postfix_iterator begin_postfix() noexcept;

    postfix_iterator end_postfix() noexcept;

    postfix_const_iterator begin_postfix() const noexcept;

    postfix_const_iterator end_postfix() const noexcept;

    postfix_const_iterator cbegin_postfix() const noexcept;

    postfix_const_iterator cend_postfix() const noexcept;

    postfix_reverse_iterator rbegin_postfix() noexcept;

    postfix_reverse_iterator rend_postfix() noexcept;

    postfix_const_reverse_iterator rbegin_postfix() const noexcept;

    postfix_const_reverse_iterator rend_postfix() const noexcept;

    postfix_const_reverse_iterator crbegin_postfix() const noexcept;

    postfix_const_reverse_iterator crend_postfix() const noexcept;

    // endregion iterators requests definition

protected:



    // region subtree rotations definition

    static void small_left_rotation(node *&subtree_root) noexcept;

    static void small_right_rotation(node *&subtree_root) noexcept;

    static void big_left_rotation(node *&subtree_root) noexcept;

    static void big_right_rotation(node *&subtree_root) noexcept;

    static void double_left_rotation(node *&subtree_root) noexcept;

    static void double_right_rotation(node *&subtree_root) noexcept;

    // endregion subtree rotations definition

    node *go_to_node_with_key(const tkey &key);

    static node *get_next_prefix(node *n);

    static node *get_prev_prefix(node *n);

    static node *get_next_infix(node *n);

    static node *get_prev_infix(node *n);

    static node *get_next_postfix(node *n);

    static node *get_prev_postfix(node *n);
};


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::get_node() const {
    return _data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::get_node() const {
    return _data;
}


namespace __detail {
    template<typename tkey, typename tvalue, typename compare, typename tag>
    class bst_impl {
    public:
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, tag>::node *
        create_node(binary_search_tree<tkey, tvalue, compare, tag> &cont, Args &&...args);


        static void delete_node(binary_search_tree<tkey, tvalue, compare, tag> &cont,
                                binary_search_tree<tkey, tvalue, compare, tag>::node *n);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, tag>::node **) {}

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, tag> &cont,
                                binary_search_tree<tkey, tvalue, compare, tag>::node **) {}

        static void erase(binary_search_tree<tkey, tvalue, compare, tag> &cont,
                          binary_search_tree<tkey, tvalue, compare, tag>::node **);

        static void swap(binary_search_tree<tkey, tvalue, compare, tag> &lhs,
                         binary_search_tree<tkey, tvalue, compare, tag> &rhs) noexcept;
    };
}


template<typename tkey, typename tvalue, typename compare, typename tag>
void __detail::bst_impl<tkey, tvalue, compare, tag>::swap(binary_search_tree<tkey, tvalue, compare, tag> &lhs,
                                                          binary_search_tree<tkey, tvalue, compare, tag> &rhs) noexcept {
    std::swap(lhs._root, rhs._root);
    std::swap(lhs._allocator, rhs._allocator);
    std::swap(lhs._size, rhs._size);
    std::swap(lhs._logger, rhs._logger);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<input_iterator_for_pair<tkey, tvalue> iterator>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(iterator begin, iterator end, const compare &cmp,
                                                                   pp_allocator<typename binary_search_tree<tkey, tvalue, compare, tag>::value_type> alloc,
                                                                   logger *logger) {
    _allocator = alloc;
    _logger = logger;
    insert(begin, end);
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::compare_pairs(const binary_search_tree::value_type &lhs,
                                                                   const binary_search_tree::value_type &rhs) const {
    return compare()(lhs.first, rhs.first);
}


template<typename compare, typename U, typename iterator>
explicit binary_search_tree(iterator begin, iterator end, const compare &cmp = compare(),
                            pp_allocator<U> alloc = pp_allocator<U>(),
                            logger *logger = nullptr)->binary_search_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit binary_search_tree(Range &&range, const compare &cmp = compare(),
                            pp_allocator<U> alloc = pp_allocator<U>(),
                            logger *logger = nullptr)->binary_search_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare>;

template<typename tkey, typename tvalue, typename compare, typename U>
binary_search_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(),
                   pp_allocator<U> alloc = pp_allocator<U>(),
                   logger *logger = nullptr) -> binary_search_tree<tkey, tvalue, compare>;


// region node implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
binary_search_tree<tkey, tvalue, compare, tag>::node::node(node *parent, Args &&...args) : data(args ...) {
    binary_search_tree<tkey, tvalue, compare, tag>::node::parent = parent;
    left_subtree = nullptr;
    right_subtree = nullptr;
}

// endregion node implementation


//region prefix_iterator
template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator++() & noexcept {
    _backup = _data;
    _data = binary_search_tree<tkey, tvalue, compare, tag>::get_next_prefix(_data);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::prefix_iterator(node *data) {
    _data = data;
    _backup = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator==(
        prefix_iterator const &other) const noexcept {
    return _data == other._data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator!=(
        prefix_iterator const &other) const noexcept {
    return !binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator==(other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator--() & noexcept {
    if (_data == nullptr) {
        _data = _backup;
        return *this;
    }
    _backup = _data;
    _data = get_prev_prefix(_data);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator--(int not_used) noexcept {

    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator*() const {
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::operator->() noexcept {
    return &(_data->data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator::depth() const noexcept {
    size_t res = 0;
    node *current = _data;
    if (current == nullptr) {
        return res;
    }
    while (current->parent != NULL) {
        current = current->parent;
        res++;
    }
    return res;
}
//endregion prefix_iterator

// region prefix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::prefix_const_iterator(const node *data) : _base(
        const_cast<node *>(data)) {
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::prefix_const_iterator(
        const prefix_iterator &other) noexcept {
    _base = other;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator==(
        const binary_search_tree::prefix_const_iterator &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator!=(
        const binary_search_tree::prefix_const_iterator &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator++() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator--() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator::depth() const noexcept {
    return _base.depth();
}

// endregion prefix_const_iterator implementation

// region prefix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::prefix_reverse_iterator(node *data) : _base(
        data) {

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::prefix_reverse_iterator(
        const prefix_iterator &it) noexcept {
    _base = it;
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator() const noexcept {
    return std::prev(_base);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::base() const noexcept {
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator==(
        prefix_reverse_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator!=(
        prefix_reverse_iterator const &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator++() & noexcept {
    --_base;
    return (*this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator--() & noexcept {
    ++_base;
    return (*this);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator::depth() const noexcept {
    return _base.depth();
}

// endregion prefix_reverse_iterator implementation

// region prefix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        const node *data) : _base(data) {

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        const prefix_const_iterator &it) noexcept {
    _base = it;
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator() const noexcept {
    return std::prev(_base);
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::base() const noexcept {
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator==(
        prefix_const_reverse_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator!=(
        prefix_const_reverse_iterator const &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator++() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator--() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator::depth() const noexcept {
    return _base.depth();
}

// endregion prefix_const_reverse_iterator implementation

//region infix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::get_node() const {
    return _data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::infix_iterator(node *data) {
    _data = data;
    _backup = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator==(infix_iterator const &other) const noexcept {
    if (_data != nullptr || other._data != nullptr) {
        return _data == other._data;
    } else {
        return _backup == other._backup;
    }

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator!=(infix_iterator const &other) const noexcept {
    return !binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator==(other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator++() & noexcept {
    //return to backup if it points to end or before begin
    if (_data == nullptr) {
        _data = _backup;
        return *this;
    }
    _backup = _data;
    _data = get_next_infix(_data);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator++(int not_used) noexcept {
    infix_iterator copy = *this;
    ++(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator--() & noexcept {
    if (_data == nullptr) {
        _data = _backup;
        return *this;
    }
    _backup = _data;
    _data = get_prev_infix(_data);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator--(int not_used) noexcept {
    infix_iterator copy = *this;
    --(*this);
    return copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator*() const {
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::operator->() noexcept {
    return &(_data->data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator::depth() const noexcept {
    size_t res = 0;
    node *current = _data;
    while (current->parent != NULL) {
        current = current->parent;
        res++;
    }
    return res;
}


//endregion infix_iterator implementation

//region infix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::infix_const_iterator(const node *data) {
    _base._data = const_cast<node *>(data);
    _base._backup = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::infix_const_iterator(
        const infix_iterator &it) noexcept {
    _base = it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator==(
        binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator const &other) const noexcept {
    if (_base._data != nullptr && other._base != nullptr) {
        return _base._data == other._base._data;
    } else {
        return _base._backup == other._base._backup;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator!=(
        infix_const_iterator const &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator++() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++_base;
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator--() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --_base;
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator*() const {
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator::depth() const noexcept {
    return _base.depth();
}

//endregion infix_const_iterator implementation

//region infix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::infix_reverse_iterator(node *data) {
    _base(data);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::infix_reverse_iterator(
        const infix_iterator &it) noexcept {
    _base = it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator() const noexcept {
    return std::prev(_base);
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::base() const noexcept {
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator==(
        infix_reverse_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator!=(
        infix_reverse_iterator const &other) const noexcept {
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator++() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator--() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator*() const {
    return *_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator::depth() const noexcept {
    return _base.depth();
}

//endregion infix_reverse_iterator implementation

//region infix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        const node *data) : _base(data) {

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        const infix_const_iterator &it) noexcept {
    _base = it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator() const noexcept {
    return std::prev(_base);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::base() const noexcept {
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator==(
        infix_const_reverse_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator!=(
        infix_const_reverse_iterator const &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator++() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator--() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator::depth() const noexcept {
    return _base.depth();
}

//endregion infix_const_reverse_iterator implementation

//region postfix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::postfix_iterator(node *data) {
    _data = data;
    _backup = nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator==(
        postfix_iterator const &other) const noexcept {
    if (_data != nullptr || other._data != nullptr) {
        return _data == other._data;
    } else {
        return _backup == other._backup;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator!=(
        postfix_iterator const &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator++() & noexcept {
    if (_data == nullptr) {
        _data = _backup;
        return *this;
    }

    _backup = _data;
    _data = get_next_postfix(_data);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator++(int not_used) noexcept {
    auto Copy = (*this);
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator--() & noexcept {
    if (_data == nullptr) {
        _data = _backup;
        return *this;
    }

    _backup = _data;
    _data = get_prev_postfix(_data);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator--(int not_used) noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator*() const {
    return _data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::operator->() noexcept {
    return &_data->data;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator::depth() const noexcept {
    size_t res = 0;
    node *current = _data;
    while (current->parent != NULL) {
        current = current->parent;
        res++;
    }
    return res;
}

//endregion postfix_iterator implementation

// region postfix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::postfix_const_iterator(const node *data)
        : _base(const_cast<node *>(data)) {

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::postfix_const_iterator(
        const postfix_iterator &it) noexcept {
    _base = it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator==(
        postfix_const_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator!=(
        postfix_const_iterator const &other) const noexcept {
    return _base != other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator++() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator--() & noexcept {
    --_base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator::depth() const noexcept {
    return _base.depth();
}

// endregion postfix_const_iterator implementation

// region postfix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::postfix_reverse_iterator(node *data) : _base(
        data) {

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::postfix_reverse_iterator(
        const postfix_iterator &it) noexcept {
    _base = it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator() const noexcept {
    return std::prev(_base);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::base() const noexcept {
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator==(
        postfix_reverse_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator!=(
        postfix_reverse_iterator const &other) const noexcept {
    return !((*this) == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator++() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator--() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator--(int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator::depth() const noexcept {
    return _base.depth();
}

// endregion postfix_reverse_iterator implementation

// region postfix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        const node *data) : _base(data) {

}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        const postfix_const_iterator &it) noexcept {
    _base = it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator() const noexcept {
    return std::prev(_base);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::base() const noexcept {
    return _base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator==(
        postfix_const_reverse_iterator const &other) const noexcept {
    return _base == other._base;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator!=(
        postfix_const_reverse_iterator const &other) const noexcept {
    return !(*this == other);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator++() & noexcept {
    --_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator++(int not_used) noexcept {
    auto Copy = *this;
    ++(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator &
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator--() & noexcept {
    ++_base;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator const
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator--(
        int not_used) const noexcept {
    auto Copy = *this;
    --(*this);
    return Copy;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::reference
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator*() const {
    return _base.operator*();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::pointer
binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::operator->() noexcept {
    return _base.operator->();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator::depth() const noexcept {
    return _base.depth();
    return _base.depth();
}


// endregion postfix_const_reverse_iterator implementation


// region binary_search_tree methods_access implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::go_to_node_with_key(const tkey &key) {
    binary_search_tree<tkey, tvalue, compare, tag>::node *current = _root;
    while (current != nullptr) {
        if (key == current->data.first) {
            break;
        }
        if (compare_keys(key, current->data.first)) {
            current = current->left_subtree;
        } else {
            current = current->right_subtree;
        }
    }
    return current;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue &binary_search_tree<tkey, tvalue, compare, tag>::at(const tkey &key) {
    node *current = go_to_node_with_key(key);
    if (current == nullptr) {
        throw std::out_of_range("Incorrect key");
    }
    return current->data.second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
const tvalue &binary_search_tree<tkey, tvalue, compare, tag>::at(const tkey &key) const {
    return (*this).at(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue &binary_search_tree<tkey, tvalue, compare, tag>::operator[](const tkey &key) {
    return go_to_node_with_key(key)->data.second;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
tvalue &binary_search_tree<tkey, tvalue, compare, tag>::operator[](tkey &&key) {
    return (*this)[key];
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::empty() const noexcept {
    return _size == 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::size() const noexcept {
    return _size;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::clear() noexcept {

}

// endregion binary_search_tree methods_access implementation


// region binary_search_tree swap_method implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::swap(binary_search_tree &other) noexcept {
    __detail::bst_impl<tkey, tvalue, compare, tag>::swap(*this, other);
}

// endregion binary_search_tree swap_method implementation

// region binary_search_tree methods_search and methods_erase implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::contains(const tkey &key) const {
    node *n = go_to_node_with_key(key);
    return n == nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::find(const tkey &key) {
    node *n = go_to_node_with_key(key);
    infix_iterator it(n);
    post_search(&n);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::find(const tkey &key) const {
    node *n = go_to_node_with_key(key);
    infix_const_iterator it(n);
    post_search(&n);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::lower_bound(const tkey &key) {
    node *current = _root;
    while (current != nullptr) {
        if (compare_keys(key, current->data.first)) {
            if (compare_keys(key, current->left_subtree->data.first)) {

                current = current->left_subtree;
            } else {
                break;
            }
        } else {
            current = current->right_subtree;
        }
    }
    if (current == nullptr) {
        return end_infix();
    }
    infix_iterator it(current->left_subtree);

    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::lower_bound(const tkey &key) const {
    node *current = _root;
    while (current != nullptr) {
        if (compare_keys(key, current->data.first)) {
            if (current->left_subtree != nullptr && compare_keys(key, current->left_subtree->data.first)) {
                current = current->left_subtree;
            } else {
                break;
            }
        } else {
            current = current->right_subtree;
        }
    }
    if (current == nullptr) {
        return end_infix();
    }
    infix_const_iterator it(current);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::upper_bound(const tkey &key) {
    node *current = _root;
    node* prev = nullptr;
    node* prev_prev = nullptr;
    while (current != nullptr) {
        prev_prev = prev;
        prev = current;
        if (compare_keys(key, current->data.first)) {
            current = current->left_subtree;
        } else {
            current = current->right_subtree;
        }
    }

    auto mx = prev > prev_prev ? prev : prev_prev;
    auto mn = prev < prev_prev ? prev : prev_prev;

    if (compare_keys(key, mn->data.first)){
        return infix_iterator(mn);
    } else {
        return infix_iterator(mx);
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::upper_bound(const tkey &key) const {
    node *current = _root;
    node* prev;
    while (current != nullptr) {
        prev = current;
        if (compare_keys(key, current->data.first)) {
            current = current->left_subtree;
        } else {
            current = current->right_subtree;
        }
    }
    infix_iterator it(prev);

    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_iterator pos) {
    node *n = pos.get_node();
    __detail::bst_impl<tkey, tvalue, compare, tag>::erase(*this, &n);
    infix_iterator it(n);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_const_iterator pos) {
    erase(pos._base());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_iterator first, infix_iterator last) {
    infix_iterator prev = first;
    infix_iterator cur = first;
    while (cur != last) {
        prev = cur;
        cur++;
        erase(prev);
    }
    erase(prev);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::erase(infix_const_iterator first, infix_const_iterator last) {
    erase(first.base(), last._base());
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
size_t binary_search_tree<tkey, tvalue, compare, tag>::erase(const tkey &key) {
    node *n = go_to_node_with_key(key);
    __detail::bst_impl<tkey, tvalue, compare, tag>::erase(*this, &n);
    return size();
}



// endregion binary_search_tree methods_search and methods_erase implementation

// region infix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin() noexcept {
    return begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end() noexcept {
    return end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin() const noexcept {
    return begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end() const noexcept {
    return end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin() const noexcept {
    return begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend() const noexcept {
    return end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin() noexcept {
    infix_iterator a = end_infix();
    a--;
    infix_reverse_iterator it(a);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend() noexcept {
    infix_reverse_iterator it(std::prev(begin_infix()));
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin() const noexcept {
    infix_const_reverse_iterator it(std::prev(end_infix()));
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend() const noexcept {
    infix_const_iterator a = begin_infix();
    a--;
    infix_const_reverse_iterator it(a);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin() const noexcept {
    infix_const_reverse_iterator it(std::prev(end_infix()));
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend() const noexcept {
    infix_const_iterator a = begin_infix();
    a--;
    infix_const_reverse_iterator it(a);
    return it;
}

// endregion infix_iterators requests implementation

// region prefix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_prefix() noexcept {
    prefix_iterator res(_root);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_prefix() noexcept {
    node *tmp = _root;
    while (tmp->left_subtree != nullptr) {
        while (tmp->right_subtree != nullptr) {
            tmp = tmp->right_subtree;
        }
        if (tmp->left_subtree != nullptr) {
            tmp = tmp->left_subtree;
        }
    }
    prefix_iterator res(nullptr);
    res._backup = tmp;
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_prefix() const noexcept {
    prefix_iterator it(_root);
    prefix_const_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_prefix() const noexcept {
    node *tmp = _root;
    while (tmp->left_subtree != nullptr) {
        while (tmp->right_subtree != nullptr) {
            tmp = tmp->right_subtree;
        }
        if (tmp->left_subtree != nullptr) {
            tmp = tmp->left_subtree;
        }
    }
    prefix_iterator it(nullptr);
    it._backup = tmp;
    prefix_const_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_prefix() const noexcept {
    return begin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_prefix() const noexcept {
    return end_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_prefix() noexcept {
    prefix_iterator it = end_prefix();
    it--;
    prefix_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_prefix() noexcept {
    prefix_iterator it = begin_prefix();
    it--;
    prefix_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_prefix() const noexcept {
    prefix_const_iterator it = end_prefix();
    it--;
    prefix_const_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_prefix() const noexcept {
    prefix_const_iterator it = begin_prefix();
    it--;
    prefix_const_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_prefix() const noexcept {
    return rbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::prefix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_prefix() const noexcept {
    return rend_prefix();
}

// endregion prefix_iterators requests implementation

// region infix_iterators methods implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_infix() noexcept {
    if (_root == nullptr) {
        return infix_iterator(nullptr);
    }
    node *current = _root;
    while (current->left_subtree != nullptr) {
        current = current->left_subtree;
    }
    infix_iterator res(current);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_infix() noexcept {
    infix_iterator it(nullptr);
    node *current = _root;
    if (_root == nullptr) {
        return it;
    } else {
        while (current->right_subtree != nullptr) {
            current = current->right_subtree;
        }
    }
    it._backup = current;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_infix() const noexcept {
    if (_root == nullptr) {
        return infix_iterator(nullptr);
    }
    node *current = _root;
    while (current->left_subtree != nullptr) {
        current = current->left_subtree;
    }
    infix_iterator tmp(current);
    infix_const_iterator res(tmp);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_infix() const noexcept {
    infix_iterator it(nullptr);
    node *current = _root;
    if (_root == nullptr) {
        return it;
    } else {
        while (current->right_subtree != nullptr) {
            current = current->right_subtree;
        }
    }
    it._backup = current;
    infix_const_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_infix() const noexcept {
    return begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_infix() const noexcept {
    return end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_infix() noexcept {
    infix_iterator it = end_infix();
    it--;
    infix_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_infix() noexcept {
    infix_iterator it = begin_infix();
    it--;
    infix_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_infix() const noexcept {
    infix_const_iterator it = end_infix();
    it--;
    infix_const_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_infix() const noexcept {
    infix_const_iterator it = begin_infix();
    it--;
    infix_const_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_infix() const noexcept {
    return rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_infix() const noexcept {
    return rend_infix();
}

// endregion infix_iterators methods implementation

// region postfix_iterators requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_postfix() noexcept {
    node *tmp = _root;
    while (tmp->left_subtree) {
        tmp = tmp->left_subtree;
    }
    while (tmp->right_subtree) {
        tmp = tmp->right_subtree;
    }
    postfix_iterator it(tmp);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_postfix() noexcept {
    postfix_iterator it(nullptr);
    it._backup = _root;
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::begin_postfix() const noexcept {
    node *tmp = _root;
    while (tmp->left_subtree) {
        tmp = tmp->left_subtree;
    }
    while (tmp->right_subtree) {
        tmp = tmp->right_subtree;
    }
    postfix_iterator it(tmp);
    postfix_const_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::end_postfix() const noexcept {
    postfix_iterator it(nullptr);
    it._backup = _root;
    postfix_const_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cbegin_postfix() const noexcept {
    return begin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_iterator
binary_search_tree<tkey, tvalue, compare, tag>::cend_postfix() const noexcept {
    return end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_postfix() noexcept {
    postfix_iterator it = end_postfix();
    it--;
    postfix_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_postfix() noexcept {
    postfix_iterator it = begin_postfix();
    it--;
    postfix_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rbegin_postfix() const noexcept {
    postfix_const_iterator it = end_postfix();
    it--;
    postfix_const_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::rend_postfix() const noexcept {
    postfix_const_iterator it = begin_postfix();
    it--;
    postfix_const_reverse_iterator res(it);
    return res;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crbegin_postfix() const noexcept {
    return rbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::postfix_const_reverse_iterator
binary_search_tree<tkey, tvalue, compare, tag>::crend_postfix() const noexcept {
    rend_postfix();
}

// endregion postfix_iterators requests implementation

// endregion iterators requests implementation

// region subtree rotations implemention

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::small_left_rotation(node *&subtree_root) noexcept {
    //     a                       c
    //   b    c      ----->     a   e
    //       d e              b  d
    if (subtree_root == nullptr) {
        return;
    }

    if (subtree_root->right_subtree == nullptr) {
        return;
    }


    node *a = subtree_root;
    node *b = a->left_subtree;
    node *c = a->right_subtree;
    node *d = c->left_subtree;
    node *e = c->right_subtree;

    if (a->parent){
        if (a == a->parent->left_subtree){
            a->parent->left_subtree = c;
        } else {
            a->parent->right_subtree = c;
        }
    }

    c->left_subtree = a;
    c->parent = a->parent;
    if (a != nullptr) {
        a->parent = c;
    }
    c->right_subtree = e;
    if (e != nullptr) {
        e->parent = c;
    }

    a->left_subtree = b;
    if (b != nullptr) {
        b->parent = a;
    }

    a->right_subtree = d;
    if (d != nullptr) {
        d->parent = a;
    }
    subtree_root = c;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::small_right_rotation(node *&subtree_root) noexcept {
    //     a                      b
    //   b    c      ----->     d   a
    //  d e                        e  c
    if (subtree_root == nullptr) {
        return;
    }

    if (subtree_root->left_subtree == nullptr) {
        return;
    }


    node *a = subtree_root;

    node *b = a->left_subtree;
    node *c = a->right_subtree;
    node *d = b->left_subtree;
    node *e = b->right_subtree;

    if (a->parent){
        if (a == a->parent->left_subtree){
            a->parent->left_subtree = b;
        } else {
            a->parent->right_subtree = b;
        }
    }

    b->left_subtree = d;
    b->parent = a->parent;
    if (d != nullptr) {
        d->parent = b;
    }
    b->right_subtree = a;
    if (a != nullptr) {
        a->parent = b;
    }

    a->left_subtree = e;
    if (e != nullptr) {
        e->parent = a;
    }

    a->right_subtree = c;
    if (c != nullptr) {
        c->parent = a;
    }
    subtree_root = b;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::big_left_rotation(node *&subtree_root) noexcept {
    if (subtree_root->right_subtree == nullptr) {
        return;
    }
    small_right_rotation(subtree_root->right_subtree);
    small_left_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::big_right_rotation(node *&subtree_root) noexcept {
    if (subtree_root->left_subtree == nullptr) {
        return;
    }
    small_left_rotation(subtree_root->left_subtree);
    small_right_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::double_left_rotation(node *&subtree_root) noexcept {
    if (subtree_root->right_subtree == nullptr || subtree_root->right_subtree->right_subtree == nullptr) {
        return;
    }

    small_left_rotation(subtree_root);
    small_left_rotation(subtree_root);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
void binary_search_tree<tkey, tvalue, compare, tag>::double_right_rotation(node *&subtree_root) noexcept {
    if (subtree_root->left_subtree == nullptr || subtree_root->left_subtree->left_subtree == nullptr) {
        return;
    }

    small_right_rotation(subtree_root);
    small_right_rotation(subtree_root);
}

// endregion

namespace __detail {
    template<typename tkey, typename tvalue, typename compare, typename tag>
    template<typename ...Args>
    typename binary_search_tree<tkey, tvalue, compare, tag>::node *
    bst_impl<tkey, tvalue, compare, tag>::create_node(binary_search_tree<tkey, tvalue, compare, tag> &cont,
                                                      Args &&...args) {
        typename binary_search_tree<tkey, tvalue, compare, tag>::node *n = cont._allocator.template new_object<typename binary_search_tree<tkey, tvalue, compare, tag>::node>(
                args ...);
        return n;
    }

    template<typename tkey, typename tvalue, typename compare, typename tag>
    void bst_impl<tkey, tvalue, compare, tag>::delete_node(binary_search_tree<tkey, tvalue, compare, tag> &cont,
                                                           binary_search_tree<tkey, tvalue, compare, tag>::node *n) {
        cont._allocator.delete_object(n);
    }

    template<typename tkey, typename tvalue, typename compare, typename tag>
    void bst_impl<tkey, tvalue, compare, tag>::erase(binary_search_tree<tkey, tvalue, compare, tag> &cont,
                                                     typename binary_search_tree<tkey, tvalue, compare, tag>::node **node_ptr) {

        auto *node_to_delete = *node_ptr;
        if (node_to_delete == nullptr) {
            throw std::out_of_range("Incorrect iterator for erase\n");
        }

        //node* prev = (std::prev(pos)).get_node();
        auto *parent = node_to_delete->parent;
        typename binary_search_tree<tkey, tvalue, compare, tag>::node *new_node = nullptr;
        if ((node_to_delete->left_subtree == nullptr) && (node_to_delete->right_subtree == nullptr)) {
            if (parent->left_subtree == node_to_delete) {
                parent->left_subtree = nullptr;
            } else {
                parent->right_subtree = nullptr;
            }
        } else if (node_to_delete->left_subtree != nullptr && node_to_delete->right_subtree == nullptr) {
            new_node = node_to_delete->left_subtree;
            if (parent->left_subtree == node_to_delete) {
                parent->left_subtree = new_node;
                new_node->parent = node_to_delete->parent;
            } else {
                parent->right_subtree = new_node;
                new_node->parent = node_to_delete->parent;
            }
        } else if (node_to_delete->left_subtree == nullptr && node_to_delete->right_subtree != nullptr) {
            new_node = node_to_delete->right_subtree;
            if (parent != nullptr) {
                if (parent->left_subtree == node_to_delete) {
                    parent->left_subtree = new_node;
                } else {
                    parent->right_subtree = new_node;
                }
            } else {
                cont._root = new_node;
            }
            new_node->parent = node_to_delete->parent;
        } else {
            typename binary_search_tree<tkey, tvalue, compare, tag>::node *successor;

            successor = node_to_delete->left_subtree;
            while (successor->right_subtree) {
                successor = successor->right_subtree;
            }

            new_node = successor;
            if (node_to_delete->parent != nullptr) {
                if (node_to_delete->parent->right_subtree == node_to_delete) {
                    node_to_delete->parent->right_subtree = new_node;
                } else {
                    node_to_delete->parent->left_subtree = new_node;
                }
            }

            if (new_node->parent != nullptr) {
                if (new_node->parent->right_subtree == new_node) {
                    new_node->parent->right_subtree = new_node->left_subtree;
                } else {
                    node_to_delete->parent->left_subtree = new_node->left_subtree;
                }
            }

            //Managing case when new_node is child f node_to_delete
            if (new_node->parent == node_to_delete) {
                new_node->right_subtree = node_to_delete->right_subtree;
            } else {
                new_node->left_subtree = node_to_delete->left_subtree;
                new_node->right_subtree = node_to_delete->right_subtree;
            }


            new_node->parent = node_to_delete->parent;

            if (new_node->left_subtree) {
                new_node->left_subtree->parent = new_node;
            }
            if (new_node->right_subtree) {
                new_node->right_subtree->parent = new_node;
            }
        }

        if (node_to_delete->parent == nullptr) {
            cont._root = new_node;
        }
        cont._size--;
        __detail::bst_impl<tkey, tvalue, compare, tag>::delete_node(cont, node_to_delete);


        *node_ptr = new_node;
    }
}

//region 5_rules_implemetation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(const binary_search_tree &other) {
    prefix_const_iterator iter_other = other.begin_prefix();

    while (iter_other != other.end_prefix()) {
        insert(*iter_other);
        iter_other++;

    }

    *_logger = *other._logger;
    _allocator = other._allocator;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(binary_search_tree &&other) noexcept {
    _root = other._root;
    _logger = other._logger;
    _allocator = other._allocator;
    _size = other._size;
    other._root = nullptr;
    other._size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag> &
binary_search_tree<tkey, tvalue, compare, tag>::operator=(const binary_search_tree &other) {
    if (this == &other)
        return *this;

    while (size() != 0) {
        erase(begin_infix());
    }

    prefix_const_iterator iter_other = other.begin_prefix();

    while (iter_other != other.end_prefix()) {
        insert(*iter_other);
        iter_other++;
    }

    *_logger = *other._logger;
    _allocator = other._allocator;
    return *this;
}


template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag> &
binary_search_tree<tkey, tvalue, compare, tag>::operator=(binary_search_tree &&other) noexcept {
    if (this == &other)
        return *this;

    while (size() != 0) {
        erase(begin_infix());
    }

    _root = other._root;
    _logger = other._logger;
    _allocator = other._allocator;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::~binary_search_tree() {
    std::vector<node *> vec;
    infix_iterator it = begin();
    while (it != end()) {
        vec.push_back(it.get_node());
        it++;
    }
    for (node *n: vec) {
        __detail::bst_impl<tkey, tvalue, compare, tag>::delete_node(*this, n);
    }
}

//endregion 5_rules_implemetation

//region binary_search_tree methods_insert and methods_emplace implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::get_next_prefix(node *n) {
    if (n == nullptr) {
        return nullptr;
    }
    if (n->left_subtree != nullptr) {
        return n->left_subtree;
    } else if (n->right_subtree != nullptr) {
        return n->right_subtree;
    }

    node *parent = n->parent;
    if (n == parent->left_subtree) {
        //we dont have any childs and we are left subtree.
        // This means that we have traversed all left subtree of our parent and now we have to go to the right subtree of one of our parents
        node *tmp = parent;
        node *prev = n;
        bool from_left = false;
        while (tmp) {
            //if node doesnt have right_subtree, then traversal for it is finished and we need to go upper
            if (tmp->left_subtree == prev && tmp->right_subtree != nullptr) {
                from_left = true;
                break;
            }
            prev = tmp;
            tmp = tmp->parent;
        }

        //if we havent found any suitable nodes it means that traversal is finished
        if (!from_left) {
            return nullptr;
        } else {
            return tmp->right_subtree;
        }
    } else if (n == parent->right_subtree) {
        node *tmp = parent;
        node *prev = n;
        bool found = false;
        //we are managing situation like this:
        //
        //            pparent
        //   parent           child_pparent
        //      we_here
        // then child_pparent is the next node

        while (tmp->parent) {
            prev = tmp;
            tmp = tmp->parent;
            if (tmp->right_subtree != prev && tmp->right_subtree != nullptr) {
                found = true;
                break;
            }
        }

        if (!found) {
            return nullptr;
        } else {
            return tmp->right_subtree;
        }
        return nullptr;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::get_prev_prefix(node *n) {
    if (n == nullptr) {
        return nullptr;
    }
    if (n->parent == nullptr) {
        //we are root. root is the first node in traversal
        return nullptr;
    }

    node *parent = n->parent;
    if (n == parent->left_subtree) {
        return parent;
    } else if (n == parent->right_subtree) {
        node *tmp = parent;
        //prev node is the most right node in parent left subtree
        while (tmp->left_subtree) {
            tmp = tmp->left_subtree;
            while (tmp->right_subtree) {
                tmp = tmp->right_subtree;
            }
        }
        return tmp;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::get_next_infix(node *n) {
    if (n == nullptr) {
        return nullptr;
    }
    //specific logic for root
    if (n->parent == nullptr) {
        if (n->right_subtree != nullptr) {
            node *tmp = n->right_subtree;
            while (tmp->left_subtree) {
                tmp = tmp->left_subtree;
            }
            return tmp;
        } else {
            return nullptr;
        }
    }

    node *parent = n->parent;
    if (n->right_subtree == nullptr && n == n->parent->left_subtree) {
        return parent;
    }

    if (n->right_subtree != nullptr) {
        node *tmp = n->right_subtree;
        while (tmp->left_subtree) {
            tmp = tmp->left_subtree;
        }
        return tmp;
    }

    //managing situation like this:
    //             pparent
    //   parent               pparent_child
    //      we_here     left_subtree   right_subtree
    //
    //we have finished traversal of left_subtree of pparent, so now we should go to it

    node *tmp = n;
    bool from_left = false;
    while (tmp->parent) {
        if (tmp->parent->left_subtree == tmp) {
            from_left = true;
            break;
        } else {
            tmp = tmp->parent;
        }
    }

    if (from_left) {
        return tmp->parent;
    }
    //if no suitable nodes found then traversal is ended
    return nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::get_prev_infix(node *n) {
    if (n == nullptr) {
        return nullptr;
    }

    if (n->left_subtree != nullptr) {
        //managing situation like this:
        //             n
        //     p1          pparent_child
        //  ch2  ch3
        // ch3 is previous node to n

        node *tmp = n;
        tmp = tmp->left_subtree;
        while (tmp->right_subtree) {
            tmp = tmp->right_subtree;
        }
        return tmp;
    } else {
        if (n->parent == nullptr) {
            return nullptr;
        }

        if (n->parent->right_subtree == n) {
            return n->parent;
        } else {
            //we are in the right subtree of some node (lets call it subtree_root). The previous node is subtree_root
            node *tmp = n;
            while (tmp->parent) {
                if (tmp->parent->right_subtree == tmp) {
                    return tmp->parent;
                } else {
                    tmp = tmp->parent;
                }
            }
            return nullptr;
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::get_next_postfix(node *n) {
    if (n == nullptr) {
        return nullptr;
    }

    if (n->parent == nullptr) {
        return nullptr;
    }

    if (n == n->parent->right_subtree || n->parent->right_subtree == nullptr) {
        return n->parent;
    }

    node *tmp = n->parent->right_subtree;
    while (tmp->left_subtree) {
        tmp = tmp->left_subtree;
    }

    while (tmp->right_subtree) {
        tmp = tmp->right_subtree;
    }

    return tmp;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::node *
binary_search_tree<tkey, tvalue, compare, tag>::get_prev_postfix(node *n) {
    if (n->right_subtree) {
        return n->right_subtree;
    }

    if (n->left_subtree) {
        return n->left_subtree;
    }

    if (n->parent == nullptr) {
        return nullptr;
    }

    if (n->parent->left_subtree == n) {
        node *tmp = n;
        while (tmp->parent) {
            if (tmp->parent->right_subtree == tmp && tmp->parent->left_subtree != nullptr) {
                return tmp->parent->left_subtree;
            } else {
                tmp = tmp->parent;
            }
        }
    } else {
        return n->parent->left_subtree;
    }
    return nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::insert(const value_type &value) {
    node *prev = nullptr;
    node *current = _root;
    while (current != nullptr) {
        if (compare_keys(value.first, current->data.first)) {
            prev = current;
            current = current->left_subtree;
        } else {
            prev = current;
            current = current->right_subtree;
        }
    }
    node *new_node;

    new_node = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, value.first, value.second);
    infix_iterator itt(new_node);
    if (new_node == nullptr) {
        return std::pair<infix_iterator, bool>(itt, false);
    }
    if (prev == nullptr) {
        _root = new_node;
        __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, &new_node);
        return std::pair<infix_iterator, bool>(itt, true);
    }

    infix_iterator it(new_node);
    if (compare_keys(value.first, prev->data.first)) {
        prev->left_subtree = new_node;
    } else {
        prev->right_subtree = new_node;
    }
    _size++;
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, &new_node);
    return std::pair<infix_iterator, bool>(it, true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::insert(value_type &&value) {
    node *prev = NULL;
    node *current = _root;
    while (current != nullptr) {
        if (compare_keys(value.first, current->data.first)) {
            prev = current;
            current = current->left_subtree;
        } else {
            prev = current;
            current = current->right_subtree;
        }
    }
    node *new_node;

    //differs from default insert only here
    new_node = __detail::bst_impl<tkey, tvalue, compare, tag>::create_node(*this, prev, std::move(value.first), std::move(value.second));
    infix_iterator itt(new_node);
    if (new_node == nullptr) {
        return std::pair<infix_iterator, bool>(itt, false);
    }

    if (prev == nullptr){
        _root = new_node;
        __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, &new_node);
        return std::pair<infix_iterator, bool>(itt, true);
    }

    infix_iterator it(new_node);
    if (compare_keys(value.first, prev->data.first)) {
        prev->left_subtree = new_node;
    } else {
        prev->right_subtree = new_node;
    }
    __detail::bst_impl<tkey, tvalue, compare, tag>::post_insert(*this, &new_node);
    _size++;
    return std::pair<infix_iterator, bool>(it, true);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::input_iterator InputIt>
void binary_search_tree<tkey, tvalue, compare, tag>::insert(InputIt first, InputIt last) {
    InputIt it = first;
    while (it != last) {
        insert((*it));
        it++;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::ranges::input_range R>
void binary_search_tree<tkey, tvalue, compare, tag>::insert_range(R &&rg) {
    for (const auto key: rg) {
        insert(std::move(key));
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
std::pair<typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator, bool>
binary_search_tree<tkey, tvalue, compare, tag>::emplace(Args &&... args) {
    value_type n(args ...);
    return this->insert(std::move(n));
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(const value_type &value) {
    node *n = go_to_node_with_key(value.first);
    if (n == nullptr) {
        return insert(value).first;
    } else {
        n->data.second = value.second;
    }
    infix_iterator it(n);
    return it;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(value_type &&value) {
    node *n = go_to_node_with_key(value->first);
    if (n == NULL) {
        insert(std::forward(value));
    } else {
        n->data.second = value.second;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::input_iterator InputIt>
void binary_search_tree<tkey, tvalue, compare, tag>::insert_or_assign(InputIt first, InputIt last) {
    InputIt it = first;
    while (it != last) {
        insert_or_assign((*it));
        it++;
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<class ...Args>
typename binary_search_tree<tkey, tvalue, compare, tag>::infix_iterator
binary_search_tree<tkey, tvalue, compare, tag>::emplace_or_assign(Args &&... args) {
    value_type n(args ...);
    return insert_or_assign(n);
}

//endregion binary_search_tree methods_insert and methods_emplace implementation

//region binary_search_tree implementation

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        const compare &comp,
        pp_allocator<value_type> alloc,
        logger *logger) {
    _root = nullptr;
    _allocator = alloc;
    _logger = logger;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        pp_allocator<value_type> alloc,
        const compare &comp,
        logger *logger) {
    _root = nullptr;
    _allocator = alloc;
    _logger = logger;
    _size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
template<std::ranges::input_range Range>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        Range &&range,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *logger) {
    _allocator = alloc;
    _logger = logger;
    _size = 0;
    _root = nullptr;
    insert_range(range);
}

template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
binary_search_tree<tkey, tvalue, compare, tag>::binary_search_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *logger) {
    _allocator = alloc;
    _logger = logger;
    _size = 0;
    _root = nullptr;
    for (std::pair<tkey, tvalue> p: data) {
        insert(p);
    }
}

//endregion binary_search_tree methods_insert and methods_emplace implementation

//return true if lhs < rhs
//I do not know if this is what it is supposed to be.
template<typename tkey, typename tvalue, compator<tkey> compare, typename tag>
bool binary_search_tree<tkey, tvalue, compare, tag>::compare_keys(const tkey &lhs, const tkey &rhs) const {
    return compare()(lhs, rhs);
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_BINARY_SEARCH_TREE_H;

