#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H

#include <pp_allocator.h>
#include <allocator_test_utils.h>
#include <allocator_with_fit_mode.h>
#include <logger_guardant.h>
#include <typename_holder.h>
#include <mutex>

class allocator_red_black_tree final:
    public smart_mem_resource,
    public allocator_test_utils,
    public allocator_with_fit_mode,
    private logger_guardant,
    private typename_holder
{

private:

    enum class block_color : unsigned char
    { RED, BLACK };

    /* Отключаем выравнивание для структур блоков, иначе не будут проходить тесты. */
#ifdef _MSC_VER
#   pragma pack(push, 1)
#endif

    struct block_metadata
    {
        bool occupied : 4;
        block_color color : 4;

        block_metadata* back_;
        block_metadata* forward_;
        /** Если |occupied| == true, это поле будет указывать на доверенную
         * область памяти. Иначе на родителя в КЧД. */
        void* parent_;

        size_t get_size(void* trusted_memory)
        {
            if (forward_ != nullptr)
            {
                return reinterpret_cast<std::byte*>(forward_)
                    - reinterpret_cast<std::byte*>(this)
                    - sizeof(block_metadata);
            }
            else
            {
                const auto* alloc = static_cast<allocator_metadata*>(trusted_memory);

                return static_cast<std::byte*>(trusted_memory)
                    + sizeof(allocator_metadata)
                    + alloc->size_
                    - reinterpret_cast<std::byte*>(this)
                    - sizeof(block_metadata);
            }
        }
#if defined(__GNUC__) || defined(__clang__)
    } __attribute__((packed));
#else
    };
#endif

    struct free_block_metadata : block_metadata
    {
        free_block_metadata* left_;
        free_block_metadata* right_;

        free_block_metadata* get_parent() const
        {
            return static_cast<free_block_metadata*>(parent_);
        }

        free_block_metadata* get_grandparent() const
        {
            const free_block_metadata* parent = get_parent();
            return parent ? parent->get_parent() : nullptr;
        }

        free_block_metadata* get_sibling() const
        {
            const free_block_metadata* parent = get_parent();
            if (!parent) return nullptr;
            return this == parent->left_ ? parent->right_ : parent->left_;
        }
#if defined(__GNUC__) || defined(__clang__)
    } __attribute__((packed));
#else
    };
#endif

#ifdef _MSC_VER
#   pragma pack(pop)
#endif

    struct allocator_metadata
    {
        logger* logger_;
        memory_resource* parent_allocator_;
        fit_mode fit_mode_;
        size_t size_;
        std::mutex mutex_;
        free_block_metadata* root_;
    };

    void *_trusted_memory;

public:
    
    ~allocator_red_black_tree() override;
    
    allocator_red_black_tree(
        allocator_red_black_tree const &other) = delete;
    
    allocator_red_black_tree &operator=(
        allocator_red_black_tree const &other) = delete;
    
    allocator_red_black_tree(
        allocator_red_black_tree &&other) noexcept;
    
    allocator_red_black_tree &operator=(
        allocator_red_black_tree &&other) noexcept;

public:
    
    explicit allocator_red_black_tree(
            size_t space_size,
            std::pmr::memory_resource *parent_allocator = nullptr,
            logger *logger = nullptr,
            allocator_with_fit_mode::fit_mode allocate_fit_mode = allocator_with_fit_mode::fit_mode::first_fit);

public:
    
    [[nodiscard]] void *do_allocate_sm(
        size_t size) override;
    
    void do_deallocate_sm(
        void *at) override;

    bool do_is_equal(const std::pmr::memory_resource&) const noexcept override;

    std::vector<allocator_test_utils::block_info> get_blocks_info() const override;
    
    inline void set_fit_mode(allocator_with_fit_mode::fit_mode mode) override;

    inline logger *get_logger() const override;

private:

    std::vector<allocator_test_utils::block_info> get_blocks_info_inner() const override;

    inline std::string get_typename() const noexcept override;

    allocator_metadata* get_metadata() const noexcept
    {
        return static_cast<allocator_metadata*>(_trusted_memory);
    }

    inline free_block_metadata* get_first_free_block(size_t size) const noexcept;

    inline free_block_metadata* get_best_free_block(size_t size) const noexcept;

    inline free_block_metadata* get_worst_free_block(size_t size) const noexcept;

    inline size_t available_memory() const noexcept;

    void rb_tree_insert(free_block_metadata* z);

    void rb_tree_remove(free_block_metadata* z);

    inline void rb_small_left_rotation(free_block_metadata *&subtree_root);

    inline void rb_small_right_rotation(free_block_metadata *&subtree_root);

    class rb_iterator
    {
        block_metadata* _block_ptr;
        void* _trusted;

    public:

        using iterator_category = std::forward_iterator_tag;
        using value_type = void*;
        using reference = void*&;
        using pointer = void**;
        using difference_type = ptrdiff_t;

        bool operator==(const rb_iterator&) const noexcept;

        bool operator!=(const rb_iterator&) const noexcept;

        rb_iterator& operator++() & noexcept;

        rb_iterator operator++(int n);

        size_t size() const noexcept;

        block_metadata* operator*() const noexcept;

        bool occupied()const noexcept;

        rb_iterator();

        rb_iterator(void* trusted);
    };

    friend class rb_iterator;

    rb_iterator begin() const noexcept;
    rb_iterator end() const noexcept;

};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_ALLOCATOR_ALLOCATOR_RED_BLACK_TREE_H