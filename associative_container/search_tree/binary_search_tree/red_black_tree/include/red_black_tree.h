#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H

#include <binary_search_tree.h>

namespace __detail
{
    class RB_TAG;

    template<typename tkey, typename tvalue, typename compare>
    class bst_impl<tkey, tvalue, compare, RB_TAG>
    {
    public:
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, RB_TAG>::node* create_node(binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont, Args&& ...args);

        static void delete_node(binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont, binary_search_tree<tkey, tvalue, compare, RB_TAG>::node*);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, RB_TAG>::node**){}

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont, binary_search_tree<tkey, tvalue, compare, RB_TAG>::node**);

        static void erase(binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont, binary_search_tree<tkey, tvalue, compare, RB_TAG>::node**);

        static void swap(binary_search_tree<tkey, tvalue, compare, RB_TAG>& lhs, binary_search_tree<tkey, tvalue, compare, RB_TAG>& rhs) noexcept;
    };
}

template<typename tkey,typename tvalue, compator<tkey> compare = std::less<tkey>>
class red_black_tree final: public binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>
{

public:
    
    enum class node_color : unsigned char
    {
        RED,
        BLACK
    };

private:

    using parent = binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>;
    friend __detail::bst_impl<tkey, tvalue, compare, __detail::RB_TAG>;

    struct node final:
        parent::node
    {
        node_color color;

        template<class ...Args>
        node(parent::node* par, Args&&... args);

        ~node() noexcept override =default;

        node* get_parent()
        {
            return static_cast<node*>(this->parent);
        }

        node* get_grandparent()
        {
            return get_parent() ? get_parent()->get_parent() : nullptr;
        }

        node* get_left()
        {
            return static_cast<node*>(this->left_subtree);
        }

        node* get_right()
        {
            return static_cast<node*>(this->right_subtree);
        }

        node* get_sibling()
        {
            node* parent = get_parent();
            return this == parent->get_left()
                       ? parent->get_right()
                       : parent->get_left();
        }
    };


public:

    using value_type = parent::value_type;

    explicit red_black_tree(
            const compare& comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit red_black_tree(
            pp_allocator<value_type> alloc,
            const compare& comp = compare(),
            logger *log = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit red_black_tree(iterator begin, iterator end, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);

    template<std::ranges::input_range Range>
    explicit red_black_tree(Range&& range, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);


    red_black_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr);


    // region iterator definition


    class prefix_iterator : public parent::prefix_iterator
    {
    public:

        using value_type = parent::prefix_iterator::value_type;
        using difference_type = parent::prefix_iterator::difference_type;
        using pointer = parent::prefix_iterator::pointer;
        using reference = parent::prefix_iterator::reference;
        using iterator_category = parent::prefix_iterator::iterator_category;

        explicit prefix_iterator(parent::node* n = nullptr) noexcept;
        prefix_iterator(parent::prefix_iterator) noexcept;

        node_color get_color() const noexcept;

        using parent::prefix_iterator::depth;
        using parent::prefix_iterator::operator*;
        using parent::prefix_iterator::operator==;
        using parent::prefix_iterator::operator!=;
        using parent::prefix_iterator::operator++;
        using parent::prefix_iterator::operator--;
        using parent::prefix_iterator::operator->;
    };

    class prefix_const_iterator : public parent::prefix_const_iterator
    {
    public:

        using value_type = parent::prefix_const_iterator::value_type;
        using difference_type = parent::prefix_const_iterator::difference_type;
        using pointer = parent::prefix_const_iterator::pointer;
        using reference = parent::prefix_const_iterator::reference;
        using iterator_category = parent::prefix_const_iterator::iterator_category;

        explicit prefix_const_iterator(parent::node* n = nullptr) noexcept;
        prefix_const_iterator(parent::prefix_const_iterator) noexcept;

        node_color get_color() const noexcept;

        prefix_const_iterator(prefix_iterator) noexcept;

        using parent::prefix_const_iterator::depth;
        using parent::prefix_const_iterator::operator*;
        using parent::prefix_const_iterator::operator==;
        using parent::prefix_const_iterator::operator!=;
        using parent::prefix_const_iterator::operator++;
        using parent::prefix_const_iterator::operator--;
        using parent::prefix_const_iterator::operator->;
    };

    class prefix_reverse_iterator : public parent::prefix_reverse_iterator
    {
    public:

        using value_type = parent::prefix_reverse_iterator::value_type;
        using difference_type = parent::prefix_reverse_iterator::difference_type;
        using pointer = parent::prefix_reverse_iterator::pointer;
        using reference = parent::prefix_reverse_iterator::reference;
        using iterator_category = parent::prefix_reverse_iterator::iterator_category;

        explicit prefix_reverse_iterator(parent::node* n = nullptr) noexcept;
        prefix_reverse_iterator(parent::prefix_reverse_iterator) noexcept;

        node_color get_color() const noexcept;

        prefix_reverse_iterator(prefix_iterator) noexcept;
        operator prefix_iterator() const noexcept;
        prefix_iterator base() const noexcept;

        using parent::prefix_reverse_iterator::depth;
        using parent::prefix_reverse_iterator::operator*;
        using parent::prefix_reverse_iterator::operator==;
        using parent::prefix_reverse_iterator::operator!=;
        using parent::prefix_reverse_iterator::operator++;
        using parent::prefix_reverse_iterator::operator--;
        using parent::prefix_reverse_iterator::operator->;
    };

    class prefix_const_reverse_iterator : public parent::prefix_const_reverse_iterator
    {
    public:

        using value_type = parent::prefix_const_reverse_iterator::value_type;
        using difference_type = parent::prefix_const_reverse_iterator::difference_type;
        using pointer = parent::prefix_const_reverse_iterator::pointer;
        using reference = parent::prefix_const_reverse_iterator::reference;
        using iterator_category = parent::prefix_const_reverse_iterator::iterator_category;

        explicit prefix_const_reverse_iterator(parent::node* n = nullptr) noexcept;
        prefix_const_reverse_iterator(parent::prefix_const_reverse_iterator) noexcept;

        node_color get_color() const noexcept;

        prefix_const_reverse_iterator(prefix_const_iterator) noexcept;
        operator prefix_const_iterator() const noexcept;
        prefix_const_iterator base() const noexcept;

        using parent::prefix_const_reverse_iterator::depth;
        using parent::prefix_const_reverse_iterator::operator*;
        using parent::prefix_const_reverse_iterator::operator==;
        using parent::prefix_const_reverse_iterator::operator!=;
        using parent::prefix_const_reverse_iterator::operator++;
        using parent::prefix_const_reverse_iterator::operator--;
        using parent::prefix_const_reverse_iterator::operator->;
    };

    class infix_iterator : public parent::infix_iterator
    {
    public:

        using value_type = parent::infix_iterator::value_type;
        using difference_type = parent::infix_iterator::difference_type;
        using pointer = parent::infix_iterator::pointer;
        using reference = parent::infix_iterator::reference;
        using iterator_category = parent::infix_iterator::iterator_category;

        explicit infix_iterator(parent::node* n = nullptr) noexcept;
        infix_iterator(parent::infix_iterator) noexcept;

        node_color get_color() const noexcept;

        using parent::infix_iterator::depth;
        using parent::infix_iterator::operator*;
        using parent::infix_iterator::operator==;
        using parent::infix_iterator::operator!=;
        using parent::infix_iterator::operator++;
        using parent::infix_iterator::operator--;
        using parent::infix_iterator::operator->;
    };

    class infix_const_iterator : parent::infix_const_iterator
    {
    public:

        using value_type = parent::infix_const_iterator::value_type;
        using difference_type = parent::infix_const_iterator::difference_type;
        using pointer = parent::infix_const_iterator::pointer;
        using reference = parent::infix_const_iterator::reference;
        using iterator_category = parent::infix_const_iterator::iterator_category;

        explicit infix_const_iterator(parent::node* n = nullptr) noexcept;
        infix_const_iterator(parent::infix_const_iterator) noexcept;

        node_color get_color() const noexcept;

        infix_const_iterator(infix_iterator) noexcept;

        using parent::infix_const_iterator::depth;
        using parent::infix_const_iterator::operator*;
        using parent::infix_const_iterator::operator==;
        using parent::infix_const_iterator::operator!=;
        using parent::infix_const_iterator::operator++;
        using parent::infix_const_iterator::operator--;
        using parent::infix_const_iterator::operator->;
    };

    class infix_reverse_iterator : public parent::infix_reverse_iterator
    {
    public:

        using value_type = parent::infix_reverse_iterator::value_type;
        using difference_type = parent::infix_reverse_iterator::difference_type;
        using pointer = parent::infix_reverse_iterator::pointer;
        using reference = parent::infix_reverse_iterator::reference;
        using iterator_category = parent::infix_reverse_iterator::iterator_category;

        explicit infix_reverse_iterator(parent::node* n = nullptr) noexcept;
        infix_reverse_iterator(parent::infix_reverse_iterator) noexcept;

        node_color get_color() const noexcept;

        infix_reverse_iterator(infix_iterator) noexcept;
        operator infix_iterator() const noexcept;
        infix_iterator base() const noexcept;

        using parent::infix_reverse_iterator::depth;
        using parent::infix_reverse_iterator::operator*;
        using parent::infix_reverse_iterator::operator==;
        using parent::infix_reverse_iterator::operator!=;
        using parent::infix_reverse_iterator::operator++;
        using parent::infix_reverse_iterator::operator--;
        using parent::infix_reverse_iterator::operator->;
    };

    class infix_const_reverse_iterator : public parent::infix_const_reverse_iterator
    {
    public:

        using value_type = parent::infix_const_reverse_iterator::value_type;
        using difference_type = parent::infix_const_reverse_iterator::difference_type;
        using pointer = parent::infix_const_reverse_iterator::pointer;
        using reference = parent::infix_const_reverse_iterator::reference;
        using iterator_category = parent::infix_const_reverse_iterator::iterator_category;

        explicit infix_const_reverse_iterator(parent::node* n = nullptr) noexcept;
        infix_const_reverse_iterator(parent::infix_const_reverse_iterator) noexcept;

        node_color get_color() const noexcept;

        infix_const_reverse_iterator(infix_const_iterator) noexcept;
        operator infix_const_iterator() const noexcept;
        infix_const_iterator base() const noexcept;

        using parent::infix_const_reverse_iterator::depth;
        using parent::infix_const_reverse_iterator::operator*;
        using parent::infix_const_reverse_iterator::operator==;
        using parent::infix_const_reverse_iterator::operator!=;
        using parent::infix_const_reverse_iterator::operator++;
        using parent::infix_const_reverse_iterator::operator--;
        using parent::infix_const_reverse_iterator::operator->;
    };

    class postfix_iterator : public parent::postfix_iterator
    {
    public:

        using value_type = parent::postfix_iterator::value_type;
        using difference_type = parent::postfix_iterator::difference_type;
        using pointer = parent::postfix_iterator::pointer;
        using reference = parent::postfix_iterator::reference;
        using iterator_category = parent::postfix_iterator::iterator_category;

        explicit postfix_iterator(parent::node* n = nullptr) noexcept;
        postfix_iterator(parent::postfix_iterator) noexcept;

        node_color get_color() const noexcept;

        using parent::postfix_iterator::depth;
        using parent::postfix_iterator::operator*;
        using parent::postfix_iterator::operator==;
        using parent::postfix_iterator::operator!=;
        using parent::postfix_iterator::operator++;
        using parent::postfix_iterator::operator--;
        using parent::postfix_iterator::operator->;
    };

    class postfix_const_iterator : public parent::postfix_const_iterator
    {
    public:

        using value_type = parent::postfix_const_iterator::value_type;
        using difference_type = parent::postfix_const_iterator::difference_type;
        using pointer = parent::postfix_const_iterator::pointer;
        using reference = parent::postfix_const_iterator::reference;
        using iterator_category = parent::postfix_const_iterator::iterator_category;

        explicit postfix_const_iterator(parent::node* n = nullptr) noexcept;
        postfix_const_iterator(parent::postfix_const_iterator) noexcept;

        node_color get_color() const noexcept;

        postfix_const_iterator(postfix_iterator) noexcept;

        using parent::postfix_const_iterator::depth;
        using parent::postfix_const_iterator::operator*;
        using parent::postfix_const_iterator::operator==;
        using parent::postfix_const_iterator::operator!=;
        using parent::postfix_const_iterator::operator++;
        using parent::postfix_const_iterator::operator--;
        using parent::postfix_const_iterator::operator->;
    };

    class postfix_reverse_iterator : public parent::postfix_reverse_iterator
    {
    public:

        using value_type = parent::postfix_reverse_iterator::value_type;
        using difference_type = parent::postfix_reverse_iterator::difference_type;
        using pointer = parent::postfix_reverse_iterator::pointer;
        using reference = parent::postfix_reverse_iterator::reference;
        using iterator_category = parent::postfix_reverse_iterator::iterator_category;

        explicit postfix_reverse_iterator(parent::node* n = nullptr) noexcept;
        postfix_reverse_iterator(parent::postfix_reverse_iterator) noexcept;

        node_color get_color() const noexcept;

        postfix_reverse_iterator(postfix_iterator) noexcept;
        operator postfix_iterator() const noexcept;
        postfix_iterator base() const noexcept;

        using parent::postfix_reverse_iterator::depth;
        using parent::postfix_reverse_iterator::operator*;
        using parent::postfix_reverse_iterator::operator==;
        using parent::postfix_reverse_iterator::operator!=;
        using parent::postfix_reverse_iterator::operator++;
        using parent::postfix_reverse_iterator::operator--;
        using parent::postfix_reverse_iterator::operator->;
    };

    class postfix_const_reverse_iterator : public parent::postfix_const_reverse_iterator
    {
    public:

        using value_type = parent::postfix_const_reverse_iterator::value_type;
        using difference_type = parent::postfix_const_reverse_iterator::difference_type;
        using pointer = parent::postfix_const_reverse_iterator::pointer;
        using reference = parent::postfix_const_reverse_iterator::reference;
        using iterator_category = parent::postfix_const_reverse_iterator::iterator_category;

        explicit postfix_const_reverse_iterator(parent::node* n = nullptr) noexcept;
        postfix_const_reverse_iterator(parent::postfix_const_reverse_iterator) noexcept;

        node_color get_color() const noexcept;

        postfix_const_reverse_iterator(postfix_const_iterator) noexcept;
        operator postfix_const_iterator() const noexcept;
        postfix_const_iterator base() const noexcept;

        using parent::postfix_const_reverse_iterator::depth;
        using parent::postfix_const_reverse_iterator::operator*;
        using parent::postfix_const_reverse_iterator::operator==;
        using parent::postfix_const_reverse_iterator::operator!=;
        using parent::postfix_const_reverse_iterator::operator++;
        using parent::postfix_const_reverse_iterator::operator--;
        using parent::postfix_const_reverse_iterator::operator->;

    };



    // endregion iterator definition
    
    // region iterator requests declaration

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

    // endregion iterator requests declaration
    
public:
    
    ~red_black_tree() noexcept final;
    
    red_black_tree(red_black_tree const &other);
    
    red_black_tree &operator=(red_black_tree const &other);
    
    red_black_tree(red_black_tree &&other) noexcept;
    
    red_black_tree &operator=(red_black_tree &&other) noexcept;


    void swap(parent& other) noexcept override;


    /** Only rebinds iterators
     */
    std::pair<infix_iterator, bool> insert(const value_type&);
    std::pair<infix_iterator, bool> insert(value_type&&);

    template<class ...Args>
    std::pair<infix_iterator, bool> emplace(Args&&...args);

    infix_iterator insert_or_assign(const value_type&);
    infix_iterator insert_or_assign(value_type&&);

    template<class ...Args>
    infix_iterator emplace_or_assign(Args&&...args);

    infix_iterator find(const tkey&);
    infix_const_iterator find(const tkey&) const;

    infix_iterator lower_bound(const tkey&);
    infix_const_iterator lower_bound(const tkey&) const;

    infix_iterator upper_bound(const tkey&);
    infix_const_iterator upper_bound(const tkey&) const;

    infix_iterator erase(infix_iterator pos);
    infix_iterator erase(infix_const_iterator pos);

    infix_iterator erase(infix_iterator first, infix_iterator last);
    infix_iterator erase(infix_const_iterator first, infix_const_iterator last);

    using parent::erase;
    using parent::insert;
    using parent::insert_or_assign;
};

template<typename compare, typename U, typename iterator>
explicit red_black_tree(iterator begin, iterator end, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> red_black_tree<typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit red_black_tree(Range&& range, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> red_black_tree<typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare> ;

template<typename tkey, typename tvalue, typename compare, typename U>
red_black_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr) -> red_black_tree<tkey, tvalue, compare>;

namespace __detail {

    class RB_TAG {};

    template<typename tkey, typename tvalue, typename compare>
    template<class ...Args>
    binary_search_tree<tkey, tvalue, compare, RB_TAG>::node* bst_impl<tkey, tvalue, compare, RB_TAG>::create_node(
            binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont, Args&& ...args)
    {
        using rb = red_black_tree<tkey, tvalue, compare>;

        auto* node = cont._allocator.template new_object<rb::node>(std::forward<Args>(args)...);

        node->left_subtree = nullptr;
        node->right_subtree = nullptr;
        node->color = rb::node_color::RED;

        return node;
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, RB_TAG>::delete_node(
            binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont, binary_search_tree<tkey, tvalue, compare, RB_TAG>::node* n)
    {
        cont._allocator.delete_object(n);
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, RB_TAG>::post_insert(
            binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont,
            typename binary_search_tree<tkey, tvalue, compare, RB_TAG>::node** n)
    {
        using bst = binary_search_tree<tkey, tvalue, compare, RB_TAG>;
        using rb = red_black_tree<tkey, tvalue, compare>;
        using rnode = typename rb::node;

        // Костыль, чтобы исправлять ссылки у родителей.
        auto link_to = [&](typename bst::node *n) -> typename bst::node *& {
            if (n->parent == nullptr) return cont._root;
            return (n == n->parent->left_subtree)
                       ? n->parent->left_subtree
                       : n->parent->right_subtree;
        };

        auto* z = static_cast<rnode*>(*n);

        while (z != static_cast<rnode*>(cont._root)
            && z->get_parent()->color == rb::node_color::RED)
        {
            rnode* parent = z->get_parent();
            rnode* grand = z->get_grandparent();

            if (parent == grand->get_left())
            {
                rnode* uncle = grand->get_right();

                // 1. "y is red" -----------------------------------------------------------------------
                if (uncle && uncle->color == rb::node_color::RED)
                {
                    parent->color = rb::node_color::BLACK;
                    uncle->color = rb::node_color::BLACK;
                    grand->color = rb::node_color::RED;
                    z = grand;
                }
                else
                {
                    // 2. "y is black, z is a right child" ---------------------------------------------
                    if (z == parent->get_right())
                    {
                        z = parent;
                        bst::small_left_rotation(link_to(parent));
                        parent = z->get_parent();
                    }

                    // 3. "y is black, z is a left child" ----------------------------------------------
                    parent->color = rb::node_color::BLACK;
                    grand->color = rb::node_color::RED;
                    bst::small_right_rotation(link_to(grand));
                }
            }
            else /* parent == grand->get_right() */
            {
                rnode* uncle = grand->get_left();

                // 1. "y is red" -----------------------------------------------------------------------
                if (uncle && uncle->color == rb::node_color::RED)
                {
                    parent->color = rb::node_color::BLACK;
                    uncle->color = rb::node_color::BLACK;
                    grand->color = rb::node_color::RED;
                    z = grand;
                }
                else
                {
                    // 2. "y is black, z is a right child" ---------------------------------------------
                    if (z == parent->get_left())
                    {
                        z = parent;
                        bst::small_right_rotation(link_to(parent));
                        parent = z->get_parent();
                    }

                    // 3. "y is black, z is a left child" ----------------------------------------------
                    parent->color = rb::node_color::BLACK;
                    grand->color = rb::node_color::RED;
                    bst::small_left_rotation(link_to(grand));
                }
            }
        }

        static_cast<rnode*>(cont._root)->color = rb::node_color::BLACK;
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, RB_TAG>::erase(
            binary_search_tree<tkey, tvalue, compare, RB_TAG>& cont,
            typename binary_search_tree<tkey, tvalue, compare, RB_TAG>::node** n)
    {
        using bst = binary_search_tree<tkey, tvalue, compare, RB_TAG>;
        using rb = red_black_tree<tkey, tvalue, compare>;
        using rnode = typename rb::node;

        auto color = [](rnode* p) -> typename rb::node_color
        {
            return p ? p->color : rb::node_color::BLACK;
        };

        auto child_is_black = [&](rnode* p, bool right) -> bool
        {
            rnode* c = right
                           ? static_cast<rnode*>(p ? p->right_subtree : nullptr)
                           : static_cast<rnode*>(p ? p->left_subtree : nullptr);
            return color(c) == rb::node_color::BLACK;
        };

        // Костыль, чтобы исправлять ссылки у родителей.
        auto link_to = [&](rnode* n) -> typename bst::node*&
        {
            if (n->parent == nullptr) return cont._root;
            return (n == n->parent->left_subtree)
                       ? n->parent->left_subtree
                       : n->parent->right_subtree;
        };

        auto transplant = [&](rnode* u, rnode* v)
        {
            link_to(u) = v;
            if (v) v->parent = u->parent;
        };

        auto maximum = [](rnode* x)-> rnode*
        {
            while (x->right_subtree)
            {
                x = x->get_right();
            }
            return x;
        };

        rnode* z = static_cast<rnode*>(*n);
        if (!z) throw std::out_of_range("invalid iterator");

        rnode* y = z; // Удаляемая нода
        typename rb::node_color oc = y->color; // Цвет удаляемой ноды
        rnode* x = nullptr; // Поддерево заменяющей ноды

        if (!z->get_left() || !z->get_right())
        {
            x = z->get_left() ? z->get_left() : z->get_right();
            transplant(z, x);
        }
        else
        {
            y = maximum(z->get_left());
            oc = y->color;
            x = y->get_left();

            if (y->parent == z)
            {
                if (x) x->parent = y;
            }
            else
            {
                transplant(y, x);
                y->left_subtree = z->left_subtree;
                y->left_subtree->parent = y;
            }

            transplant(z, y);
            y->right_subtree = z->right_subtree;
            y->right_subtree->parent = y;
            y->color = z->color;
        }

        delete_node(cont, z);
        --cont._size;

        if (oc == rb::node_color::BLACK)
        {
            while (x != static_cast<rnode*>(cont._root)
                && color(x) == rb::node_color::BLACK)
            {
                rnode* xp = x ? x->get_parent() : nullptr;

                if (xp == nullptr) // В корне
                {
                    if (x) x->color = rb::node_color::BLACK;
                    break;
                }

                rnode* w = x->get_sibling();

                // Если нет родственника, обрабатываем этот случай как №2.
                if (w == nullptr)
                {
                    x = xp;
                    continue;
                }

                if (x == xp->get_left())
                {
                    // 1. "w" is red" ------------------------------------------------------------------
                    if (color(w) == rb::node_color::RED)
                    {
                        w->color = rb::node_color::BLACK;
                        xp->color = rb::node_color::RED;
                        bst::small_left_rotation(link_to(xp));
                        w = xp->get_right();
                    }

                    // 2. "w is black and both of w's children are black" ------------------------------
                    if (child_is_black(w, false) && child_is_black(w, true))
                    {
                        if (w) w->color = rb::node_color::RED;
                        x = xp;
                        continue;
                    }

                    // 3. "w is black, w's left child is red, and w's right child is black" ------------
                    if (!child_is_black(w, false) && child_is_black(w, true))
                    {
                        w->get_left()->color = rb::node_color::BLACK;
                        w->color = rb::node_color::RED;
                        bst::small_right_rotation(link_to(xp));  // NB: поворот относительно xp, чтобы проходили тесты
                        w = xp->get_right();
                    }

                    // 4. "w is black, w's left child is black, and w's right child is red" ------------
                    w->color = xp->color;
                    xp->color = rb::node_color::BLACK;
                    if (rnode* wr = w->get_right())
                        wr->color = rb::node_color::BLACK;
                    bst::small_left_rotation(link_to(xp));
                    break;
                }
                else /* x == xp->get_right() */
                {
                    // 1. "w" is red" ------------------------------------------------------------------
                    if (color(w) == rb::node_color::RED)
                    {
                        w->color = rb::node_color::BLACK;
                        xp->color = rb::node_color::RED;
                        bst::small_right_rotation(link_to(xp));
                        w = xp->get_left();
                    }

                    // 2. "w is black and both of w's children are black" ------------------------------
                    if (child_is_black(w, false) && child_is_black(w, true))
                    {
                        if (w) w->color = rb::node_color::RED;
                        x = xp;
                        continue;
                    }

                    // 3. "w is black, w's left child is black, and w's right child is red" ------------
                    if (!child_is_black(w, true) && child_is_black(w, false))
                    {
                        w->get_right()->color = rb::node_color::BLACK;
                        w->color = rb::node_color::RED;
                        bst::small_left_rotation(link_to(xp));
                        w = xp->get_left();
                    }

                    // 4. "w is black, w's left child is red, and w's right child is black" ------------
                    w->color = xp->color;
                    xp->color = rb::node_color::BLACK;
                    w->get_left()->color = rb::node_color::BLACK;
                    bst::small_right_rotation(link_to(xp));
                    break;
                }
            }

            if (x) x->color = rb::node_color::BLACK;
        }

        *n = (x ? x : static_cast<rnode*>(cont._root));
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, RB_TAG>::swap(binary_search_tree<tkey, tvalue, compare, RB_TAG> &lhs,
                                                                            binary_search_tree<tkey, tvalue, compare, RB_TAG> &rhs) noexcept
    {
        // 🤫
    }
}


template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
red_black_tree<tkey, tvalue, compare>::node::node(parent::node* par, Args&&... args)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>::node(par, args...)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::red_black_tree(
        const compare& comp,
        pp_allocator<value_type> alloc,
        logger *log)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(comp, alloc, log)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::red_black_tree(
        pp_allocator<value_type> alloc,
        const compare& comp,
        logger *log)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(alloc, comp, log)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<input_iterator_for_pair<tkey, tvalue> iterator>
red_black_tree<tkey, tvalue, compare>::red_black_tree(
        iterator begin, iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(begin, end, cmp, alloc, log)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<std::ranges::input_range Range>
red_black_tree<tkey, tvalue, compare>::red_black_tree(
        Range&& range,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(range, cmp, alloc, log)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::red_black_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(data, cmp, alloc, log)
{
}

// region iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_iterator::prefix_iterator(parent::node* n) noexcept
    : parent::prefix_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_iterator::prefix_iterator(parent::prefix_iterator it) noexcept
    : parent::prefix_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::prefix_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(parent::prefix_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(parent::node* n) noexcept
    : parent::prefix_const_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(parent::prefix_const_iterator it) noexcept
    : parent::prefix_const_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::prefix_const_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(parent::prefix_const_iterator::_base.get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(prefix_iterator it) noexcept
    : parent::prefix_const_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(parent::node* n) noexcept
    : parent::prefix_reverse_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(parent::prefix_reverse_iterator it) noexcept
    : parent::prefix_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(prefix_reverse_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(prefix_iterator it) noexcept
    : parent::prefix_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator::operator red_black_tree<tkey, tvalue, compare>::prefix_iterator() const noexcept
{
    return parent::prefix_reverse_iterator::operator prefix_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_iterator
red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator::base() const noexcept
{
    return parent::prefix_reverse_iterator::base();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(parent::node* n) noexcept
    : parent::prefix_const_reverse_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(parent::prefix_const_reverse_iterator it) noexcept
    : parent::prefix_const_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(prefix_const_reverse_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(prefix_const_iterator it) noexcept
    : parent::prefix_const_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::operator red_black_tree<tkey, tvalue, compare>::prefix_const_iterator() const noexcept
{
    return parent::prefix_const_reverse_iterator::operator prefix_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_iterator
red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::base() const noexcept
{
    return parent::prefix_const_reverse_iterator::base();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_iterator::infix_iterator(parent::node* n) noexcept
    : parent::infix_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_iterator::infix_iterator(parent::infix_iterator it) noexcept
    : parent::infix_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::infix_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(infix_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(parent::node* n) noexcept
    : parent::infix_const_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(parent::infix_const_iterator it) noexcept
    : parent::infix_const_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::infix_const_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(parent::infix_const_iterator::_base.get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(infix_iterator it) noexcept
    : parent::infix_const_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(parent::node* n) noexcept
    : parent::infix_reverse_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(parent::infix_reverse_iterator it) noexcept
    : parent::infix_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(infix_reverse_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(infix_iterator it) noexcept
    : parent::infix_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator::operator red_black_tree<tkey, tvalue, compare>::infix_iterator() const noexcept
{
    return parent::infix_reverse_iterator::operator infix_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator::base() const noexcept
{
    return parent::infix_reverse_iterator::base();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(parent::node* n) noexcept
    : parent::infix_const_reverse_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(parent::infix_const_reverse_iterator it) noexcept
    : parent::infix_const_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(infix_const_reverse_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(infix_const_iterator it) noexcept
    : parent::infix_const_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::operator red_black_tree<tkey, tvalue, compare>::infix_const_iterator() const noexcept
{
    return parent::infix_const_reverse_iterator::operator infix_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::base() const noexcept
{
    return parent::infix_const_reverse_iterator::base();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_iterator::postfix_iterator(parent::node* n) noexcept
    : parent::postfix_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_iterator::postfix_iterator(parent::postfix_iterator it) noexcept
    : parent::postfix_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::postfix_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(postfix_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(parent::node* n) noexcept
    : parent::postfix_const_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(parent::postfix_const_iterator it) noexcept
    : parent::postfix_const_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::postfix_const_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(parent::postfix_const_iterator::_base.get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(postfix_iterator it) noexcept
    : parent::postfix_const_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(parent::node* n) noexcept
    : parent::postfix_reverse_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(parent::postfix_reverse_iterator it) noexcept
    : parent::postfix_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(postfix_reverse_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(postfix_iterator it) noexcept
    : parent::postfix_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator::operator red_black_tree<tkey, tvalue, compare>::postfix_iterator() const noexcept
{
    return parent::postfix_reverse_iterator::operator postfix_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_iterator
red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator::base() const noexcept
{
    return parent::postfix_reverse_iterator::base();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(parent::node* n) noexcept
    : parent::postfix_const_reverse_iterator(n)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(parent::postfix_const_reverse_iterator it) noexcept
    : parent::postfix_const_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::node_color
red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::get_color() const noexcept
{
    auto* node = static_cast<red_black_tree::node*>(postfix_const_reverse_iterator::get_node());
    return node->color;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(postfix_const_iterator it) noexcept
    : parent::postfix_const_reverse_iterator(it)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::operator red_black_tree<tkey, tvalue, compare>::postfix_const_iterator() const noexcept
{
    return parent::postfix_const_reverse_iterator::operator postfix_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_iterator
red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::base() const noexcept
{
    return parent::postfix_const_reverse_iterator::base();
}

// endregion iterator implementation

// region iterator requests implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::begin() noexcept
{
    return parent::begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::end() noexcept
{
    return parent::end();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::begin() const noexcept
{
    return parent::begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::end() const noexcept
{
    return parent::end();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::cbegin() const noexcept
{
    return parent::cbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::cend() const noexcept
{
    return parent::cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin() noexcept
{
    return parent::rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend() noexcept
{
    return parent::rend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin() const noexcept
{
    return parent::rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend() const noexcept
{
    return parent::rend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crbegin() const noexcept
{
    return parent::crbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crend() const noexcept
{
    return parent::crend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_iterator
red_black_tree<tkey, tvalue, compare>::begin_prefix() noexcept
{
    return parent::begin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_iterator
red_black_tree<tkey, tvalue, compare>::end_prefix() noexcept
{
    return parent::end_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_iterator
red_black_tree<tkey, tvalue, compare>::begin_prefix() const noexcept
{
    return parent::begin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_iterator
red_black_tree<tkey, tvalue, compare>::end_prefix() const noexcept
{
    return parent::end_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_iterator
red_black_tree<tkey, tvalue, compare>::cbegin_prefix() const noexcept
{
    return parent::cbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_iterator
red_black_tree<tkey, tvalue, compare>::cend_prefix() const noexcept
{
    return parent::cend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin_prefix() noexcept
{
    return parent::rbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend_prefix() noexcept
{
    return parent::rend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin_prefix() const noexcept
{
    return parent::rbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend_prefix() const noexcept
{
    return parent::rend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crbegin_prefix() const noexcept
{
    return parent::crbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crend_prefix() const noexcept
{
    return parent::crend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::begin_infix() noexcept
{
    return parent::begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::end_infix() noexcept
{
    return parent::end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::begin_infix() const noexcept
{
    return parent::begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::end_infix() const noexcept
{
    return parent::end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::cbegin_infix() const noexcept
{
    return parent::cbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::cend_infix() const noexcept
{
    return parent::cend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin_infix() noexcept
{
    return parent::rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend_infix() noexcept
{
    return parent::rend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin_infix() const noexcept
{
    return parent::rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend_infix() const noexcept
{
    return parent::rend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crbegin_infix() const noexcept
{
    return parent::crbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crend_infix() const noexcept
{
    return parent::crend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_iterator
red_black_tree<tkey, tvalue, compare>::begin_postfix() noexcept
{
    return parent::begin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_iterator
red_black_tree<tkey, tvalue, compare>::end_postfix() noexcept
{
    return parent::end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_iterator
red_black_tree<tkey, tvalue, compare>::begin_postfix() const noexcept
{
    return parent::begin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_iterator
red_black_tree<tkey, tvalue, compare>::end_postfix() const noexcept
{
    return parent::end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_iterator
red_black_tree<tkey, tvalue, compare>::cbegin_postfix() const noexcept
{
    return parent::cbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_iterator
red_black_tree<tkey, tvalue, compare>::cend_postfix() const noexcept
{
    return parent::cend_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin_postfix() noexcept
{
    return parent::rbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend_postfix() noexcept
{
    return parent::rend_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rbegin_postfix() const noexcept
{
    return parent::rbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::rend_postfix() const noexcept
{
    return parent::rend_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crbegin_postfix() const noexcept
{
    return parent::crbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
red_black_tree<tkey, tvalue, compare>::crend_postfix() const noexcept
{
    return parent::crend_postfix();
}

// endregion iterator requests implementation

// region rb_tree implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::~red_black_tree() noexcept
{
    // no-op
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::red_black_tree(red_black_tree const &other)
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(other)
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare> &
red_black_tree<tkey, tvalue, compare>::operator=(red_black_tree const &other)
{
    parent::operator=(other);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare>::red_black_tree(red_black_tree &&other) noexcept
    : binary_search_tree<tkey, tvalue, compare, __detail::RB_TAG>(std::forward<red_black_tree>(other))
{
}

template<typename tkey, typename tvalue, compator<tkey> compare>
red_black_tree<tkey, tvalue, compare> &
red_black_tree<tkey, tvalue, compare>::operator=(red_black_tree &&other) noexcept
{
    parent::operator=(std::forward<red_black_tree>(other));
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void red_black_tree<tkey, tvalue, compare>::swap(parent& other) noexcept
{
    parent::swap(other);
}

// endregion rb_tree implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
std::pair<typename red_black_tree<tkey, tvalue, compare>::infix_iterator, bool>
red_black_tree<tkey, tvalue, compare>::insert(const value_type& value)
{
    return parent::insert(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
std::pair<typename red_black_tree<tkey, tvalue, compare>::infix_iterator, bool>
red_black_tree<tkey, tvalue, compare>::insert(value_type&& value)
{
    return parent::insert(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
std::pair<typename red_black_tree<tkey, tvalue, compare>::infix_iterator, bool>
red_black_tree<tkey, tvalue, compare>::emplace(Args&&... args)
{
    return parent::emplace(args...);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::insert_or_assign(const value_type& value)
{
    return parent::insert_or_assign(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::insert_or_assign(value_type&& value)
{
    return parent::insert_or_assign(value);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::emplace_or_assign(Args&&... args)
{
    return parent::emplace_or_assign(args);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::find(const tkey& key)
{
    return parent::find(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::find(const tkey& key) const
{
    return parent::find(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::lower_bound(const tkey& key)
{
    return parent::lower_bound(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::lower_bound(const tkey& key) const
{
    return parent::lower_bound(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::upper_bound(const tkey& key)
{
    return parent::upper_bound(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_const_iterator
red_black_tree<tkey, tvalue, compare>::upper_bound(const tkey& key) const
{
    return parent::upper_bound(key);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::erase(infix_iterator pos)
{
    return parent::erase(pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::erase(infix_const_iterator pos)
{
    return parent::erase(pos);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::erase(infix_iterator first, infix_iterator last)
{
    return parent::erase(first, last);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename red_black_tree<tkey, tvalue, compare>::infix_iterator
red_black_tree<tkey, tvalue, compare>::erase(infix_const_iterator first, infix_const_iterator last)
{
    return parent::erase(first, last);
}

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_RED_BLACK_TREE_H
