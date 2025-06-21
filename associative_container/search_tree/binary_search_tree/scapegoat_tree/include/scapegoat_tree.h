#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SCAPEGOAT_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SCAPEGOAT_TREE_H

#include <binary_search_tree.h>
#include <iterator>

namespace __detail
{
    class SPG_TAG;

    template<typename tkey, typename tvalue, typename compare>
    class bst_impl<tkey, tvalue, compare, SPG_TAG>
    {
    public:
        template<class ...Args>
        static binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node* create_node(binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont, Args&& ...args);

        static void delete_node(binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node*);

        //Does not invalidate node*, needed for splay tree
        static void post_search(binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node**);

        //Does not invalidate node*
        static void post_insert(binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node**);

        static void erase(binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont, binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node**);

        static void swap(binary_search_tree<tkey, tvalue, compare, SPG_TAG>& lhs, binary_search_tree<tkey, tvalue, compare, SPG_TAG>& rhs) noexcept;
    };
}


template<typename tkey, typename tvalue, compator<tkey> compare = std::less<tkey>>
class scapegoat_tree final:
    public binary_search_tree<tkey, tvalue, compare, __detail::SPG_TAG>
{

private:

    using parent = binary_search_tree<tkey, tvalue, compare, __detail::SPG_TAG>;
    friend __detail::bst_impl<tkey, tvalue, compare, __detail::SPG_TAG>;
    struct node final:
        parent::node
    {
        size_t size;

        node(tkey const &key_, tvalue &&value_);
        node(tkey const &key_, const tvalue& value_);

        template<class ...Args>
        node(parent::node* par, Args&&... args);

        void recalculate_size() noexcept;
        bool is_disbalanced(double alpha) noexcept;
        size_t get_left_size();
        size_t get_right_size();
    };

    node* find_scapegoat(node* from);

    void rebalance_from_scapegoat(node* scapegoat);

    node* rebalance_recursively(std::vector<node*>);
    size_t get_mediana(std::vector<node*>);

public:

    using value_type = parent::value_type;

    explicit scapegoat_tree(
            const compare& comp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger *log = nullptr, double alpha = 0.7);

    explicit scapegoat_tree(
            pp_allocator<value_type> alloc,
            const compare& comp = compare(),
            logger *log = nullptr, double alpha = 0.7);

    template<input_iterator_for_pair<tkey, tvalue> iterator>
    explicit scapegoat_tree(iterator begin, iterator end, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr, double alpha = 0.7);

    template<std::ranges::input_range Range>
    explicit scapegoat_tree(Range&& range, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr, double alpha = 0.7);


    scapegoat_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
            pp_allocator<value_type> alloc = pp_allocator<value_type>(),
            logger* log = nullptr, double alpha = 0.7);


public:
    
    ~scapegoat_tree() noexcept final;
    
    scapegoat_tree(scapegoat_tree const &other);
    
    scapegoat_tree &operator=(scapegoat_tree const &other);
    
    scapegoat_tree(scapegoat_tree &&other) noexcept;
    
    scapegoat_tree &operator=(scapegoat_tree &&other) noexcept;

public:
    
    void setup_alpha(double alpha);

private:

    double _alpha;
};



namespace __detail {

    // create_node
    template<typename tkey, typename tvalue, typename compare>
    template<class ...Args>
    binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node*
    bst_impl<tkey, tvalue, compare, SPG_TAG>::create_node(
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont,
            Args&& ...args)
    {
        typename scapegoat_tree<tkey, tvalue, compare>::node* n = cont._allocator.
                template new_object<typename scapegoat_tree<tkey, tvalue, compare>::node>(args ...);
        n->left_subtree = nullptr;
        n->right_subtree = nullptr;
        n->recalculate_size();
        return static_cast<binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node *>(n);
    }

    // delete_node
    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPG_TAG>::delete_node(
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont,
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node* node)
    {
        auto *spg_node = static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(node);
        cont._allocator.delete_object(spg_node);
    }

    // post_search
    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPG_TAG>::post_search(
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node** node_ptr)
    {
        // Empty as per requirements
    }

    // post_insert
    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPG_TAG>::post_insert(
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont,
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node** node_ptr)
    {
        auto spg = static_cast<scapegoat_tree<tkey, tvalue, compare>&>(cont).find_scapegoat(static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(*node_ptr));
        if (spg != nullptr){
            static_cast<scapegoat_tree<tkey, tvalue, compare>&>(cont).rebalance_from_scapegoat(spg);
        }
    }

    // erase
    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPG_TAG>::erase(
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>& cont,
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node** node_ptr)
    {
        auto *node_to_delete = *node_ptr;
        if (node_to_delete == nullptr) {
            throw std::out_of_range("Incorrect iterator for erase\n");
        }

        //node* prev = (std::prev(pos)).get_node();
        auto *parent = node_to_delete->parent;
        typename binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node *new_node = nullptr;
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
            typename binary_search_tree<tkey, tvalue, compare, SPG_TAG>::node *successor;

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
        __detail::bst_impl<tkey, tvalue, compare, SPG_TAG>::delete_node(cont, node_to_delete);
        *node_ptr = new_node;
        //end of binary tree erase

        auto spg = static_cast<scapegoat_tree<tkey, tvalue, compare>&>(cont).find_scapegoat(
                static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(new_node));
        if (spg != nullptr){
            static_cast<scapegoat_tree<tkey, tvalue, compare>&>(cont).rebalance_from_scapegoat(spg);
        }
    }

    // swap
    template<typename tkey, typename tvalue, typename compare>
    void bst_impl<tkey, tvalue, compare, SPG_TAG>::swap(
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>& lhs,
            binary_search_tree<tkey, tvalue, compare, SPG_TAG>& rhs) noexcept
    {
        // TODO: implement
    }

}

template<typename compare, typename U, typename iterator>
explicit scapegoat_tree(iterator begin, iterator end, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr, double alpha = 0.7) -> scapegoat_tree<const typename std::iterator_traits<iterator>::value_type::first_type, typename std::iterator_traits<iterator>::value_type::second_type, compare>;

template<typename compare, typename U, std::ranges::forward_range Range>
explicit scapegoat_tree(Range&& range, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr, double alpha = 0.7) -> scapegoat_tree<const typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::first_type, typename std::iterator_traits<typename std::ranges::iterator_t<Range>>::value_type::second_type, compare> ;

template<typename tkey, typename tvalue, typename compare, typename U>
scapegoat_tree(std::initializer_list<std::pair<tkey, tvalue>> data, const compare& cmp = compare(),
        pp_allocator<U> alloc = pp_allocator<U>(),
        logger* log = nullptr, double alpha = 0.7) -> scapegoat_tree<tkey, tvalue, compare>;

// region implementation
template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::node *scapegoat_tree<tkey, tvalue, compare>::find_scapegoat(scapegoat_tree::node *from) {
    auto cur_node = from;
    while(cur_node != nullptr){
        cur_node->recalculate_size();
        if (cur_node->is_disbalanced(_alpha)){
            return cur_node;
        }
        cur_node = static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(cur_node->parent);
    }
    return nullptr;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void scapegoat_tree<tkey, tvalue, compare>::rebalance_from_scapegoat(scapegoat_tree::node *scapegoat) {
    scapegoat_tree::node * cur_node = scapegoat;
    std::vector<scapegoat_tree::node*> nodes;
    while(cur_node->right_subtree != nullptr){
        cur_node = static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(cur_node->right_subtree);
    }
    // собираем все ноды в вектор
    typename parent::infix_iterator end_iter(cur_node);
    end_iter++;

    cur_node = scapegoat;
    while(cur_node->left_subtree != nullptr){
        cur_node = static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(cur_node->left_subtree);
    }
    typename parent::infix_iterator begin_iter(cur_node);
    while(begin_iter != end_iter){
        nodes.push_back(static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(begin_iter.get_node()));
        begin_iter++;
    }
    if (scapegoat->parent == nullptr){
        this->_root = rebalance_recursively(nodes);
        this->_root->parent = nullptr;
    } else {
        if (scapegoat == scapegoat->parent->left_subtree){
            auto par = scapegoat->parent;
            auto n = rebalance_recursively(nodes);
            par->left_subtree = n;
            n->parent = par;
        } else {
            auto par = scapegoat->parent;
            auto n = rebalance_recursively(nodes);
            par->right_subtree = n;
            n->parent = par;
        }
    }
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t scapegoat_tree<tkey, tvalue, compare>::get_mediana(std::vector<scapegoat_tree::node*> nodes) {
    return nodes.size() / 2;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::node *scapegoat_tree<tkey, tvalue, compare>::rebalance_recursively(std::vector<scapegoat_tree::node*> nodes) {
    if (nodes.size() == 0){
        return nullptr;
    }
    auto med = get_mediana(nodes);
    std::vector<scapegoat_tree::node*> left_sub(nodes.begin(), nodes.begin() + med);
    std::vector<scapegoat_tree::node*> right_sub(nodes.begin() + med + 1, nodes.end());
    nodes[med]->left_subtree = rebalance_recursively(left_sub);
    nodes[med]->right_subtree = rebalance_recursively(right_sub);
    if (nodes[med]->left_subtree != nullptr){
        nodes[med]->left_subtree->parent = nodes[med];
    }
    if (nodes[med]->right_subtree != nullptr){
        nodes[med]->right_subtree->parent = nodes[med];
    }
    static_cast<scapegoat_tree<tkey, tvalue, compare>::node*>(nodes[med])->recalculate_size();
    return nodes[med];
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::node::node(tkey const &key_, tvalue &&value_)
{
    parent::data.first = key_;
    parent::data.second = std::move(value_);
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::node::node(tkey const &key_, const tvalue& value_)
{
    parent::data.first = key_;
    parent::data.second = value_;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<class ...Args>
scapegoat_tree<tkey, tvalue, compare>::node::node(parent::node* par, Args&&... args)
: binary_search_tree<tkey, tvalue, compare, __detail::SPG_TAG>::node(par, args ...)
{
    size = 0;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void scapegoat_tree<tkey, tvalue, compare>::node::recalculate_size() noexcept
{
    size = get_left_size() + get_right_size() + 1;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
bool scapegoat_tree<tkey, tvalue, compare>::node::is_disbalanced(double alpha) noexcept
{
    if (get_left_size() > alpha * size || get_right_size() > alpha * size){
        return true;
    }
    return false;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t scapegoat_tree<tkey, tvalue, compare>::node::get_right_size() {
    auto n = static_cast<scapegoat_tree::node*>(this->right_subtree);
    return n == nullptr ? 0 : n->size;}

template<typename tkey, typename tvalue, compator<tkey> compare>
size_t scapegoat_tree<tkey, tvalue, compare>::node::get_left_size() {
    auto n = static_cast<scapegoat_tree::node*>(this->left_subtree);
    return n == nullptr ? 0 : n->size;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(
        const compare& comp,
        pp_allocator<value_type> alloc,
        logger *log,
        double alpha)
{
    parent::_allocator = alloc;
    parent::_logger = log;
    _alpha = alpha;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(
        pp_allocator<value_type> alloc,
        const compare& comp,
        logger *log,
        double alpha)
{
    parent::_allocator = alloc;
    parent::_logger = log;
    _alpha = alpha;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<input_iterator_for_pair<tkey, tvalue> iterator>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(
        iterator begin,
        iterator end,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log,
        double alpha)
{
    parent::_allocator = alloc;
    parent::_logger = log;
    _alpha = alpha;
    //todo
}

template<typename tkey, typename tvalue, compator<tkey> compare>
template<std::ranges::input_range Range>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(
        Range&& range,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log,
        double alpha)
{
//todo
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(
        std::initializer_list<std::pair<tkey, tvalue>> data,
        const compare& cmp,
        pp_allocator<value_type> alloc,
        logger* log,
        double alpha)
{
//todo
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::~scapegoat_tree() noexcept
{
//todo
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(scapegoat_tree const &other)
{
    while (parent::size() != 0) {
        erase(parent::begin_infix());
    }

    typename parent::prefix_const_iterator iter_other = other.begin_prefix();

    while (iter_other != other.end_prefix()) {
        insert(*iter_other);
        iter_other++;
    }

    *parent::_logger = *other._logger;
    parent::_allocator = other._allocator;
    _alpha = other._alpha;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare> &scapegoat_tree<tkey, tvalue, compare>::operator=(scapegoat_tree const &other)
{
    if (this == &other)
        return *this;

    while (parent::size() != 0) {
        erase(parent::begin_infix());
    }

    typename parent::prefix_const_iterator iter_other = other.begin_prefix();

    while (iter_other != other.end_prefix()) {
        insert(*iter_other);
        iter_other++;
    }

    *parent::_logger = *other._logger;
    parent::_allocator = other._allocator;
    _alpha = other._alpha;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare>::scapegoat_tree(scapegoat_tree &&other) noexcept
{
    parent::_root = other._root;
    parent::_logger = other._logger;
    parent::_allocator = other._allocator;
    parent::_size = other._size;
    other._root = nullptr;
    other._size = 0;
    _alpha = other._alpha;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
scapegoat_tree<tkey, tvalue, compare> &scapegoat_tree<tkey, tvalue, compare>::operator=(scapegoat_tree &&other) noexcept
{
    parent::_root = other._root;
    parent::_logger = other._logger;
    parent::_allocator = other._allocator;
    parent::_size = other._size;
    other._root = nullptr;
    other._size = 0;
    _alpha = other._alpha;
    return *this;
}

template<typename tkey, typename tvalue, compator<tkey> compare>
void scapegoat_tree<tkey, tvalue, compare>::setup_alpha(double alpha)
{
    _alpha = alpha;
}

// endregion implementation

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SCAPEGOAT_TREE_H