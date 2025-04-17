#include <not_implemented.h>
#include "../include/allocator_global_heap.h"
#include <format>

allocator_global_heap::allocator_global_heap(
    logger *logger) : _logger(logger)
{
}

[[nodiscard]] void *allocator_global_heap::do_allocate_sm(
    size_t size)
{
    debug_with_guard(std::format("[*] do_allocate_sm({})", size));

    void* mem;

    try
    {
        mem = ::operator new(size);
    } catch (const std::bad_alloc &e)
    {
        error_with_guard(std::format("[!] allocation failed: {}", e.what()));
        throw;
    }

    debug_with_guard(std::format("[+] allocated {} bytes at {:p}", size, mem));

    return mem;
}

void allocator_global_heap::do_deallocate_sm(
    void *at)
{
    if (at)
    {
        debug_with_guard(std::format("[*] freeing at {:p}", at));
        ::operator delete(at);
    }
}

inline logger *allocator_global_heap::get_logger() const
{
    return _logger;
}

inline std::string allocator_global_heap::get_typename() const
{
    return "allocator_global_heap";
}

allocator_global_heap::~allocator_global_heap()
    = default;

allocator_global_heap::allocator_global_heap(const allocator_global_heap &other)
    : _logger(other._logger)
{
    trace_with_guard(
        "[>] allocator_global_heap::allocator_global_heap(const allocator_global_heap &other)");
    trace_with_guard(
        "[<] allocator_global_heap::allocator_global_heap(const allocator_global_heap &other)");
}

allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)
{
    if (this == &other)
    {
        return *this;
    }
    trace_with_guard(
        "[>] allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)");
    _logger = other._logger;
    trace_with_guard(
        "[<] allocator_global_heap &allocator_global_heap::operator=(const allocator_global_heap &other)");
    return *this;
}

bool allocator_global_heap::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}

allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept
{
    trace_with_guard(
        "[>] allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept");
    if (this != &other)
    {
        _logger = other._logger;
    }
    trace_with_guard(
        "[<] allocator_global_heap::allocator_global_heap(allocator_global_heap &&other) noexcept");
}

allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept
{
    trace_with_guard(
        "[>] allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept");
    if (this != &other)
    {
        _logger = other._logger;
    }
    trace_with_guard(
        "[<] allocator_global_heap &allocator_global_heap::operator=(allocator_global_heap &&other) noexcept");
    return *this;
}
