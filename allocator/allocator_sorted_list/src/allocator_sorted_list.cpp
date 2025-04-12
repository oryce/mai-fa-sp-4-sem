#include "allocator_sorted_list.h"

allocator_sorted_list::allocator_sorted_list(
        size_t space_size,
        std::pmr::memory_resource *parent_allocator,
        logger *logger,
        allocator_with_fit_mode::fit_mode allocate_fit_mode) :
        _trusted_memory(parent_allocator ?
                        parent_allocator->allocate(allocator_metadata_size + space_size + block_metadata_size) :
                        ::operator new(allocator_metadata_size + space_size + block_metadata_size)) {
    auto *mem = static_cast<uint8_t *>(_trusted_memory);
    *reinterpret_cast<class logger **>(mem) = logger;
    mem += sizeof(class logger * );
    *reinterpret_cast<std::pmr::memory_resource **>(mem) = parent_allocator;
    mem += sizeof(std::pmr::memory_resource *);
    *reinterpret_cast<fit_mode *>(mem) = allocate_fit_mode;
    mem += sizeof(fit_mode);
    *reinterpret_cast<size_t *>(mem) = space_size;
    mem += sizeof(size_t);
    new(mem) std::mutex;
    mem += sizeof(std::mutex);

    void *first_block = mem + sizeof(void *);
    *reinterpret_cast<void **>(first_block) = mem + sizeof(void *);
}

allocator_sorted_list::~allocator_sorted_list() {
    auto *parent = *reinterpret_cast<std::pmr::memory_resource **>(static_cast<uint8_t *>(_trusted_memory) +
                                                                   sizeof(logger *));
    if (parent)
        parent->deallocate(_trusted_memory, allocator_metadata_size +
                                            *reinterpret_cast<size_t *>(static_cast<uint8_t *>(_trusted_memory) +
                                                                        sizeof(logger *) +
                                                                        sizeof(std::pmr::memory_resource *) +
                                                                        sizeof(fit_mode)));
    else
        ::operator delete(_trusted_memory);
}

std::mutex& allocator_sorted_list::get_mutex(){
    auto * mutex_ptr = reinterpret_cast<std::mutex*>(static_cast<uint8_t *>(_trusted_memory)
            + sizeof(logger *)
            + sizeof(std::pmr::memory_resource *)
            + sizeof(fit_mode)
            + sizeof(size_t));
    return *mutex_ptr;
}

allocator_sorted_list::fit_mode& allocator_sorted_list::get_fit_mode(){
    return *reinterpret_cast<fit_mode *>(static_cast<uint8_t *>(_trusted_memory) + sizeof(logger *) +
                                         sizeof(std::pmr::memory_resource *));
}

void*& allocator_sorted_list::get_ptr_from_block_metadata(void* block_header){
    auto ptr = reinterpret_cast<void**>(static_cast<uint8_t *>(block_header) + sizeof(size_t));
    return *ptr;
}

size_t& allocator_sorted_list::get_size_block(void* block_header){
    auto ptr = reinterpret_cast<size_t*>(static_cast<uint8_t *>(block_header));
    return *ptr;
}

void *allocator_sorted_list::do_allocate_sm(size_t size) {
    std::lock_guard<std::mutex> lock(get_mutex());

    void* prev_free_block = nullptr;
    void *result_block = nullptr;
    size_t best_size = 0;
    auto mode = get_fit_mode();

    for (auto it = free_begin(); it != free_end(); ++it) {
        auto block_size = it.size();
        prev_free_block = result_block;
        if (block_size >= size + block_metadata_size) {
            if (mode == fit_mode::first_fit) {
                result_block = *it;
                best_size = block_size;
                break;
            } else if (mode == fit_mode::the_best_fit && (result_block == nullptr || block_size < best_size)) {
                result_block = *it;
                best_size = block_size;
            } else if (mode == fit_mode::the_worst_fit && (result_block == nullptr || block_size > best_size)) {
                result_block = *it;
                best_size = block_size;
            }
        }
    }

    if (!result_block)
        throw std::bad_alloc();

    auto remaining = best_size - size - block_metadata_size;
    uint8_t *new_block;
    void*& result_block_next_ptr = get_ptr_from_block_metadata(result_block);
    if (remaining >= block_metadata_size + 1) {
        //есть ли вообще смысл создавать этот блок, или он получается слишком маленький
        new_block = static_cast<uint8_t *>(result_block) + block_metadata_size + size;
        size_t& new_block_size = get_size_block(new_block);
        new_block_size = remaining - block_metadata_size;
        void*& new_block_ptr = get_ptr_from_block_metadata(new_block);
        new_block_ptr = result_block_next_ptr;
    }

    if (prev_free_block != nullptr){
        void*& next_ptr = get_ptr_from_block_metadata(prev_free_block);
        if (new_block){
            next_ptr = new_block;
        } else {
            next_ptr = get_ptr_from_block_metadata(result_block);
        }
    }
    result_block_next_ptr = prev_free_block;
    std::lock_guard<std::mutex> unlock(get_mutex());
    return reinterpret_cast<void*>(static_cast<uint8_t *>(result_block) + block_metadata_size);
}

void allocator_sorted_list::do_deallocate_sm(void *at) {
    std::lock_guard<std::mutex> lock(get_mutex());

    uint8_t* block_header = reinterpret_cast<uint8_t*>(at) - block_metadata_size;
    uint8_t* prev_free_block = reinterpret_cast<uint8_t*>(get_ptr_from_block_metadata(block_header));
    uint8_t* next_free_block = reinterpret_cast<uint8_t*>(get_ptr_from_block_metadata(prev_free_block));
    if (try_merge(prev_free_block, block_header)){
        void*& ptr = get_ptr_from_block_metadata(prev_free_block);
        ptr = next_free_block;

        if (try_merge(prev_free_block, next_free_block)){
            ptr = get_ptr_from_block_metadata(next_free_block);
        }
    } else if (try_merge(block_header, next_free_block)){
        void*& ptr = get_ptr_from_block_metadata(block_header);
        ptr = get_ptr_from_block_metadata(next_free_block);

        if (try_merge(prev_free_block, next_free_block)){
            ptr = get_ptr_from_block_metadata(prev_free_block);
            ptr = get_ptr_from_block_metadata(next_free_block);
        }
    } else {
        void*& ptr = get_ptr_from_block_metadata(prev_free_block);
        ptr = block_header;
        void*& ptr_cur = get_ptr_from_block_metadata(block_header);
        ptr_cur = next_free_block;
    }
    std::lock_guard<std::mutex> unlock(get_mutex());
}



bool allocator_sorted_list::try_merge (uint8_t *left, uint8_t *right) {
    size_t& l_size = get_size_block(left);
    size_t& r_size = get_size_block(right);
    if (left + block_metadata_size + l_size == right){
        l_size = right + r_size - left; //полная формула l_size = (right + r_size + block_metadata_size) - (left + block_metadata_size)
        return true;
    }


    return false;
};

bool allocator_sorted_list::do_is_equal(const std::pmr::memory_resource &other) const noexcept {
    return this == &other;
}

void allocator_sorted_list::set_fit_mode(allocator_with_fit_mode::fit_mode mode) {
    *reinterpret_cast<fit_mode *>(static_cast<uint8_t *>(_trusted_memory) + sizeof(logger *) +
                                  sizeof(std::pmr::memory_resource *)) = mode;
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept {
    return get_blocks_info_inner();
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info_inner() const {
    std::vector<block_info> info;
    for (auto it = begin(); it != end(); ++it)
        info.emplace_back(it.size(), it.occupied());
    return info;
}

logger *allocator_sorted_list::get_logger() const {
    return *reinterpret_cast<logger **>(_trusted_memory);
}

std::string allocator_sorted_list::get_typename() const {
    return "allocator_sorted_list";
}

// Iterator implementations
bool allocator_sorted_list::sorted_free_iterator::operator==(const sorted_free_iterator &other) const noexcept {
    return _free_ptr == other._free_ptr;
}

bool allocator_sorted_list::sorted_free_iterator::operator!=(const sorted_free_iterator &other) const noexcept {
    return !(*this == other);
}

allocator_sorted_list::sorted_free_iterator &allocator_sorted_list::sorted_free_iterator::operator++() & noexcept {
    _free_ptr = *reinterpret_cast<void **>(_free_ptr);
    return *this;
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::sorted_free_iterator::operator++(int n) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_sorted_list::sorted_free_iterator::size() const noexcept {
    return *reinterpret_cast<size_t *>(static_cast<uint8_t *>(_free_ptr) + sizeof(void *));
}

void *allocator_sorted_list::sorted_free_iterator::operator*() const noexcept { return _free_ptr; }

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator() : _free_ptr(nullptr) {}

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator(void *trusted) {
    uint8_t* ptr = *reinterpret_cast<uint8_t**>
            (reinterpret_cast<uint8_t*>(trusted) + sizeof(class logger *)
                                                     + sizeof(std::pmr::memory_resource *)
                                                     + sizeof(fit_mode)
                                                     + sizeof(size_t)
                                                     + sizeof(std::mutex));
    _free_ptr = ptr;
}

bool allocator_sorted_list::sorted_iterator::operator==(const sorted_iterator &other) const noexcept {
    return _current_ptr == other._current_ptr;
}

bool allocator_sorted_list::sorted_iterator::operator!=(const sorted_iterator &other) const noexcept {
    return !(*this == other);
}

allocator_sorted_list::sorted_iterator &allocator_sorted_list::sorted_iterator::operator++() & noexcept {
    if (_current_ptr == _free_ptr){
        _free_ptr = get_ptr_from_block_metadata(_free_ptr);
    }
    _current_ptr = static_cast<uint8_t *>(_current_ptr) + block_metadata_size + size();
    return *this;
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::sorted_iterator::operator++(int n) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_sorted_list::sorted_iterator::size() const noexcept {
    return *reinterpret_cast<size_t *>(static_cast<uint8_t *>(_current_ptr) + sizeof(void *));
}

void *allocator_sorted_list::sorted_iterator::operator*() const noexcept { return _current_ptr; }

bool allocator_sorted_list::sorted_iterator::occupied() const noexcept { return _current_ptr < _free_ptr; }

allocator_sorted_list::sorted_iterator::sorted_iterator() {
    _trusted_memory = nullptr;
    _free_ptr = nullptr;
    _current_ptr = nullptr;
}

allocator_sorted_list::sorted_iterator::sorted_iterator(void *trusted) :
        _trusted_memory(trusted),
        _free_ptr(*reinterpret_cast<void **>(static_cast<uint8_t *>(trusted) + sizeof(logger *) +
                                             sizeof(std::pmr::memory_resource *) + sizeof(fit_mode) + sizeof(size_t) +
                                             sizeof(std::mutex))),
        _current_ptr(static_cast<uint8_t *>(trusted) + allocator_metadata_size) {}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_begin() const noexcept {
    return {_trusted_memory};
}

allocator_sorted_list::sorted_free_iterator
allocator_sorted_list::free_end() const noexcept {
    return {};
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::begin() const noexcept {
    return {_trusted_memory};
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::end() const noexcept {
    return sorted_iterator();
}