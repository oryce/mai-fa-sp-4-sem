#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H

#include <binary_search_tree.h>

namespace __detail {
    class AVL_TAG;

    template<typename tkey, typename tvalue, typename compare>

    class bst_impl<tkey, tvalue, compare, AVL_TAG>
    {
    public:
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *
        create_node(binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont, Args &&...args);


        static void delete_node(binary_search_tree<tkey, tvalue, compare, AVL_TAG>& cont, binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node*);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **) {}

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont,
                                binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **);

        static void erase(binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont,
                          binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **);

        static void swap(binary_search_tree<tkey, tvalue, compare, AVL_TAG> &lhs,
                         binary_search_tree<tkey, tvalue, compare, AVL_TAG> &rhs) noexcept;
    };
}

template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>>
class AVL_tree final :
        public binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG> {
    using parent = binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>;
    friend __detail::bst_impl<tkey, tvalue, compare, __detail::AVL_TAG>;
private:

    struct node final : public parent::node {
        size_t height;

        void recalculate_height() noexcept;

        /*
         * Returns positive if right subtree is bigger
         */
        short get_balance() const noexcept;

        template<class ...Args>
        node(parent::node *par, Args &&... args);

        ~node() noexcept override = default;
    };

public:

    using value_type = parent::value_type;

    //Весь смысл всех этих итераторов только в том, что они преобразовывают итераторы обычного бинарного дерева в итераторы авл дерева.
    //Никакой дополнительной логики у них нет.
    // region iterator definition

    class prefix_iterator : public parent::prefix_iterator {
    public:

        using value_type = parent::prefix_iterator::value_type;
        using difference_type = parent::prefix_iterator::difference_type;
        using pointer = parent::prefix_iterator::pointer;
        using reference = parent::prefix_iterator::reference;
        using iterator_category = parent::prefix_iterator::iterator_category;

        explicit prefix_iterator(parent::node *n = nullptr) noexcept;

        prefix_iterator(parent::prefix_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

        using parent::prefix_iterator::depth;
        using parent::prefix_iterator::operator*;
        using parent::prefix_iterator::operator==;
        using parent::prefix_iterator::operator!=;
        using parent::prefix_iterator::operator++;
        using parent::prefix_iterator::operator--;
        using parent::prefix_iterator::operator->;
    };

    class prefix_const_iterator : public parent::prefix_const_iterator {
    public:

        using value_type = parent::prefix_const_iterator::value_type;
        using difference_type = parent::prefix_const_iterator::difference_type;
        using pointer = parent::prefix_const_iterator::pointer;
        using reference = parent::prefix_const_iterator::reference;
        using iterator_category = parent::prefix_const_iterator::iterator_category;

        explicit prefix_const_iterator(parent::node *n = nullptr) noexcept;

        prefix_const_iterator(parent::prefix_const_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

        prefix_const_iterator(prefix_iterator) noexcept;

        using parent::prefix_const_iterator::depth;
        using parent::prefix_const_iterator::operator*;
        using parent::prefix_const_iterator::operator==;
        using parent::prefix_const_iterator::operator!=;
        using parent::prefix_const_iterator::operator++;
        using parent::prefix_const_iterator::operator--;
        using parent::prefix_const_iterator::operator->;
    };

    class prefix_reverse_iterator : public parent::prefix_reverse_iterator {
    public:

        using value_type = parent::prefix_reverse_iterator::value_type;
        using difference_type = parent::prefix_reverse_iterator::difference_type;
        using pointer = parent::prefix_reverse_iterator::pointer;
        using reference = parent::prefix_reverse_iterator::reference;
        using iterator_category = parent::prefix_reverse_iterator::iterator_category;

        explicit prefix_reverse_iterator(parent::node *n = nullptr) noexcept;

        prefix_reverse_iterator(parent::prefix_reverse_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

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

    class prefix_const_reverse_iterator : public parent::prefix_const_reverse_iterator {
    public:

        using value_type = parent::prefix_const_reverse_iterator::value_type;
        using difference_type = parent::prefix_const_reverse_iterator::difference_type;
        using pointer = parent::prefix_const_reverse_iterator::pointer;
        using reference = parent::prefix_const_reverse_iterator::reference;
        using iterator_category = parent::prefix_const_reverse_iterator::iterator_category;

        explicit prefix_const_reverse_iterator(parent::node *n = nullptr) noexcept;

        prefix_const_reverse_iterator(parent::prefix_const_reverse_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

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

    class infix_iterator : public parent::infix_iterator {
    public:

        using value_type = parent::infix_iterator::value_type;
        using difference_type = parent::infix_iterator::difference_type;
        using pointer = parent::infix_iterator::pointer;
        using reference = parent::infix_iterator::reference;
        using iterator_category = parent::infix_iterator::iterator_category;

        explicit infix_iterator(parent::node *n = nullptr) noexcept;

        infix_iterator(parent::infix_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

        using parent::infix_iterator::depth;
        using parent::infix_iterator::operator*;
        using parent::infix_iterator::operator==;
        using parent::infix_iterator::operator!=;
        using parent::infix_iterator::operator++;
        using parent::infix_iterator::operator--;
        using parent::infix_iterator::operator->;
    };

    class infix_const_iterator : parent::infix_const_iterator {
    public:

        using value_type = parent::infix_const_iterator::value_type;
        using difference_type = parent::infix_const_iterator::difference_type;
        using pointer = parent::infix_const_iterator::pointer;
        using reference = parent::infix_const_iterator::reference;
        using iterator_category = parent::infix_const_iterator::iterator_category;

        explicit infix_const_iterator(parent::node *n = nullptr) noexcept;

        infix_const_iterator(parent::infix_const_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

        infix_const_iterator(infix_iterator) noexcept;

        using parent::infix_const_iterator::depth;
        using parent::infix_const_iterator::operator*;
        using parent::infix_const_iterator::operator==;
        using parent::infix_const_iterator::operator!=;
        using parent::infix_const_iterator::operator++;
        using parent::infix_const_iterator::operator--;
        using parent::infix_const_iterator::operator->;
    };

    class infix_reverse_iterator : public parent::infix_reverse_iterator {
    public:

        using value_type = parent::infix_reverse_iterator::value_type;
        using difference_type = parent::infix_reverse_iterator::difference_type;
        using pointer = parent::infix_reverse_iterator::pointer;
        using reference = parent::infix_reverse_iterator::reference;
        using iterator_category = parent::infix_reverse_iterator::iterator_category;

        explicit infix_reverse_iterator(parent::node *n = nullptr) noexcept;

        infix_reverse_iterator(parent::infix_reverse_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

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

    class infix_const_reverse_iterator : public parent::infix_const_reverse_iterator {
    public:

        using value_type = parent::infix_const_reverse_iterator::value_type;
        using difference_type = parent::infix_const_reverse_iterator::difference_type;
        using pointer = parent::infix_const_reverse_iterator::pointer;
        using reference = parent::infix_const_reverse_iterator::reference;
        using iterator_category = parent::infix_const_reverse_iterator::iterator_category;

        explicit infix_const_reverse_iterator(parent::node *n = nullptr) noexcept;

        infix_const_reverse_iterator(parent::infix_const_reverse_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

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

    class postfix_iterator : public parent::postfix_iterator {
    public:

        using value_type = parent::postfix_iterator::value_type;
        using difference_type = parent::postfix_iterator::difference_type;
        using pointer = parent::postfix_iterator::pointer;
        using reference = parent::postfix_iterator::reference;
        using iterator_category = parent::postfix_iterator::iterator_category;

        explicit postfix_iterator(parent::node *n = nullptr) noexcept;

        postfix_iterator(parent::postfix_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

        using parent::postfix_iterator::depth;
        using parent::postfix_iterator::operator*;
        using parent::postfix_iterator::operator==;
        using parent::postfix_iterator::operator!=;
        using parent::postfix_iterator::operator++;
        using parent::postfix_iterator::operator--;
        using parent::postfix_iterator::operator->;
    };

    class postfix_const_iterator : public parent::postfix_const_iterator {
    public:

        using value_type = parent::postfix_const_iterator::value_type;
        using difference_type = parent::postfix_const_iterator::difference_type;
        using pointer = parent::postfix_const_iterator::pointer;
        using reference = parent::postfix_const_iterator::reference;
        using iterator_category = parent::postfix_const_iterator::iterator_category;

        explicit postfix_const_iterator(parent::node *n = nullptr) noexcept;

        postfix_const_iterator(parent::postfix_const_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

        postfix_const_iterator(postfix_iterator) noexcept;

        using parent::postfix_const_iterator::depth;
        using parent::postfix_const_iterator::operator*;
        using parent::postfix_const_iterator::operator==;
        using parent::postfix_const_iterator::operator!=;
        using parent::postfix_const_iterator::operator++;
        using parent::postfix_const_iterator::operator--;
        using parent::postfix_const_iterator::operator->;
    };

    class postfix_reverse_iterator : public parent::postfix_reverse_iterator {
    public:

        using value_type = parent::postfix_reverse_iterator::value_type;
        using difference_type = parent::postfix_reverse_iterator::difference_type;
        using pointer = parent::postfix_reverse_iterator::pointer;
        using reference = parent::postfix_reverse_iterator::reference;
        using iterator_category = parent::postfix_reverse_iterator::iterator_category;

        explicit postfix_reverse_iterator(parent::node *n = nullptr) noexcept;

        postfix_reverse_iterator(parent::postfix_reverse_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

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

    class postfix_const_reverse_iterator : public parent::postfix_const_reverse_iterator {
    public:

        using value_type = parent::postfix_const_reverse_iterator::value_type;
        using difference_type = parent::postfix_const_reverse_iterator::difference_type;
        using pointer = parent::postfix_const_reverse_iterator::pointer;
        using reference = parent::postfix_const_reverse_iterator::reference;
        using iterator_category = parent::postfix_const_reverse_iterator::iterator_category;

        explicit postfix_const_reverse_iterator(parent::node *n = nullptr) noexcept;

        postfix_const_reverse_iterator(parent::postfix_const_reverse_iterator) noexcept;

        size_t get_height() const noexcept;

        size_t get_balance() const noexcept;

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

    explicit AVL_tree(
            const compare &comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr);

    explicit AVL_tree(
            pp_allocator<value_type> alloc,
            const compare &comp = compare(),
            logger *log = nullptr);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit AVL_tree(iterator begin, iterator end, const compare &cmp = compare(),
                      pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                      logger *log = nullptr);

    template<std::ranges::input_range Range>
    explicit AVL_tree(Range &&range, const compare &cmp = compare(),
                      pp_allocator<value_type> alloc = pp_allocator<value_type>(),
                      logger *log = nullptr);


    AVL_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(),
             pp_allocator<value_type> alloc = pp_allocator<value_type>(),
             logger *log = nullptr);

public:

    ~AVL_tree() noexcept final = default;

    AVL_tree(AVL_tree const &other);

    AVL_tree &operator=(AVL_tree const &other);

    AVL_tree(AVL_tree &&other) noexcept = default;

    AVL_tree &operator=(AVL_tree &&other) noexcept = default;

    void swap(parent &other) noexcept override;


    using parent::erase;
    using parent::insert;
    using parent::insert_or_assign;
    using parent::emplace;

    static parent::node *rebalance(parent::node *to_balance);

    void rebalance_to_root(parent::node *from);
};

template<typename compare, typename U, typename iterator>
explicit AVL_tree(iterator begin, iterator end, const compare &cmp = compare(),
                  pp_allocator<U> alloc = pp_allocator<U>(),
                  logger *log = nullptr) -> AVL_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit AVL_tree(Range &&range, const compare &cmp = compare(),
                  pp_allocator<U> alloc = pp_allocator<U>(),
                  logger *log = nullptr) -> AVL_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare>;

template<typename tkey, typename tvalue, typename compare, typename U>
AVL_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare &cmp = compare(),
         pp_allocator<U> alloc = pp_allocator<U>(),
         logger *log = nullptr) -> AVL_tree<tkey, tvalue, compare>;

namespace __detail {
    template<typename tkey, typename tvalue, typename compare>
    template<class ...Args>
    binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *bst_impl<tkey, tvalue, compare, AVL_TAG>::create_node(
            binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont, Args &&...args) {
        typename AVL_tree<tkey, tvalue, compare>::node *n = cont._allocator.new_object<typename AVL_tree<tkey, tvalue, compare>::node>(
                args ...);
        n->left_subtree = nullptr;
        n->right_subtree = nullptr;
        return dynamic_cast<binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *>(n);
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, AVL_TAG>::delete_node(
            binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont,
            binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *n) {
        auto *avl_node = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(n);
        cont._allocator.delete_object(n);
    }

    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, AVL_TAG>::post_insert(
            binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont,
            typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **node) {

        typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *cur = *node;
        while (cur != nullptr) {
            dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(cur)->recalculate_height();
            cur = AVL_tree<tkey, tvalue, compare>::rebalance(cur);
            if (cur == nullptr) {
                break;
            }
            dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(cur)->recalculate_height();
            if (cur->parent == nullptr) {
                cont._root = cur;
            }
            cur = cur->parent;
        }
    }


    //Работает следующим образом:
    //Если у удаляемого элемента нет правого поддерева, то мы просто передвигаем его левое поддерево на его место.
    //Если у удаляемого элемента есть правое поддерево, то мы находим в нем самый левый элемент и ставим его на место удаляемого.
    //После этого запускаем ребалансировку от нового элемента до корня.
    //Не самый оптимальный алгоритм (вероятно), но ассимптотитечски он ничего не портит, О(log n) остается.
    
    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, AVL_TAG>::erase(
            binary_search_tree<tkey, tvalue, compare, AVL_TAG> &cont,
            typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node **node) {
        auto &avl_tree = dynamic_cast<AVL_tree<tkey, tvalue, compare> &>(cont);
        auto *node_to_delete = *node;
        if (node_to_delete == nullptr) {
            throw std::out_of_range("Incorrect iterator for erase\n");
        }

        auto *parent = node_to_delete->parent;
        typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *new_node = nullptr;
        if (node_to_delete->right_subtree == nullptr) {
            if (parent->left_subtree == node_to_delete) {
                parent->left_subtree = node_to_delete->left_subtree;
            } else {
                parent->right_subtree = node_to_delete->left_subtree;
            }
            new_node = node_to_delete->left_subtree;
        } else {
            typename binary_search_tree<tkey, tvalue, compare, AVL_TAG>::node *successor;

            successor = node_to_delete->left_subtree;
            while (successor->right_subtree) {
                successor = successor->right_subtree;
            }

            new_node = successor;


            if (new_node->parent != nullptr) {
                if (new_node->parent->right_subtree == new_node) {
                    new_node->parent->right_subtree = new_node->right_subtree;
                } else {
                    new_node->parent->left_subtree = new_node->right_subtree;
                }
            }
            dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(new_node)->recalculate_height();


            if (node_to_delete->parent != nullptr) {
                if (node_to_delete->parent->right_subtree == node_to_delete) {
                    node_to_delete->parent->right_subtree = new_node;
                } else {
                    node_to_delete->parent->left_subtree = new_node;
                }
            }

            //Managing case when new_node is child of node_to_delete
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
            avl_tree._root = new_node;
        }
        avl_tree.rebalance_to_root(new_node);
        avl_tree._size--;
        __detail::bst_impl<tkey, tvalue, compare, __detail::AVL_TAG>::delete_node(cont, node_to_delete);
        *node = new_node;
    }

    template<typename tkey, typename tvalue, typename compare>
    void __detail::bst_impl<tkey, tvalue, compare, __detail::AVL_TAG>::swap(
            binary_search_tree<tkey, tvalue, compare, AVL_TAG> &lhs,
            binary_search_tree<tkey, tvalue, compare, AVL_TAG> &rhs) noexcept {
    }
}

// region node implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
void AVL_tree<tkey, tvalue, compare>::node::recalculate_height() noexcept {
    auto *left = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(this->left_subtree);
    auto *right = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(this->right_subtree);
    int left_height = (left ? left->height : 0);
    int right_height = (right ? right->height : 0);
    this->height = 1 + std::max(left_height, right_height);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
short AVL_tree<tkey, tvalue, compare>::node::get_balance() const noexcept {
    auto left = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(this->left_subtree);
    auto right = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(this->right_subtree);
    short left_height = (left ? left->height : 0);
    short right_height = (right ? right->height : 0);
    return right_height - left_height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
AVL_tree<tkey, tvalue, compare>::node::node(parent::node *par, Args &&... args)
        : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>::node(par,
                                                                             args ...) {
    height = 0;
}

// endregion node implementation

//region iterators implemention

// region prefix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_iterator::prefix_iterator(parent::node *n) noexcept : parent::prefix_iterator(
        n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_iterator::prefix_iterator(parent::prefix_iterator it) noexcept
        : parent::prefix_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_iterator.get_node())->get_balance();
}

// endregion prefix_iterator implementation

// region prefix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(parent::node *n) noexcept
        : parent::prefix_const_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(
        parent::prefix_const_iterator it) noexcept : parent::prefix_const_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::prefix_const_iterator(prefix_iterator it) noexcept
        : parent::prefix_const_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_const_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_const_iterator::_base.get_node())->get_balance();
}

// endregion prefix_const_iterator implementation

// region prefix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(parent::node *n) noexcept
        : parent::prefix_reverse_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(
        parent::prefix_reverse_iterator it) noexcept : parent::prefix_reverse_iterator(it) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_reverse_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_reverse_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::prefix_reverse_iterator(prefix_iterator it) noexcept
        : parent::prefix_reverse_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::prefix_iterator() const noexcept {
    return parent::prefix_reverse_iterator::operator prefix_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_iterator
AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator::base() const noexcept {
    return parent::prefix_reverse_iterator::base();
}

// endregion prefix_reverse_iterator implementation

// region prefix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        parent::node *n) noexcept : parent::prefix_const_reverse_iterator(n){

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        parent::prefix_const_reverse_iterator it) noexcept : parent::prefix_const_reverse_iterator(it){
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_const_reverse_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::prefix_const_reverse_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::prefix_const_reverse_iterator(
        prefix_const_iterator it) noexcept : parent::prefix_const_reverse_iterator(it){
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::prefix_const_iterator() const noexcept {
    return parent::prefix_const_reverse_iterator::operator prefix_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator
AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator::base() const noexcept {
    return parent::prefix_const_reverse_iterator::base();
}

// endregion prefix_const_reverse_iterator implementation

// region infix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_iterator::infix_iterator(parent::node *n) noexcept : parent::infix_iterator(n) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_iterator::infix_iterator(parent::infix_iterator it) noexcept {
    this->_data = it.get_data();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_iterator::get_node())->get_balance();
}

// endregion infix_iterator implementation

// region infix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(parent::node *n) noexcept
        : parent::infix_const_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(parent::infix_const_iterator it) noexcept
        : parent::infix_const_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_const_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_const_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_iterator::infix_const_iterator(infix_iterator it) noexcept
        : parent::infix_const_iterator(it) {

}

// endregion infix_const_iterator implementation

// region infix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(parent::node *n) noexcept
        : parent::infix_reverse_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(
        parent::infix_reverse_iterator it) noexcept {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_reverse_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_reverse_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::infix_reverse_iterator(infix_iterator it) noexcept
        : parent::infix_reverse_iterator::infix_reverse_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::infix_iterator() const noexcept {
    return parent::infix_reverse_iterator::operator infix_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator::base() const noexcept {
    return parent::infix_reverse_iterator::base();
}

// endregion infix_reverse_iterator implementation

// region infix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(parent::node *n) noexcept
        : parent::infix_const_reverse_iterator::infix_const_reverse_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        parent::infix_const_reverse_iterator it) noexcept : parent::infix_const_reverse_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_const_reverse_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::infix_const_reverse_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::infix_const_reverse_iterator(
        infix_const_iterator it) noexcept : parent::infix_const_reverse_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::infix_const_iterator() const noexcept {
    return parent::infix_const_reverse_iterator::operator infix_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator::base() const noexcept {
    return parent::infix_const_reverse_iterator::base();
}

// endregion infix_const_reverse_iterator implementation

// region postfix_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_iterator::postfix_iterator(parent::node *n) noexcept
        : parent::postfix_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_iterator::postfix_iterator(parent::postfix_iterator it) noexcept
        : parent::postfix_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_iterator.get_node())->get_balance();
}

// endregion postfix_iterator implementation

// region postfix_const_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(parent::node *n) noexcept
        : parent::postfix_const_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(
        parent::postfix_const_iterator it) noexcept: parent::postfix_const_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_const_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_const_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_iterator::postfix_const_iterator(postfix_iterator it) noexcept
        : parent::postfix_const_iterator(it) {

}

// endregion postfix_const_iterator implementation

// region postfix_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(parent::node *n) noexcept
        : parent::postfix_reverse_iterator(n) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(
        parent::postfix_reverse_iterator it) noexcept : parent::postfix_reverse_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_reverse_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_reverse_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::postfix_reverse_iterator(postfix_iterator it) noexcept
        : parent::postfix_reverse_iterator(it) {

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::postfix_iterator() const noexcept {
    return parent::postfix_reverse_iterator::operator postfix_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_iterator
AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator::base() const noexcept {
    return parent::postfix_reverse_iterator::base();
}

// endregion postfix_reverse_iterator implementation

// region postfix_const_reverse_iterator implementation

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        parent::node *n) noexcept : parent::postfix_const_reverse_iterator(n){

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        parent::postfix_const_reverse_iterator it) noexcept : parent::postfix_const_reverse_iterator(it){

}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::get_height() const noexcept {
    auto *n = dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_const_reverse_iterator::_base.get_node());
    return n->height;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::get_balance() const noexcept {
    return dynamic_cast<AVL_tree<tkey, tvalue, compare>::node *>(parent::postfix_const_reverse_iterator::_base.get_node())->get_balance();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::postfix_const_reverse_iterator(
        postfix_const_iterator it) noexcept : parent::postfix_const_reverse_iterator(it){

}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::operator AVL_tree<tkey, tvalue, compare>::postfix_const_iterator() const noexcept {
    return parent::postfix_const_reverse_iterator::operator postfix_const_iterator();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator
AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator::base() const noexcept {
    return parent::postfix_const_reverse_iterator::base();
}

// endregion postfix_const_reverse_iterator implementation

// region iterator requests implementation

// Infix iterators
template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator AVL_tree<tkey, tvalue, compare>::begin() noexcept {
    return parent::begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::end() noexcept {
    return parent::end();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::begin() const noexcept {
    return parent::begin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::end() const noexcept {
    return parent::end();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::cbegin() const noexcept {
    return parent::cbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::cend() const noexcept {
    return parent::cend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rbegin() noexcept {
    return parent::rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator AVL_tree<tkey, tvalue, compare>::rend() noexcept {
    return parent::rend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin() const noexcept {
    return parent::rbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend() const noexcept {
    return parent::rend();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crbegin() const noexcept {
    return parent::crbegin();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crend() const noexcept {
    return parent::crend();
}

// region prefix iterators

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_iterator AVL_tree<tkey, tvalue, compare>::begin_prefix() noexcept {
    return parent::begin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_iterator AVL_tree<tkey, tvalue, compare>::end_prefix() noexcept {
    return parent::end_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator
AVL_tree<tkey, tvalue, compare>::begin_prefix() const noexcept {
    return parent::begin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator
AVL_tree<tkey, tvalue, compare>::end_prefix() const noexcept {
    return parent::end_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator
AVL_tree<tkey, tvalue, compare>::cbegin_prefix() const noexcept {
    return parent::cbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_iterator
AVL_tree<tkey, tvalue, compare>::cend_prefix() const noexcept {
    return parent::cend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin_prefix() noexcept {
    return parent::rbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend_prefix() noexcept {
    return parent::rend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin_prefix() const noexcept {
    return parent::rbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend_prefix() const noexcept {
    return parent::rend_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crbegin_prefix() const noexcept {
    return parent::crbegin_prefix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::prefix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crend_prefix() const noexcept {
    return parent::crend_prefix();
}

// endregion prefix iterators
// region infix iterators
template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::begin_infix() noexcept {
    return parent::begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_iterator
AVL_tree<tkey, tvalue, compare>::end_infix() noexcept {
    return parent::end_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::begin_infix() const noexcept {
    return parent::begin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::end_infix() const noexcept {
    return parent::end_infix();
}


template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::cbegin_infix() const noexcept {
    return parent::cbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_iterator
AVL_tree<tkey, tvalue, compare>::cend_infix() const noexcept {
    return parent::cend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin_infix() noexcept {
    return parent::rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend_infix() noexcept {
    return parent::rend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin_infix() const noexcept {
    return parent::rbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend_infix() const noexcept {
    return parent::rend_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crbegin_infix() const noexcept {
    return parent::crbegin_infix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::infix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crend_infix() const noexcept {
    return parent::crend_infix();
}

// endregion infix iterators

// region postfix iterators
template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_iterator
AVL_tree<tkey, tvalue, compare>::begin_postfix() noexcept {
    return parent::begin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_iterator
AVL_tree<tkey, tvalue, compare>::end_postfix() noexcept {
    return parent::end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator
AVL_tree<tkey, tvalue, compare>::begin_postfix() const noexcept {
    return parent::begin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator
AVL_tree<tkey, tvalue, compare>::end_postfix() const noexcept {
    return parent::end_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator
AVL_tree<tkey, tvalue, compare>::cbegin_postfix() const noexcept {
    return parent::cbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_iterator
AVL_tree<tkey, tvalue, compare>::cend_postfix() const noexcept {
    return parent::cend_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin_postfix() noexcept {
    return parent::rbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend_postfix() noexcept {
    return parent::rend_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rbegin_postfix() const noexcept {
    return parent::rbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::rend_postfix() const noexcept {
    return parent::rend_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crbegin_postfix() const noexcept {
    return parent::crbegin_postfix();
}

template<typename tkey, typename tvalue, compator<tkey> compare>
typename AVL_tree<tkey, tvalue, compare>::postfix_const_reverse_iterator
AVL_tree<tkey, tvalue, compare>::crend_postfix() const noexcept {
    return parent::crend_postfix();
}

// endregion postfix iterators

// endregion iterator requests implementation

// endregion iterators implemention

// region AVL_tree constructors

// Constructors
template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        const compare &comp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>(comp, alloc, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        pp_allocator<value_type> alloc,
        const compare &comp,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>(alloc, comp, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<input_iterator_for_pair<tkey, tvalue> iterator>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        iterator begin, iterator end,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>(begin, end, cmp, alloc) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<std::ranges::input_range Range>
AVL_tree<tkey, tvalue, compare>::AVL_tree(
        Range &&range,
        const compare &cmp,
        pp_allocator<value_type> alloc,
        logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>(range, cmp, alloc, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(std::initializer_list<std::pair<tkey, tvalue>> data,
                                          const compare &cmp, pp_allocator<value_type> alloc,
                                          logger *log) : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>(
        data, alloc, log) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare>::AVL_tree(const AVL_tree &other)
        : binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>(other) {
}

template<typename tkey, typename tvalue, compator<tkey> compare>
AVL_tree<tkey, tvalue, compare> &AVL_tree<tkey, tvalue, compare>::operator=(const AVL_tree &other) {
    parent::operator=(other);
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void AVL_tree<tkey, tvalue, compare>::swap(parent &other) noexcept {
    parent::swap(other);
}

// endregion AVL_tree constructors

// region AVL_tree methods

//Функция возвращает указатель на ноду, которая заменила старую ноду, если был произведен поворот, т.е.
//
//  cur                          cur
//   ↓                            ↓
//   a      какой то поворот      b
// b   c    ---------------->       a
//                                    c
//
template<typename tkey, typename tvalue, compator<tkey> compare>
binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>::node *
AVL_tree<tkey, tvalue, compare>::rebalance(parent::node *to_balance) {
    auto *n = dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(to_balance);
    auto *left = dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(n->left_subtree);
    auto *right = dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(n->right_subtree);
    if (n->get_balance() == 2) {
        auto hl = right->left_subtree
                  ? dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(right->left_subtree)->height : 0;
        auto hr = right->right_subtree
                  ? dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(right->right_subtree)->height : 0;
        if (hl <= hr) {
            binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>::small_left_rotation(to_balance);
        } else {
            binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>::big_left_rotation(to_balance);
        }
    } else if (n->get_balance() == -2) {
        auto hl = left->left_subtree
                  ? dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(left->left_subtree)->height : 0;
        auto hr = left->right_subtree
                  ? dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(left->right_subtree)->height : 0;
        if (hr <= hl) {
            binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>::small_right_rotation(to_balance);
        } else {
            binary_search_tree<tkey, tvalue, compare, __detail::AVL_TAG>::big_right_rotation(to_balance);
        }
    }
    n->recalculate_height();
    if (n->parent) {
        if (n->parent->left_subtree) {
            dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(n->parent->left_subtree)->recalculate_height();
        }
        if (n->parent->right_subtree) {
            dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(n->parent->right_subtree)->recalculate_height();
        }
    }
    return to_balance;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void AVL_tree<tkey, tvalue, compare>::rebalance_to_root(parent::node *from) {
    auto cur = from;
    while (cur != nullptr) {
        cur = AVL_tree<tkey, tvalue, compare>::rebalance(cur);
        dynamic_cast<typename AVL_tree<tkey, tvalue, compare>::node *>(cur)->recalculate_height();
        if (cur->parent == nullptr) {
            this->_root = cur;
        }
        cur = cur->parent;
    }
}

// endregion AVL_tree methods

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_AVL_TREE_H