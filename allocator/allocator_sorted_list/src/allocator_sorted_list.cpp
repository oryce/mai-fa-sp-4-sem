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
    mem += sizeof(class logger *);
    *reinterpret_cast<std::pmr::memory_resource **>(mem) = parent_allocator;
    mem += sizeof(std::pmr::memory_resource *);
    *reinterpret_cast<fit_mode *>(mem) = allocate_fit_mode;
    mem += sizeof(fit_mode);
    *reinterpret_cast<size_t *>(mem) = space_size;
    mem += sizeof(size_t);
    new(mem) std::mutex;
    mem += sizeof(std::mutex);
    void *first_block = mem;
    *reinterpret_cast<void **>(first_block) = mem + sizeof(void *);
    mem += sizeof(void *);
    set_size_in_block_metadata(mem, space_size);
    set_next_ptr(mem, nullptr);
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

std::mutex &allocator_sorted_list::get_mutex() {
    auto *mutex_ptr = reinterpret_cast<std::mutex *>(static_cast<uint8_t *>(_trusted_memory)
                                                     + sizeof(class logger *)
                                                     + sizeof(std::pmr::memory_resource *)
                                                     + sizeof(fit_mode)
                                                     + sizeof(size_t));
    return *mutex_ptr;
}

size_t allocator_sorted_list::get_space_size(void *trusted_memory) {
    auto *mutex_ptr = reinterpret_cast<size_t *>(static_cast<uint8_t *>(trusted_memory)
                                                 + sizeof(class logger *)
                                                 + sizeof(std::pmr::memory_resource *)
                                                 + sizeof(fit_mode));
    return *mutex_ptr;
}

void *allocator_sorted_list::get_first_free_block_pointer() const {
    auto *ptr = reinterpret_cast<void **>(static_cast<uint8_t *>(_trusted_memory)
                                          + sizeof(class logger *)
                                          + sizeof(std::pmr::memory_resource *)
                                          + sizeof(fit_mode)
                                          + sizeof(size_t)
                                          + sizeof(std::mutex));
    return *ptr;
}

void allocator_sorted_list::set_first_free_block_pointer(void *new_pointer) {
    auto *ptr = reinterpret_cast<void **>(static_cast<uint8_t *>(_trusted_memory)
                                          + sizeof(class logger *)
                                          + sizeof(std::pmr::memory_resource *)
                                          + sizeof(fit_mode)
                                          + sizeof(size_t)
                                          + sizeof(std::mutex));
    *ptr = new_pointer;
}

allocator_sorted_list::fit_mode &allocator_sorted_list::get_fit_mode() {
    return *reinterpret_cast<fit_mode *>(static_cast<uint8_t *>(_trusted_memory) + sizeof(logger *) +
                                         sizeof(std::pmr::memory_resource *));
}
void *allocator_sorted_list::get_next_ptr(void *block_header) {

    if (block_header == nullptr) {
        return nullptr;
    }
    auto ptr = reinterpret_cast<void **>(static_cast<uint8_t *>(block_header) + sizeof(size_t));
    return *ptr;
}

void allocator_sorted_list::set_next_ptr(void *block_header, void *new_ptr) {
    auto l = get_logger();
    if (l != nullptr){
        l->trace("allocator_sorted_list::set_next_ptr started\n");
    }
    auto ptr = reinterpret_cast<void **>(static_cast<uint8_t *>(block_header) + sizeof(size_t));
    *ptr = new_ptr;
    if (l != nullptr){
        l->trace("allocator_sorted_list::set_next_ptr finished\n");
    }
}

void allocator_sorted_list::set_size_in_block_metadata(void *block_header, size_t new_size) {
    auto l = get_logger();
    if (l != nullptr){
        l->trace("allocator_sorted_list::set_size_in_block_metadata started\n");
    }
    auto ptr = reinterpret_cast<size_t *>(static_cast<uint8_t *>(block_header));
    *ptr = new_size;
    if (l != nullptr){
        l->trace("allocator_sorted_list::set_size_in_block_metadata finished\n");
    }
}

size_t allocator_sorted_list::get_size(void *block_header) {
    auto ptr = reinterpret_cast<size_t *>(static_cast<uint8_t *>(block_header));
    return *ptr;
}

size_t allocator_sorted_list::get_free_memory_count(){
    size_t res = 0;
    for (auto it = free_begin(); it != free_end(); ++it) {
        res += it.size();
    }
    return res;
}

void *allocator_sorted_list::do_allocate_sm(size_t size) {
    std::lock_guard<std::mutex> lock(get_mutex());
    logger* l = get_logger();
    if (l != nullptr){
        l->debug("do_allocate_sm started\n");
    }

    void *prev_free_block = nullptr;
    void *result_block = nullptr;
    size_t best_size = 0;
    auto mode = get_fit_mode();
    sorted_free_iterator prev_iter;
    for (auto it = free_begin(); it != free_end(); prev_iter = it, ++it) {
//        std::cout << *it << std::endl;
        auto block_size = it.size();
        if (block_size >= size + block_metadata_size) {
            if (mode == fit_mode::first_fit) {
                prev_free_block = *prev_iter;
                result_block = *it;
                best_size = block_size;
                break;
            } else if (mode == fit_mode::the_best_fit && (result_block == nullptr || block_size < best_size)) {
                prev_free_block = *prev_iter;
                result_block = *it;
                best_size = block_size;
            } else if (mode == fit_mode::the_worst_fit && (result_block == nullptr || block_size > best_size)) {
                prev_free_block = *prev_iter;
                result_block = *it;
                best_size = block_size;
            }
        }
    }

    if (!result_block) {
        logger* l = get_logger();
        if (l != nullptr) {
            l->information("Unable to allocate memory\n");
        }
        throw std::bad_alloc();
    }
    auto remaining = best_size - size;
    uint8_t *new_block = nullptr;
    void *result_block_next_ptr = get_next_ptr(result_block);
    if (remaining >= block_metadata_size + 1) {
        //есть ли вообще смысл создавать этот блок, или он получается слишком маленький
        new_block = static_cast<uint8_t *>(result_block) + block_metadata_size + size;
        set_size_in_block_metadata(new_block, remaining - block_metadata_size);
        set_next_ptr(new_block, result_block_next_ptr);
        remaining = 0;
        set_size_in_block_metadata(result_block, size);
    }

    if (prev_free_block != nullptr) {
        if (new_block) {
            set_next_ptr(prev_free_block, new_block);
        } else {
            set_next_ptr(prev_free_block, get_next_ptr(result_block));
        }
    }
    void *ptr = get_first_free_block_pointer();
    if (ptr == result_block) {
        ptr = (new_block != nullptr) ? new_block : get_next_ptr(result_block);
        set_first_free_block_pointer(ptr);
    }
    set_next_ptr(result_block, nullptr);

    if (l != nullptr){
        l->information("Memory left: ");
        size_t count = get_free_memory_count();
        l->information(std::to_string(count) + "\n");
        auto blocks = get_blocks_info();
        for(auto item: blocks){
            l->debug(std::to_string(item.block_size) + " " + std::to_string(item.is_block_occupied) + "\n");
        }
        l->debug("do_allocate_sm finished\n");
    }
    set_next_ptr(result_block, _trusted_memory);
    return reinterpret_cast<void *>(static_cast<uint8_t *>(result_block) + block_metadata_size);
}

void allocator_sorted_list::do_deallocate_sm(void *at) {
    logger* l = get_logger();
    if (l != nullptr){
        l->debug("do_deallocate_sm started\n");
    }
    std::lock_guard<std::mutex> lock(get_mutex());

    uint8_t *block_header = static_cast<uint8_t *>(at) - block_metadata_size;
    uint8_t *prev_free_block = nullptr;
    uint8_t *next_free_block = nullptr;
    
    //Block from another allocator
    if (get_next_ptr(block_header) != _trusted_memory){
        if (get_logger() != nullptr){
            get_logger()->error("Incorrect memory deallocation\n");
            throw std::bad_alloc();
        }
    }

    auto it = begin();
    while (it != end()) {
        if (*it == block_header) {
            break;
        }
        if (!it.occupied()) {
            prev_free_block = static_cast<uint8_t *>(*it);
        }
        it++;
    }

    if (prev_free_block != nullptr) {
        next_free_block = static_cast<uint8_t *>(get_next_ptr(prev_free_block));
    } else {
        next_free_block = static_cast<uint8_t *>(get_first_free_block_pointer());
    }

    if (prev_free_block != nullptr) {
        set_next_ptr(prev_free_block, block_header);
    }
    set_next_ptr(block_header, next_free_block);

    if (try_merge(prev_free_block, static_cast<uint8_t *>(block_header))) {
        try_merge(prev_free_block, next_free_block);
    } else {
        try_merge(block_header, next_free_block);
    }

    void *first_block_ptr = get_first_free_block_pointer();
    if (prev_free_block == nullptr && static_cast<uint8_t *>(first_block_ptr) > block_header ||
        first_block_ptr == nullptr) {
        set_first_free_block_pointer(block_header);
    }

    if (l != nullptr){
        l->information("Memory left: ");
        size_t count = get_free_memory_count();
        l->information(std::to_string(count));
        auto blocks = get_blocks_info();
        for(auto item: blocks){
            l->debug(std::to_string(item.block_size) + " " + std::to_string(item.is_block_occupied) + "\n");
        }
        l->debug("do_deallocate_sm started finished\n");
    }
}

bool allocator_sorted_list::try_merge(uint8_t *left, uint8_t *right){
    auto l = get_logger();
    if (l != nullptr){
        l->trace("allocator_sorted_list::try_merge started\n");
    }
    if (left == nullptr || right == nullptr) {
        return false;
    }
    size_t l_size = get_size(left);
    size_t r_size = get_size(right);
    if (left + block_metadata_size + l_size == right) {
        size_t new_size = l_size + r_size + block_metadata_size;
        set_size_in_block_metadata(left, new_size);
        set_next_ptr(left, get_next_ptr(right));
        return true;
    }
    if (l != nullptr){
        l->trace("allocator_sorted_list::try_merge finished\n");
    }
    return false;
}

bool allocator_sorted_list::do_is_equal(const std::pmr::memory_resource &other) const noexcept {
    logger* l = get_logger();
    if (l != nullptr){
        l->debug("allocator_sorted_list::do_is_equal started\n");
    }
    auto res = *this == other;
    if (l != nullptr){
        l->debug("allocator_sorted_list::do_is_equal finished\n");
    }
    return res;
}

void allocator_sorted_list::set_fit_mode(allocator_with_fit_mode::fit_mode mode) {
    logger* l = get_logger();
    if (l != nullptr){
        l->debug("allocator_sorted_list::set_fit_mode started\n");
    }
    std::lock_guard<std::mutex> lock(get_mutex());
    *reinterpret_cast<fit_mode *>(static_cast<uint8_t *>(_trusted_memory) + sizeof(logger *) +
                                  sizeof(std::pmr::memory_resource *)) = mode;
    if (l != nullptr){
        l->debug("allocator_sorted_list::set_fit_mode finished\n");
    }
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info() const noexcept {
    return get_blocks_info_inner();
}

std::vector<allocator_test_utils::block_info> allocator_sorted_list::get_blocks_info_inner() const {
    logger* l = get_logger();
    if (l != nullptr){
        l->debug("allocator_sorted_list::get_blocks_info_inner started\n");
    }
    std::vector<block_info> info;
    for (auto it = begin(); it != end(); ++it)
        info.emplace_back(it.size(), it.occupied());

    if (l != nullptr){
        l->debug("allocator_sorted_list::get_blocks_info_inner finished\n");
    }
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
    _free_ptr = get_next_ptr(_free_ptr);
    return *this;
}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::sorted_free_iterator::operator++(int n) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_sorted_list::sorted_free_iterator::size() const noexcept {
    return get_size(_free_ptr);
}

void *allocator_sorted_list::sorted_free_iterator::operator*() const noexcept { return _free_ptr; }

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator() : _free_ptr(nullptr) {}

allocator_sorted_list::sorted_free_iterator::sorted_free_iterator(void *trusted) {
    _free_ptr = *reinterpret_cast<void **>
    (static_cast<uint8_t *>(trusted)
     + sizeof(class logger *)
     + sizeof(std::pmr::memory_resource *)
     + sizeof(fit_mode)
     + sizeof(size_t)
     + sizeof(std::mutex));
}

bool allocator_sorted_list::sorted_iterator::operator==(const sorted_iterator &other) const noexcept {
    return _current_ptr == other._current_ptr;
}

bool allocator_sorted_list::sorted_iterator::operator!=(const sorted_iterator &other) const noexcept {
    return !(*this == other);
}

allocator_sorted_list::sorted_iterator &allocator_sorted_list::sorted_iterator::operator++() & noexcept {
    if (_current_ptr == _free_ptr) {
        _free_ptr = get_next_ptr(_free_ptr);
    }
    uint8_t *new_ptr = static_cast<uint8_t *>(_current_ptr) + block_metadata_size + size();

    if (static_cast<uint8_t *>(_trusted_memory) + get_space_size(_trusted_memory) + allocator_metadata_size <=
        new_ptr) {
        _current_ptr = nullptr;
    } else {
        _current_ptr = new_ptr;
    }

    return *this;
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::sorted_iterator::operator++(int n) {
    auto tmp = *this;
    ++(*this);
    return tmp;
}

size_t allocator_sorted_list::sorted_iterator::size() const noexcept {
    return get_size(_current_ptr);
}

void *allocator_sorted_list::sorted_iterator::operator*() const noexcept { return _current_ptr; }

bool allocator_sorted_list::sorted_iterator::occupied() const noexcept { return _current_ptr != _free_ptr; }

allocator_sorted_list::sorted_iterator::sorted_iterator() {
    _trusted_memory = nullptr;
    _free_ptr = nullptr;
    _current_ptr = nullptr;
}

allocator_sorted_list::sorted_iterator::sorted_iterator(void *trusted) :
        _trusted_memory(trusted),
        _free_ptr(*reinterpret_cast<void **>(static_cast<uint8_t *>(trusted)
                                             + sizeof(class logger *)
                                             + sizeof(std::pmr::memory_resource *)
                                             + sizeof(fit_mode)
                                             + sizeof(size_t)
                                             + sizeof(std::mutex))),
        _current_ptr(static_cast<uint8_t *>(trusted) + allocator_metadata_size) {}

allocator_sorted_list::sorted_free_iterator allocator_sorted_list::free_begin() const noexcept {
    sorted_free_iterator it(_trusted_memory);
    return it;
}

allocator_sorted_list::sorted_free_iterator
allocator_sorted_list::free_end() const noexcept {
    return {};
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::begin() const noexcept {
    return {_trusted_memory};
}

allocator_sorted_list::sorted_iterator allocator_sorted_list::end() const noexcept {
    return {};
}

//rule 5
//move сематника по факту бесполезна, так как у нас одно единственное поле типа void*
allocator_sorted_list &allocator_sorted_list::operator=(allocator_sorted_list &&other) noexcept {
    _trusted_memory = other._trusted_memory;
    return *this;
}

allocator_sorted_list::allocator_sorted_list(allocator_sorted_list &&other) noexcept: _trusted_memory(
        other._trusted_memory) {

}