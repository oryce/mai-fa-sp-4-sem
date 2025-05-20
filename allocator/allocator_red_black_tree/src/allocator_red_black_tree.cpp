#include <not_implemented.h>
#include <format>

#include "../include/allocator_red_black_tree.h"

allocator_red_black_tree::~allocator_red_black_tree()
{
    if (_trusted_memory == nullptr)
    {
        return;
    }

    allocator_metadata* alloc = get_metadata();
    std::destroy_at(&alloc->mutex_);
    alloc->parent_allocator_->deallocate(_trusted_memory, sizeof(allocator_metadata) + alloc->size_);
}

allocator_red_black_tree::allocator_red_black_tree(
    allocator_red_black_tree &&other) noexcept
{
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_red_black_tree &allocator_red_black_tree::operator=(
    allocator_red_black_tree &&other) noexcept
{
    if (this != &other)
    {
        std::swap(_trusted_memory, other._trusted_memory);
    }
    return *this;
}

allocator_red_black_tree::allocator_red_black_tree(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    if (space_size == 0)
    {
        throw std::logic_error("incorrect space size");
    }

    const auto allocator = parent_allocator ? parent_allocator : std::pmr::get_default_resource();

    _trusted_memory = allocator->allocate(sizeof(allocator_metadata) + space_size);

    auto* alloc = static_cast<allocator_metadata*>(_trusted_memory);

    alloc->logger_ = logger;
    alloc->parent_allocator_ = allocator;
    alloc->fit_mode_ = allocate_fit_mode;
    alloc->size_ = space_size;
    std::construct_at(&alloc->mutex_);

    auto* first_block = reinterpret_cast<free_block_metadata*>(
        static_cast<std::byte*>(_trusted_memory) + sizeof(allocator_metadata));

    first_block->occupied = false;
    first_block->color = block_color::BLACK;
    first_block->back_ = nullptr;
    first_block->forward_ = nullptr;
    first_block->parent_ = nullptr;
    first_block->left_ = nullptr;
    first_block->right_ = nullptr;

    alloc->root_ = first_block;
}

bool allocator_red_black_tree::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}

[[nodiscard]] void *allocator_red_black_tree::do_allocate_sm(
    size_t size)
{
    debug_with_guard("[>] entering allocator_red_black_tree::do_allocate_sm");

    allocator_metadata* alloc = get_metadata();
    std::lock_guard guard(alloc->mutex_);

    debug_with_guard(std::format("[*] allocating {} bytes", size));
    free_block_metadata* taken_block = nullptr;

    switch (alloc->fit_mode_)
    {
    case fit_mode::first_fit:
        taken_block = get_first_free_block(size);
        break;
    case fit_mode::the_best_fit:
        taken_block = get_best_free_block(size);
        break;
    case fit_mode::the_worst_fit:
        taken_block = get_worst_free_block(size);
        break;
    }

    if (taken_block == nullptr)
    {
        error_with_guard(std::format("[!] out of memory: requested {} bytes", size));
        throw std::bad_alloc();
    }

    rb_tree_remove(taken_block);
    taken_block->occupied = true;
    taken_block->parent_ = _trusted_memory;

    size_t required_size = size + sizeof(block_metadata);

    const bool can_split = taken_block->get_size(_trusted_memory)
        >= required_size + sizeof(free_block_metadata);

    if (can_split)
    {
        auto* new_block = reinterpret_cast<free_block_metadata*>(
            reinterpret_cast<std::byte*>(taken_block) + required_size);

        // Подвязываем блоки в двусвязном списке
        new_block->forward_ = taken_block->forward_;
        new_block->back_ = taken_block;
        taken_block->forward_ = new_block;

        if (new_block->forward_)
        {
            new_block->forward_->back_ = new_block;
        }

        // Вставляем свободный блок в КЧД
        new_block->occupied = false;
        new_block->parent_ = nullptr;
        rb_tree_insert(new_block);
    }

    auto allocated_block = static_cast<void*>(
            reinterpret_cast<std::byte*>(taken_block) + sizeof(block_metadata));

    information_with_guard(std::format("[+] allocated {} bytes at {}, available memory: {} bytes",
                                       size, allocated_block, available_memory()));
    debug_with_guard(std::format("[*] current blocks: \n{}", print_blocks()));
    debug_with_guard("[<] leaving allocator_red_black_tree::do_allocate_sm");

    return allocated_block;
}


void allocator_red_black_tree::do_deallocate_sm(
    void *at)
{
    allocator_metadata* alloc = get_metadata();
    std::lock_guard guard(alloc->mutex_);

    debug_with_guard(std::format("[*] deallocating block at {}", at));

    auto* block = reinterpret_cast<block_metadata*>(
        static_cast<std::byte*>(at) - sizeof(block_metadata));

    if (block->parent_ != _trusted_memory)
    {
        error_with_guard(std::format("[!] block is not owned by this allocator"));
        throw std::logic_error("foreign block");
    }

    block->occupied = false;

    if (block->back_ && !block->back_->occupied)
    {
        // Сливаем предыдущий блок с текущим: удаляем из дерева свободных блоков,
        // подвязывем со следующим текущего, ставим на место текущего.
        auto* back = static_cast<free_block_metadata*>(block->back_);

        rb_tree_remove(back);

        back->forward_ = block->forward_;
        if (back->forward_) back->forward_->back_ = back;

        block = back;
    }

    if (block->forward_ && !block->forward_->occupied)
    {
        // Аналогично сливаем со следующим.
        auto* fwd = static_cast<free_block_metadata*>(block->forward_);

        rb_tree_remove(fwd);

        block->forward_ = fwd->forward_;
        if (block->forward_) block->forward_->back_ = block;
    }

    rb_tree_insert(static_cast<free_block_metadata*>(block));

    information_with_guard(std::format("[+] deallocated block at {}, available memory: {} bytes",
                                       at, available_memory()));
    debug_with_guard(std::format("[*] current blocks: \n{}", print_blocks()));
    debug_with_guard("[<] leaving allocator_red_black_tree::do_deallocate_sm");
}

void allocator_red_black_tree::set_fit_mode(allocator_with_fit_mode::fit_mode mode)
{
    allocator_metadata* alloc = get_metadata();
    std::lock_guard guard(alloc->mutex_);
    alloc->fit_mode_ = mode;
}


std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info() const
{
    allocator_metadata* alloc = get_metadata();
    std::lock_guard guard(alloc->mutex_);
    return get_blocks_info_inner();
}

inline logger *allocator_red_black_tree::get_logger() const
{
    allocator_metadata* alloc = get_metadata();
    return alloc->logger_;
}

std::vector<allocator_test_utils::block_info> allocator_red_black_tree::get_blocks_info_inner() const
{
    std::vector<block_info> blocks;
    for (auto it = begin(), it_end = end(); it != it_end; ++it)
    {
        blocks.push_back({it.size(), it.occupied()});
    }
    return blocks;
}

inline std::string allocator_red_black_tree::get_typename() const noexcept
{
    return "allocator_red_black_tree";
}

allocator_red_black_tree::free_block_metadata* allocator_red_black_tree::get_first_free_block(size_t size) const noexcept
{
    allocator_metadata* alloc = get_metadata();

    for (free_block_metadata* block = alloc->root_;
         block != nullptr; block = block->right_)
    {
        if (block->get_size(_trusted_memory) >= size)
        {
            return block;
        }
    }

    return nullptr;
}

allocator_red_black_tree::free_block_metadata* allocator_red_black_tree::get_best_free_block(size_t size) const noexcept
{
    allocator_metadata* alloc = get_metadata();

    free_block_metadata* block = alloc->root_;
    free_block_metadata* best = nullptr;

    while (block)
    {
        size_t block_size = block->get_size(_trusted_memory);

        if (block_size >= size)
        {
            best = block;
        }

        // Для наиболее подходящего идём в левое поддерево
        if (block_size < size)
        {
            block = block->right_;
        }
        else if (block_size > size)
        {
            block = block->left_;
        }
        else
        {
            break;
        }
    }

    return best;
}

allocator_red_black_tree::free_block_metadata* allocator_red_black_tree::get_worst_free_block(
    size_t size) const noexcept
{
    allocator_metadata* alloc = get_metadata();

    free_block_metadata* block = alloc->root_;
    free_block_metadata* worst = nullptr;

    while (block)
    {
        if (block->get_size(_trusted_memory) >= size)
        {
            worst = block;
        }
        // Для наименее подходящего всегда спускаемся в правое поддерево
        block = block->right_;
    }

    return worst;
}

size_t allocator_red_black_tree::available_memory() const noexcept
{
    size_t available = 0;

    for (auto it = begin(), it_end = end(); it != it_end; ++it)
    {
        if (!it.occupied())
        {
            available += it.size();
        }
    }

    return available;
}

void allocator_red_black_tree::rb_tree_insert(free_block_metadata* z)
{
    allocator_metadata* alloc = get_metadata();

    z->left_ = nullptr;
    z->right_ = nullptr;
    z->color = block_color::RED;

    free_block_metadata* parent = nullptr;

    // Ищем, куда вставить блок.
    for (free_block_metadata* node = alloc->root_; node;)
    {
        if (z->get_size(_trusted_memory) < node->get_size(_trusted_memory))
        {
            parent = node;
            node = node->left_;
        }
        else
        {
            parent = node;
            node = node->right_;
        }
    }

    // Вставляем блок.
    z->parent_ = parent;

    if (parent == nullptr)
    {
        alloc->root_ = z;
    }
    else if (z->get_size(_trusted_memory) < parent->get_size(_trusted_memory))
    {
        parent->left_ = z;
    }
    else
    {
        parent->right_ = z;
    }

    // Костыль, чтобы исправлять ссылки у родителей.
    auto link_to = [&](free_block_metadata *n) -> free_block_metadata *& {
        if (n->get_parent() == nullptr) return alloc->root_;
        return (n == n->get_parent()->left_)
                   ? n->get_parent()->left_
                   : n->get_parent()->right_;
    };

    while (z != alloc->root_ && z->get_parent()->color == block_color::RED)
    {
        free_block_metadata* parent = z->get_parent();
        free_block_metadata* grand = z->get_grandparent();

        if (parent == grand->left_)
        {
            free_block_metadata* uncle = grand->right_;

            // 1. "y is red" -----------------------------------------------------------------------
            if (uncle && uncle->color == block_color::RED)
            {
                parent->color = block_color::BLACK;
                uncle->color = block_color::BLACK;
                grand->color = block_color::RED;
                z = grand;
            }
            else
            {
                // 2. "y is black, z is a right child" ---------------------------------------------
                if (z == parent->right_)
                {
                    z = parent;
                    rb_small_left_rotation(link_to(parent));
                    parent = z->get_parent();
                }

                // 3. "y is black, z is a left child" ----------------------------------------------
                parent->color = block_color::BLACK;
                grand->color = block_color::RED;
                rb_small_right_rotation(link_to(grand));
            }
        }
        else /* parent == grand->get_right() */
        {
            free_block_metadata* uncle = grand->left_;

            // 1. "y is red" -----------------------------------------------------------------------
            if (uncle && uncle->color == block_color::RED)
            {
                parent->color = block_color::BLACK;
                uncle->color = block_color::BLACK;
                grand->color = block_color::RED;
                z = grand;
            }
            else
            {
                // 2. "y is black, z is a left child" ----------------------------------------------
                if (z == parent->left_)
                {
                    z = parent;
                    rb_small_right_rotation(link_to(parent));
                    parent = z->get_parent();
                }

                // 3. "y is black, z is a right child" ---------------------------------------------
                parent->color = block_color::BLACK;
                grand->color = block_color::RED;
                rb_small_left_rotation(link_to(grand));
            }
        }
    }

    alloc->root_->color = block_color::BLACK;
}

void allocator_red_black_tree::rb_tree_remove(free_block_metadata* z)
{
    allocator_metadata* alloc = get_metadata();

    // Хелперы, чтобы NIL-узлы были чёрными.
    auto color = [](free_block_metadata* p) -> block_color
    {
        return p ? p->color : block_color::BLACK;
    };

    auto child_is_black = [&](free_block_metadata* p, bool right) -> bool
    {
        free_block_metadata* c = right
                                     ? (p ? p->right_ : nullptr)
                                     : (p ? p->left_ : nullptr);
        return color(c) == block_color::BLACK;
    };

    // Костыль, чтобы исправлять ссылки у родителей.
    auto link_to = [&](free_block_metadata* n) -> free_block_metadata*&
    {
        if (n->get_parent() == nullptr) return alloc->root_;
        return (n == n->get_parent()->left_)
                   ? n->get_parent()->left_
                   : n->get_parent()->right_;
    };

    auto transplant = [&](free_block_metadata* u, free_block_metadata* v)
    {
        link_to(u) = v;
        if (v) v->parent_ = u->parent_;
    };

    auto maximum = [](free_block_metadata* x) -> free_block_metadata*
    {
        while (x->right_)
        {
            x = x->right_;
        }
        return x;
    };

    free_block_metadata* y = z;         // Удаляемая нода
    block_color oc = y->color;          // Цвет удаляемой ноды
    free_block_metadata* x = nullptr;   // Поддерево заменяющей ноды

    if (!z->left_ || !z->right_)
    {
        x = z->left_ ? z->left_ : z->right_;
        transplant(z, x);
    }
    else
    {
        y = maximum(z->left_);
        oc = y->color;
        x = y->left_;

        if (y->parent_ == z)
        {
            if (x) x->parent_ = y;
        }
        else
        {
            transplant(y, x);
            y->left_ = z->left_;
            y->left_->parent_ = y;
        }

        transplant(z, y);
        y->right_ = z->right_;
        y->right_->parent_ = y;
        y->color = z->color;
    }

    // Восстанавливаем свойства КЧД.

    if (oc == block_color::BLACK)
    {
        while (x != alloc->root_ && color(x) == block_color::BLACK)
        {
            free_block_metadata* xp = x ? x->get_parent() : nullptr;

            if (xp == nullptr) // В корне
            {
                if (x) x->color = block_color::BLACK;
                break;
            }

            free_block_metadata* w = x->get_sibling();

            // Если нет родственника, обрабатываем этот случай как №2.
            if (w == nullptr)
            {
                x = xp;
                continue;
            }

            if (x == xp->left_)
            {
                // 1. "w" is red" ------------------------------------------------------------------
                if (color(w) == block_color::RED)
                {
                    w->color = block_color::BLACK;
                    xp->color = block_color::RED;
                    rb_small_left_rotation(link_to(xp));
                    w = xp->right_;
                }

                // 2. "w is black and both of w's children are black" ------------------------------
                if (child_is_black(w, false) && child_is_black(w, true))
                {
                    if (w) w->color = block_color::RED;
                    x = xp;
                    continue;
                }

                // 3. "w is black, w's left child is red, and w's right child is black" ------------
                if (!child_is_black(w, false) && child_is_black(w, true))
                {
                    w->left_->color = block_color::BLACK;
                    w->color = block_color::RED;
                    rb_small_right_rotation(link_to(w));
                    w = xp->right_;
                }

                // 4. "w is black, w's left child is black, and w's right child is red" ------------
                w->color = xp->color;
                xp->color = block_color::BLACK;
                if (free_block_metadata* wr = w->right_)
                    wr->color = block_color::BLACK;
                rb_small_left_rotation(link_to(xp));
                break;
            }
            else /* x == xp->get_right() */
            {
                // 1. "w" is red" ------------------------------------------------------------------
                if (color(w) == block_color::RED)
                {
                    w->color = block_color::BLACK;
                    xp->color = block_color::RED;
                    rb_small_right_rotation(link_to(xp));
                    w = xp->left_;
                }

                // 2. "w is black and both of w's children are black" ------------------------------
                if (child_is_black(w, false) && child_is_black(w, true))
                {
                    if (w) w->color = block_color::RED;
                    x = xp;
                    continue;
                }

                // 3. "w is black, w's left child is black, and w's right child is red" ------------
                if (!child_is_black(w, true) && child_is_black(w, false))
                {
                    w->right_->color = block_color::BLACK;
                    w->color = block_color::RED;
                    rb_small_left_rotation(link_to(xp));
                    w = xp->left_;
                }

                // 4. "w is black, w's left child is red, and w's right child is black" ------------
                w->color = xp->color;
                xp->color = block_color::BLACK;
                w->left_->color = block_color::BLACK;
                rb_small_right_rotation(link_to(xp));
                break;
            }
        }

        if (x) x->color = block_color::BLACK;
    }
}

void allocator_red_black_tree::rb_small_left_rotation(free_block_metadata *&subtree_root)
{
    if (subtree_root == nullptr || subtree_root->right_ == nullptr) {
        return;
    }

    free_block_metadata* a = subtree_root;
    free_block_metadata* a_r = a->right_;
    free_block_metadata* a_r_l = a_r->left_;

    if (auto* parent = static_cast<free_block_metadata*>(a->parent_))
    {
        if (a == parent->left_)
            parent->left_ = a_r;
        else
            parent->right_ = a_r;
    }

    a_r->parent_ = a->parent_;
    a_r->left_ = a;
    a->parent_ = a_r;
    a->right_ = a_r_l;
    if (a_r_l != nullptr) a_r_l->parent_ = a;

    subtree_root = a_r;
}

void allocator_red_black_tree::rb_small_right_rotation(free_block_metadata *&subtree_root)
{
    if (subtree_root == nullptr || subtree_root->left_ == nullptr) {
        return;
    }

    free_block_metadata* a = subtree_root;
    free_block_metadata* a_l = a->left_;
    free_block_metadata* a_l_r = a_l->right_;

    if (auto* parent = static_cast<free_block_metadata*>(a->parent_))
    {
        if (a == parent->left_)
            parent->left_ = a_l;
        else
            parent->right_ = a_l;
    }

    a_l->parent_ = a->parent_;
    a_l->right_ = a;
    a->parent_ = a_l;
    a->left_ = a_l_r;
    if (a_l_r != nullptr) a_l_r->parent_ = a;

    subtree_root = a_l;
}


allocator_red_black_tree::rb_iterator allocator_red_black_tree::begin() const noexcept
{
    return {_trusted_memory};
}

allocator_red_black_tree::rb_iterator allocator_red_black_tree::end() const noexcept
{
    return {};
}


bool allocator_red_black_tree::rb_iterator::operator==(const allocator_red_black_tree::rb_iterator &other) const noexcept
{
    return _block_ptr == other._block_ptr;
}

bool allocator_red_black_tree::rb_iterator::operator!=(const allocator_red_black_tree::rb_iterator &other) const noexcept
{
    return !(*this == other);
}

allocator_red_black_tree::rb_iterator &allocator_red_black_tree::rb_iterator::operator++() & noexcept
{
    if (_block_ptr == nullptr)
    {
        return *this;
    }

    _block_ptr = _block_ptr->forward_;
    return *this;
}

allocator_red_black_tree::rb_iterator allocator_red_black_tree::rb_iterator::operator++(int n)
{
    auto prev = *this;
    ++(*this);
    return prev;
}

size_t allocator_red_black_tree::rb_iterator::size() const noexcept
{
    if (_block_ptr == nullptr)
    {
        return -1;
    }

    return _block_ptr->get_size(_trusted);
}

allocator_red_black_tree::block_metadata *allocator_red_black_tree::rb_iterator::operator*() const noexcept
{
    return _block_ptr;
}

allocator_red_black_tree::rb_iterator::rb_iterator()
    : _block_ptr(nullptr), _trusted(nullptr)
{
}

allocator_red_black_tree::rb_iterator::rb_iterator(void *trusted)
{
    _block_ptr = reinterpret_cast<block_metadata*>(
        static_cast<std::byte*>(trusted) + sizeof(allocator_metadata));
    _trusted = trusted;
}

bool allocator_red_black_tree::rb_iterator::occupied() const noexcept
{
   return _block_ptr->occupied;
}
