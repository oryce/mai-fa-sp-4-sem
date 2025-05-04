#include <not_implemented.h>
#include <cstddef>
#include "../include/allocator_buddies_system.h"
#include <format>

allocator_buddies_system::~allocator_buddies_system()
{
    if (_trusted_memory == nullptr)
    {
        return;
    }

    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    std::destroy_at(&metadata->mutex);
    metadata->allocator->deallocate(_trusted_memory, metadata->size() + sizeof(allocator_metadata));
}

allocator_buddies_system::allocator_buddies_system(
    allocator_buddies_system &&other) noexcept
{
    _trusted_memory = std::exchange(other._trusted_memory, nullptr);
}

allocator_buddies_system &allocator_buddies_system::operator=(
    allocator_buddies_system &&other) noexcept
{
    if (this != &other)
    {
        std::swap(_trusted_memory, other._trusted_memory);
    }

    return *this;
}

allocator_buddies_system::allocator_buddies_system(
    size_t space_size_power_of_two,
    std::pmr::memory_resource *parent_allocator,
    logger *logger,
    allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    size_t k = __detail::nearest_greater_k_of_2(space_size_power_of_two);

    if (k < min_k)
    {
        throw std::logic_error("space size is too small");
    }

    std::pmr::memory_resource *allocator = parent_allocator == nullptr
                                               ? std::pmr::get_default_resource()
                                               : parent_allocator;
    _trusted_memory = allocator->allocate(space_size_power_of_two + sizeof(allocator_metadata));

    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    metadata->logger = logger;
    metadata->allocator = allocator;
    metadata->fit_mode = allocate_fit_mode;
    metadata->size_k = k;

    std::construct_at(&metadata->mutex);

    auto first_block = reinterpret_cast<block_metadata *>(
        static_cast<std::byte *>(_trusted_memory) + sizeof(allocator_metadata));

    first_block->occupied = false;
    first_block->size_k = k - min_k;
}

[[nodiscard]] void *allocator_buddies_system::do_allocate_sm(
    size_t size)
{
    debug_with_guard("[>] entering allocator_buddies_system::do_allocate_sm");

    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    std::lock_guard<std::mutex> lock(metadata->mutex);

    size_t size_with_metadata = size + sizeof(occupied_block_metadata_size);
    block_metadata *block = nullptr;

    debug_with_guard(std::format("[*] allocating {} bytes", size_with_metadata));

    switch (metadata->fit_mode)
    {
    case allocator_with_fit_mode::fit_mode::first_fit:
        block = get_block_first_fit(size_with_metadata);
        break;
    case allocator_with_fit_mode::fit_mode::the_best_fit:
        block = get_block_best_fit(size_with_metadata);
        break;
    case allocator_with_fit_mode::fit_mode::the_worst_fit:
        block = get_block_worst_fit(size_with_metadata);
        break;
    }

    if (block == nullptr)
    {
        error_with_guard(std::format("[!] out of memory: requested {} bytes", size));
        throw std::bad_alloc();
    }

    // Разбиваем блоки
    while (block->size_k > 0 && block->block_size() >= size_with_metadata * 2)
    {
        --block->size_k;

        auto buddy = get_buddy(block);
        buddy->occupied = false;
        buddy->size_k = block->size_k;
    }

    if (block->block_size() != size_with_metadata)
    {
        warning_with_guard(std::format("[!] changed allocation size to {} bytes", size_with_metadata));
    }

    block->occupied = true;

    auto tm_ptr = reinterpret_cast<void **>(block + 1);
    *tm_ptr = _trusted_memory;

    auto allocated_block = static_cast<void *>(
        reinterpret_cast<std::byte *>(block) + occupied_block_metadata_size);

    information_with_guard(std::format("[+] allocated {} bytes at {}, available memory: {} bytes",
                                       size_with_metadata, allocated_block, available_memory()));
    debug_with_guard(std::format("[*] current blocks: \n{}", print_blocks()));
    debug_with_guard("[<] leaving allocator_buddies_system::do_allocate_sm");

    return allocated_block;
}

void allocator_buddies_system::do_deallocate_sm(void *at)
{
    debug_with_guard("[>] entering allocator_buddies_system::do_deallocate_sm");

    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    std::lock_guard<std::mutex> lock(metadata->mutex);

    debug_with_guard(std::format("[*] deallocating block at {}", at));

    auto block = reinterpret_cast<block_metadata *>(static_cast<std::byte *>(at) - occupied_block_metadata_size);

    {
        auto tm_ptr = reinterpret_cast<void **>(block + 1);
        if (*tm_ptr != _trusted_memory)
        {
            error_with_guard(std::format("[!] block is not allocated by this allocator"));
            throw std::logic_error("foreign block");
        }
    }

    block->occupied = false;

    auto buddy = get_buddy(block);

    // Сливаем соседние свободные блоки
    while (block->block_size() < metadata->size() && block->block_size() == buddy->block_size() && !buddy->occupied)
    {
        // Берём блок, который "выше" в памяти
        if (buddy < block)
        {
            std::swap(block, buddy);
        }

        ++block->size_k;
        buddy = get_buddy(block);
    }

    information_with_guard(std::format("[+] deallocated block at {}, available memory: {} bytes",
                                       at, available_memory()));
    debug_with_guard(std::format("[*] current blocks: \n{}", print_blocks()));
    debug_with_guard("[<] leaving allocator_buddies_system::do_deallocate_sm");
}

bool allocator_buddies_system::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}

inline void allocator_buddies_system::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    debug_with_guard("[>] entering allocator_buddies_system::set_fit_mode");

    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    std::lock_guard<std::mutex> lock(metadata->mutex);

    {
        const char *mode_string;
        switch (mode)
        {
        case fit_mode::first_fit:
            mode_string = "first_fit";
            break;
        case fit_mode::the_best_fit:
            mode_string = "the_best_fit";
            break;
        case fit_mode::the_worst_fit:
            mode_string = "the_worst_fit";
            break;
        default:
            throw std::invalid_argument("invalid fit mode");
        }
        debug_with_guard(std::format("[*] changing fit mode to {}", mode_string));
    }

    metadata->fit_mode = mode;

    debug_with_guard("[<] leaving allocator_buddies_system::set_fit_mode");
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info() const noexcept
{
    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    std::lock_guard<std::mutex> lock(metadata->mutex);
    return get_blocks_info_inner();
}

inline logger *allocator_buddies_system::get_logger() const
{
    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    return metadata->logger;
}

inline std::string allocator_buddies_system::get_typename() const
{
    return "allocator_buddies_system";
}

std::vector<allocator_test_utils::block_info> allocator_buddies_system::get_blocks_info_inner() const
{
    std::vector<allocator_test_utils::block_info> blocks;

    for (auto it = begin(), it_end = end(); it != it_end; ++it)
    {
        blocks.push_back({it.size(), it.occupied()});
    }

    return blocks;
}

allocator_buddies_system::block_metadata *allocator_buddies_system::get_block_first_fit(size_t size) const
{
    for (auto it = begin(), it_end = end(); it != it_end; ++it)
    {
        if (!it.occupied() && it.size() >= size)
        {
            return reinterpret_cast<block_metadata *>(*it);
        }
    }

    return nullptr;
}

allocator_buddies_system::block_metadata *allocator_buddies_system::get_block_best_fit(size_t size) const
{
    block_metadata *best_fit = nullptr;

    for (auto it = begin(), it_end = end(); it != it_end; ++it)
    {
        if (!it.occupied() && it.size() >= size && (!best_fit || it.size() < best_fit->block_size()))
        {
            best_fit = reinterpret_cast<block_metadata *>(*it);
        }
    }

    return best_fit;
}

allocator_buddies_system::block_metadata *allocator_buddies_system::get_block_worst_fit(size_t size) const
{
    block_metadata *worst_fit = nullptr;

    for (auto it = begin(), it_end = end(); it != it_end; ++it)
    {
        if (!it.occupied() && it.size() >= size)
        {
            worst_fit = reinterpret_cast<block_metadata *>(*it);
        }
    }

    return worst_fit;
}

allocator_buddies_system::block_metadata *allocator_buddies_system::get_buddy(block_metadata *block) const
{
    size_t block_offset = reinterpret_cast<std::byte *>(block) -
                          static_cast<std::byte *>(_trusted_memory) -
                          sizeof(allocator_metadata);

    size_t buddy_offset = block_offset ^ block->block_size();

    return reinterpret_cast<block_metadata *>(
        static_cast<std::byte *>(_trusted_memory) +
        sizeof(allocator_metadata) +
        buddy_offset);
}

size_t allocator_buddies_system::available_memory() const noexcept
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

allocator_buddies_system::buddy_iterator allocator_buddies_system::begin() const noexcept
{
    return buddy_iterator(static_cast<std::byte *>(_trusted_memory) + sizeof(allocator_metadata));
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::end() const noexcept
{
    auto metadata = reinterpret_cast<allocator_metadata *>(_trusted_memory);
    return buddy_iterator(static_cast<std::byte *>(_trusted_memory) + sizeof(allocator_metadata) + metadata->size());
}

bool allocator_buddies_system::buddy_iterator::operator==(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block == other._block;
}

bool allocator_buddies_system::buddy_iterator::operator!=(const allocator_buddies_system::buddy_iterator &other) const noexcept
{
    return _block != other._block;
}

allocator_buddies_system::buddy_iterator &allocator_buddies_system::buddy_iterator::operator++() & noexcept
{
    auto block = reinterpret_cast<block_metadata *>(_block);
    _block = static_cast<std::byte *>(_block) + block->block_size();
    return *this;
}

allocator_buddies_system::buddy_iterator allocator_buddies_system::buddy_iterator::operator++(int n)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_buddies_system::buddy_iterator::size() const noexcept
{
    auto block = reinterpret_cast<block_metadata *>(_block);
    return block->block_size();
}

bool allocator_buddies_system::buddy_iterator::occupied() const noexcept
{
    auto block = reinterpret_cast<block_metadata *>(_block);
    return block->occupied;
}

void *allocator_buddies_system::buddy_iterator::operator*() const noexcept
{
    return _block;
}

allocator_buddies_system::buddy_iterator::buddy_iterator(void *start)
    : _block(start)
{
}

allocator_buddies_system::buddy_iterator::buddy_iterator()
    : _block(nullptr)
{
}
