#include <not_implemented.h>
#include "../include/allocator_sorted_list.h"

allocator_sorted_list::~allocator_sorted_list()
{
    auto parent_allocator = get_parent_allocator();
    size_t total_size = allocator_metadata_size + get_allocator_size();

    if (parent_allocator) {
        parent_allocator->deallocate(_trusted_memory, total_size);
    } else {
        ::operator delete(_trusted_memory);
    }
}

allocator_sorted_list::allocator_sorted_list(
    allocator_sorted_list &&other) noexcept
{
    throw not_implemented("allocator_sorted_list::allocator_sorted_list(allocator_sorted_list &&) noexcept", "your code should be here...");
}

allocator_sorted_list &allocator_sorted_list::operator=(
    allocator_sorted_list &&other) noexcept
{
    throw not_implemented("allocator_sorted_list &allocator_sorted_list::operator=(allocator_sorted_list &&) noexcept", "your code should be here...");
}

allocator_sorted_list::allocator_sorted_list(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode)
{
    size_t total_size = allocator_metadata_size + space_size;
    _trusted_memory = parent_allocator ?
                      parent_allocator->allocate(total_size) :
                      ::operator new(total_size);

    // Инициализация метаданных
    *reinterpret_cast<class logger**>(_trusted_memory) = logger;
    *reinterpret_cast<std::pmr::memory_resource**>(
            reinterpret_cast<char*>(_trusted_memory) + sizeof(class logger*)) = parent_allocator;
    *reinterpret_cast<fit_mode*>(
            reinterpret_cast<char*>(_trusted_memory) + sizeof(class logger*) + sizeof(std::pmr::memory_resource*)) = allocate_fit_mode;
    *reinterpret_cast<size_t*>(
            reinterpret_cast<char*>(_trusted_memory) + sizeof(class logger*) + sizeof(std::pmr::memory_resource*) + sizeof(fit_mode)) = space_size;

    //new(adr) constructor() - создание объекта по адресу adr
    new(reinterpret_cast<std::mutex*>((char*)_trusted_memory +
        sizeof(class logger*) + sizeof(std::pmr::memory_resource*) +
        sizeof(fit_mode) + sizeof(size_t))) std::mutex();

    void* first_block = reinterpret_cast<char*>(_trusted_memory) + allocator_metadata_size;
    *reinterpret_cast<void**>(first_block) = nullptr; // next
    *reinterpret_cast<size_t*>(reinterpret_cast<char*>(first_block) + sizeof(void*)) = space_size - block_metadata_size;

    set_first_free_block(first_block);
}

[[nodiscard]] void *allocator_sorted_list::do_allocate_sm(
    size_t size)
{
    std::lock_guard<std::mutex> lock(get_mutex());

    if (size == 0) {
        return nullptr;
    }


    void* prev_free_block = nullptr;
    void* current_free_block = get_first_free_block();
    void* best_fit_block = nullptr;
    void* best_fit_prev = nullptr;
    size_t best_fit_size = SIZE_MAX;
    void* worst_fit_block = nullptr;
    void* worst_fit_prev = nullptr;
    size_t worst_fit_size = 0;

    while (current_free_block != nullptr) {
        size_t current_block_size = get_block_size(current_free_block);

        if (current_block_size >= size) {
            switch (get_fit_mode()) {
                case fit_mode::first_fit: {
                    std::lock_guard<std::mutex> unlock(get_mutex());
                    return split_block_if_needed(current_free_block, prev_free_block, size);
                }
                case fit_mode::the_best_fit: {
                    if (current_block_size < best_fit_size) {
                        best_fit_size = current_block_size;
                        best_fit_block = current_free_block;
                        best_fit_prev = prev_free_block;
                    }
                    break;
                }
                case fit_mode::the_worst_fit: {
                    if (current_block_size > worst_fit_size) {
                        worst_fit_size = current_block_size;
                        worst_fit_block = current_free_block;
                        worst_fit_prev = prev_free_block;
                    }
                    break;
                }
            }
        }

        prev_free_block = current_free_block;
        current_free_block = get_next_free_block(current_free_block);
    }

    switch (get_fit_mode()) {
        case fit_mode::the_best_fit: {
            if (best_fit_block != nullptr) {
                return split_block_if_needed(best_fit_block, best_fit_prev, size);
            }
            break;
        }
        case fit_mode::the_worst_fit: {
            if (worst_fit_block != nullptr) {
                return split_block_if_needed(worst_fit_block, worst_fit_prev, size);
            }
            break;
        }
        default:
            break;
    }
    std::lock_guard<std::mutex> unlock(get_mutex());
    throw std::bad_alloc();
}

size_t allocator_sorted_list::get_block_size(void* block) const {
    return *reinterpret_cast<size_t*>(reinterpret_cast<char*>(block) + sizeof(void*));
}

void* allocator_sorted_list::get_next_free_block(void* block) const {
    return *reinterpret_cast<void**>(block);
}

void* allocator_sorted_list::get_first_free_block() const {
    // Получаем указатель на начало trusted_memory
    char* memory_start = reinterpret_cast<char*>(_trusted_memory);

    size_t offset =
            sizeof(logger*) +
            sizeof(std::pmr::memory_resource*) +
            sizeof(fit_mode) +
            sizeof(size_t) +
            sizeof(std::mutex);

    void** first_free_ptr = reinterpret_cast<void**>(memory_start + offset);
    return *first_free_ptr;
}

void* allocator_sorted_list::split_block_if_needed(void* block, void* prev_block, size_t requested_size) {
    size_t total_block_size = get_block_size(block);
    size_t remaining_size = total_block_size - requested_size - block_metadata_size;

    if (remaining_size > 0) {
        void* new_free_block = reinterpret_cast<char*>(block) + block_metadata_size + requested_size;
        *reinterpret_cast<size_t*>(reinterpret_cast<char*>(new_free_block) + sizeof(void*)) = remaining_size;
        *reinterpret_cast<void**>(new_free_block) = get_next_free_block(block);


        if (prev_block != nullptr) {
            *reinterpret_cast<void**>(prev_block) = new_free_block;
        } else {
            set_first_free_block(new_free_block);
        }

        *reinterpret_cast<size_t*>(reinterpret_cast<char*>(block) + sizeof(void*)) = requested_size;
    }

    return reinterpret_cast<char*>(block) + block_metadata_size;
}

void allocator_sorted_list::set_first_free_block(void* new_first) {
    char *memory_start = reinterpret_cast<char *>(_trusted_memory);
    size_t offset =
            sizeof(logger *) +
            sizeof(std::pmr::memory_resource *) +
            sizeof(fit_mode) +
            sizeof(size_t) +
            sizeof(std::mutex);

    void **first_free_ptr = reinterpret_cast<void **>(memory_start + offset);
    *first_free_ptr = new_first;
}

allocator_with_fit_mode::fit_mode allocator_sorted_list::get_fit_mode() const {
    char* memory_start = reinterpret_cast<char*>(_trusted_memory);

    size_t offset = sizeof(logger*) + sizeof(std::pmr::memory_resource*);

    fit_mode* mode_ptr = reinterpret_cast<fit_mode*>(memory_start + offset);

    return *mode_ptr;
}

std::pair<void*, size_t> allocator_sorted_list::get_block_info(void* block_start){
    size_t size;
    size = reinterpret_cast<size_t>(block_start);
    block_start = (char*)block_start + sizeof(size_t);
    void** next = reinterpret_cast<void**>(block_start);
    return {*next, size};
}

allocator_sorted_list::allocator_sorted_list(const allocator_sorted_list &other)
{
    std::lock_guard<std::mutex> lock(other.get_mutex());
    size_t space_size = other.get_allocator_size();

    _trusted_memory = other.get_parent_allocator() ?
                      other.get_parent_allocator()->allocate(allocator_metadata_size + space_size) :
                      ::operator new(allocator_metadata_size + space_size);

    // Копирование метаданных
    memcpy(_trusted_memory, other._trusted_memory, allocator_metadata_size + space_size);
}

allocator_sorted_list &allocator_sorted_list::operator=(const allocator_sorted_list &other) {
    if (this != &other) {
        std::unique_lock<std::mutex> lock_this(get_mutex(), std::defer_lock);
        std::unique_lock<std::mutex> lock_other(other.get_mutex(), std::defer_lock);
        std::lock(lock_this, lock_other);

        size_t new_space_size = other.get_allocator_size();
        void* new_memory = other.get_parent_allocator() ?
                           other.get_parent_allocator()->allocate(allocator_metadata_size + new_space_size) :
                           ::operator new(allocator_metadata_size + new_space_size);

        // Освобождаем старую память
        auto parent_allocator = get_parent_allocator();
        size_t old_size = get_allocator_size();
        if (parent_allocator) {
            parent_allocator->deallocate(_trusted_memory, allocator_metadata_size + old_size);
        } else {
            ::operator delete(_trusted_memory);
        }

        _trusted_memory = new_memory;
        memcpy(_trusted_memory, other._trusted_memory, allocator_metadata_size + new_space_size);
    }
    return *this;
}

allocator_sorted_list &allocator_sorted_list::operator=(const allocator_sorted_list &other)
{
    throw not_implemented("allocator_sorted_list &allocator_sorted_list::operator=(const allocator_sorted_list &other)", "your code should be here...");
}

bool allocator_sorted_list::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    throw not_implemented("bool allocator_sorted_list::do_is_equal(const std::pmr::memory_resource &other) const noexcept", "your code should be here...");
}

void allocator_sorted_list::do_deallocate_sm(
    void *at)
{
    throw not_implemented("void allocator_sorted_list::do_deallocate_sm(void *)", "your code should be here...");
}

inline void allocator_sorted_list::set_fit_mode(
    allocator_with_fit_mode::fit_mode mode)
{
    throw not_implemented("inline void allocator_sorted_list::set_fit_mode(allocator_with_fit_mode::fit_mode)", "your code should be here...");
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept", "your code should be here...");
}

inline logger *allocator_sorted_list::get_logger() const
{
    throw not_implemented("inline logger *allocator_sorted_list::get_logger() const", "your code should be here...");
}

inline std::string allocator_sorted_list::get_typename() const
{
    throw not_implemented("inline std::string allocator_sorted_list::get_typename() const", "your code should be here...");
}


std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info_inner() const
{
    throw not_implemented("std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info_inner() const", "your code should be here...");
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_begin() const noexcept
{
    throw not_implemented("allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_begin() const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_end() const noexcept
{
    throw not_implemented("allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_end() const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::begin() const noexcept
{
    throw not_implemented("allocator_sorted_list::sorted_iterator allocator_sorted_list::begin() const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::end() const noexcept
{
    throw not_implemented("allocator_sorted_list::sorted_iterator allocator_sorted_list::end() const noexcept", "your code should be here...");
}

std::mutex &allocator_sorted_list::get_mutex() {
    char* memory_start = reinterpret_cast<char*>(_trusted_memory);
    size_t offset =
            sizeof(logger*) +
            sizeof(std::pmr::memory_resource*) +
            sizeof(fit_mode) +
            sizeof(size_t);

    return *reinterpret_cast<std::mutex*>(memory_start + offset);
}


bool allocator_sorted_list::sorted_free_iterator::operator==(
        const allocator_sorted_list::sorted_free_iterator & other) const noexcept
{
    throw not_implemented("bool allocator_sorted_list::sorted_free_iterator::operator==(const allocator_sorted_list::sorted_free_iterator &) const noexcept", "your code should be here...");
}

bool allocator_sorted_list::sorted_free_iterator::operator!=(
        const allocator_sorted_list::sorted_free_iterator &other) const noexcept
{
    throw not_implemented("bool allocator_sorted_list::sorted_free_iterator::operator!=(const allocator_sorted_list::sorted_free_iterator &) const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_free_iterator &allocator_sorted_list::sorted_free_iterator::operator++() & noexcept
{
    throw not_implemented("allocator_sorted_list::sorted_free_iterator &allocator_sorted_list::sorted_free_iterator::operator++() & noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::sorted_free_iterator::operator++(int n)
{
    throw not_implemented("allocator_sorted_list::sorted_free_iterator allocator_sorted_list::sorted_free_iterator::operator++(int)", "your code should be here...");
}

size_t allocator_sorted_list::sorted_free_iterator::size() const noexcept
{
    throw not_implemented("size_t allocator_sorted_list::sorted_free_iterator::size() const noexcept", "your code should be here...");
}

void *allocator_sorted_list::sorted_free_iterator::operator*() const noexcept
{
    throw not_implemented("void *allocator_sorted_list::sorted_free_iterator::operator*() const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator()
{
    throw not_implemented("allocator_sorted_list::sorted_free_iterator::sorted_free_iterator()", "your code should be here...");
}

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator(void *trusted)
{
    throw not_implemented("allocator_sorted_list::sorted_free_iterator::sorted_free_iterator(void *)", "your code should be here...");
}

bool allocator_sorted_list::sorted_iterator::operator==(const allocator_sorted_list::sorted_iterator & other) const noexcept
{
    throw not_implemented("bool allocator_sorted_list::sorted_iterator::operator==(const allocator_sorted_list::sorted_iterator &) const noexcept", "your code should be here...");
}

bool allocator_sorted_list::sorted_iterator::operator!=(const allocator_sorted_list::sorted_iterator &other) const noexcept
{
    throw not_implemented("bool allocator_sorted_list::sorted_iterator::operator!=(const allocator_sorted_list::sorted_iterator &) const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_iterator &allocator_sorted_list::sorted_iterator::operator++() & noexcept
{
    throw not_implemented("allocator_sorted_list::sorted_iterator &allocator_sorted_list::sorted_iterator::operator++() & noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::sorted_iterator::operator++(int n)
{
    throw not_implemented("allocator_sorted_list::sorted_iterator allocator_sorted_list::sorted_iterator::operator++(int)", "your code should be here...");
}

size_t allocator_sorted_list::sorted_iterator::size() const noexcept
{
    throw not_implemented("size_t allocator_sorted_list::sorted_iterator::size() const noexcept", "your code should be here...");
}

void *allocator_sorted_list::sorted_iterator::operator*() const noexcept
{
    throw not_implemented("void *allocator_sorted_list::sorted_iterator::operator*() const noexcept", "your code should be here...");
}

allocator_sorted_list::sorted_iterator::sorted_iterator()
{
    throw not_implemented("allocator_sorted_list::sorted_iterator::sorted_iterator() ", "your code should be here...");
}

allocator_sorted_list::sorted_iterator::sorted_iterator(void *trusted)
{
    throw not_implemented("allocator_sorted_list::sorted_iterator::sorted_iterator(void *)", "your code should be here...");
}

bool allocator_sorted_list::sorted_iterator::occupied() const noexcept
{
    throw not_implemented("bool allocator_sorted_list::sorted_iterator::occupied() const noexcept", "your code should be here...");
}
