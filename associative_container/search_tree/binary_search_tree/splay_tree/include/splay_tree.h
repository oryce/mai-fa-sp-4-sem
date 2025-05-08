#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H

#include <binary_search_tree.h>

namespace __detail {
    class SPL_TAG;

    template<typename tkey, typename tvalue, typename compare>

    class bst_impl<tkey, tvalue, compare, SPL_TAG>
    {
    public:
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node *
        create_node(binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont, Args &&...args);

        static void delete_node(binary_search_tree<tkey, tvalue, compare, SPL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node*);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node**);

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont,
                                binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node **);

        static void erase(binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont,
                          binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node **);

        static void swap(binary_search_tree<tkey, tvalue, compare, SPL_TAG> &lhs,
                         binary_search_tree<tkey, tvalue, compare, SPL_TAG> &rhs) noexcept;
    };
}

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>>
class splay_tree final : public binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG> {

    using parent = binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>;
public:

    using value_type = parent::value_type;

    explicit splay_tree(
            const compare &comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit splay_tree(
            pp_allocator<value_type> alloc,
            const compare &comp = compare(),
            logger *log = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit splay_tree(iterator begin, iterator end, const compare &cmp = compare(),
                        pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                        logger *log = nullptr);

    template<std::ranges::input_range Range>
    explicit splay_tree(Range &&range, const compare &cmp = compare(),
                        pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                        logger *log = nullptr);


    splay_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(),
               pp_allocator<value_type> alloc = pp_allocator<value_type>(),
               logger *log = nullptr);

public:

    ~splay_tree() noexcept final;

    splay_tree(splay_tree const &other);

    splay_tree &operator=(splay_tree const &other);

    splay_tree(splay_tree &&other) noexcept;

    splay_tree &operator=(splay_tree &&other) noexcept;

public:
    std::pair<typename binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::infix_iterator, bool>
    insert(const value_type &);

    std::pair<typename binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::infix_iterator, bool>
    insert(value_type &&);

public:
    void splay(parent::node *);

    parent::node *merge(parent::node *root1, parent::node *root2);

    std::pair<typename parent::node *, typename parent::node *> split(const tkey key);
};

// region other

template<typename compare, typename U, typename iterator>
explicit splay_tree(iterator begin, iterator end, const compare &cmp = compare(),
                    pp_allocator<U> alloc = pp_allocator<U>(),
                    logger *log = nullptr) -> splay_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit splay_tree(Range &&range, const compare &cmp = compare(),
                    pp_allocator<U> alloc = pp_allocator<U>(),
                    logger *log = nullptr) -> splay_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare>;

template<typename tkey, typename tvalue, typename compare, typename U>
splay_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(),
           pp_allocator<U> alloc = pp_allocator<U>(),
           logger *log = nullptr) -> splay_tree<tkey, tvalue, compare>;
//endregion other

// region implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(
        const compare &comp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>(comp, alloc, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(
        pp_allocator<value_type> alloc,
        const compare &comp,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>(alloc, comp, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<input_iterator_for_pair<tkey, tvalue> iterator>
splay_tree<tkey, tvalue, compare>::splay_tree(
        iterator begin,
        iterator end,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>(begin, end, cmp, alloc) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<std::ranges::input_range Range>
splay_tree<tkey, tvalue, compare>::splay_tree(
        Range &&range,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>(range, cmp, alloc, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>(data, cmp, alloc) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::~splay_tree() noexcept {
    //pass
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(splay_tree const &other) {
    while (parent::size() != 0) {
        parent::erase(parent::begin_infix());
    }

    typename parent::prefix_const_iterator iter_other = other.begin_prefix();

    while (iter_other != other.end_prefix()) {
        parent::insert(*iter_other);
        iter_other++;
    }

    *parent::_logger = *other._logger;
    parent::_allocator = other._allocator;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare> &splay_tree<tkey, tvalue, compare>::operator=(splay_tree const &other) {
    if (this == &other)
        return *this;

    while (parent::size() != 0) {
        parent::erase(parent::begin_infix());
    }

    typename parent::prefix_const_iterator iter_other = other.begin_prefix();

    while (iter_other != other.end_prefix()) {
        parent::insert(*iter_other);
        iter_other++;
    }

    *parent::_logger = *other._logger;
    parent::_allocator = other._allocator;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare>::splay_tree(splay_tree &&other) noexcept {
    parent::_root = other._root;
    parent::_logger = other._logger;
    parent::_allocator = other._allocator;
    parent::_size = other._size;
    other._root = nullptr;
    other._size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
splay_tree<tkey, tvalue, compare> &splay_tree<tkey, tvalue, compare>::operator=(splay_tree &&other) noexcept {
    if (this == &other)
        return *this;

    while (parent::size() != 0) {
        parent::erase(parent::begin_infix());
    }

    parent::_root = other._root;
    parent::_logger = other._logger;
    parent::_allocator = other._allocator;
    return *this;
}

// endregion implementation

// region bst_impl implementation
namespace __detail {
    template<typename tkey, typename tvalue, typename compare>
    template<class ...Args>
    binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node *
    bst_impl<tkey, tvalue, compare, SPL_TAG>::create_node(
            binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont,
            Args &&...args) {
        typename binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node *n = cont._allocator.new_object<typename binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node>(
                args ...);
        return n;
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPL_TAG>::delete_node(
            binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont,
            binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node *node) {
        cont._allocator.delete_object(node);
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPL_TAG>::post_search(
            binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node **node_ptr) {
        splay(*node_ptr);
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPL_TAG>::post_insert(
            binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont,
            binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node **node_ptr) {
        dynamic_cast<splay_tree<tkey, tvalue, compare> &>(cont).splay(*node_ptr);
        cont._root = *node_ptr;
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPL_TAG>::erase(
            binary_search_tree<tkey, tvalue, compare, SPL_TAG> &cont,
            binary_search_tree<tkey, tvalue, compare, SPL_TAG>::node **node_ptr) {
        auto n = *node_ptr;
        auto &spl = dynamic_cast<splay_tree<tkey, tvalue, compare> &>(cont);
        spl.splay(n);
        cont._root = spl.merge(n->left_subtree, n->right_subtree);
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPL_TAG>::swap(
            binary_search_tree<tkey, tvalue, compare, SPL_TAG> &lhs,
            binary_search_tree<tkey, tvalue, compare, SPL_TAG> &rhs) noexcept {

    }

}
// endregion bst_impl implementation

// region special splay_tree functions

//Перемещение элемента х в корень
template<typename tkey, typename tvalue, compator<tkey> compare>
void splay_tree<tkey, tvalue, compare>::splay(binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node *x) {
    //Используется инфа и обозначения из википедии
    //https://ru.wikipedia.org/wiki/Splay-%D0%B4%D0%B5%D1%80%D0%B5%D0%B2%D0%BE
    using node = binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node;
    if (x == nullptr) {
        return;
    }

    while (x->parent != nullptr) {
        node *p = x ? x->parent : nullptr;
        node *g = p ? p->parent : nullptr;
        if (g == nullptr) {
            if (x == p->left_subtree) {
                parent::small_right_rotation(p);
            } else {
                parent::small_left_rotation(p);
            }
            //Обновляем корень
            if (p->parent == nullptr) {
                parent::_root = p;
            }
        } else {
            bool x_is_left = x == p->left_subtree;
            bool p_is_left = p == g->left_subtree;
            if (x_is_left == p_is_left) {
                if (x_is_left) {
                    parent::double_right_rotation(g);
                } else {
                    parent::double_left_rotation(g);
                }
            } else {
                if (!x_is_left && p_is_left) {
                    parent::big_right_rotation(g);
                } else {
                    parent::big_left_rotation(g);
                }
            }
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare>
binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node *
splay_tree<tkey, tvalue, compare>::merge(binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node *t1,
                                         binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node *t2) {
    using node = binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node;
    if (t1 == nullptr) {
        return t2;
    }
    if (t2 == nullptr) {
        return t1;
    }

    t1->parent = nullptr;
    t2->parent = nullptr;

    node *prev = t1;
    node *mx = t1;
    while (mx->right_subtree) {
        mx = mx->right_subtree;
        prev = mx;
    }
    splay(mx);
    prev->right_subtree = t2;
    t2->parent = prev;
    return t1;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
std::pair<typename binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node *, typename binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node *>
splay_tree<tkey, tvalue, compare>::split(
        const tkey key) {
    using node = binary_search_tree<tkey, tvalue, compare, __detail::SPL_TAG>::node;
    auto it = parent::lower_bound(key);
    node *n = it.get_node();
    splay(n);
    node *left_child = parent::_root->left_subtree;
    parent::_root->left_subtree = nullptr;
    left_child->parent = nullptr;
    return {parent::_root, left_child};
}


// endregion special splay_tree functions


#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SPLAY_TREE_H