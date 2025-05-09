//
// Created by Des Caldnd on 2/28/2025.
//

#ifndef B_TREE_DISK_HPP
#define B_TREE_DISK_HPP

#include <iterator>
#include <utility>
#include <vector>
#include <concepts>
#include <stack>
#include <fstream>
#include <optional>
#include <cstddef>
#include <filesystem>
#include <logger_guardant.h>

template<typename compare, typename tkey>
concept compator = requires(const compare c, const tkey& lhs, const tkey& rhs)
{
    {c(lhs, rhs)} -> std::same_as<bool>;
} && std::copyable<compare> && std::default_initializable<compare>;

template<typename f_iter, typename tkey, typename tval>
concept input_iterator_for_pair = std::input_iterator<f_iter> && std::same_as<typename std::iterator_traits<f_iter>::value_type, std::pair<tkey, tval>>;

template<typename T>
concept serializable = requires (const T t, std::fstream& s)
{
    {t.serialize(s)};
    {T::deserialize(s)} -> std::same_as<T>;
    {t.serialize_size()} -> std::same_as<size_t>;
} && std::copyable<T>;


template <serializable tkey, serializable tvalue, compator<tkey> compare = std::less<tkey>, std::size_t t = 2>
class B_tree_disk final : private compare
{
public:

    using tree_data_type = std::pair<tkey, tvalue>;
    using tree_data_type_const = std::pair<tkey, tvalue>;

private:

    static constexpr const size_t minimum_keys_in_node = t - 1;
    static constexpr const size_t maximum_keys_in_node = 2 * t - 1;
    static constexpr size_t maximum_pointers_in_node = maximum_keys_in_node + 1;

    // region comparators declaration

    inline bool compare_keys(const tkey& lhs, const tkey& rhs) const;

    inline bool compare_pairs(const tree_data_type& lhs, const tree_data_type& rhs) const;

    // endregion comparators declaration

public:

    struct btree_disk_node
    {
        static size_t _last_position_in_key_value_file;
        size_t size; // кол-во заполненных ячеек
        bool _is_leaf;
        size_t position_in_disk;
        std::vector<tree_data_type> keys;
        std::vector<size_t> pointers;
        mutable std::vector<size_t> positions_in_key_value_file;
        void serialize(std::fstream& stream, std::fstream& stream_for_data) const;

        static btree_disk_node deserialize(std::fstream& stream, std::fstream& stream_for_data);

        explicit btree_disk_node(bool is_leaf);
        btree_disk_node();
        btree_disk_node(size_t);
        static const size_t node_metadata_size = 2 * sizeof(size_t)
                                                        + sizeof(bool)
                                                        + sizeof(size_t) * maximum_pointers_in_node
                                                        + sizeof(size_t) * maximum_keys_in_node; //for positions_in_key_value_file
    };


private:

    friend btree_disk_node;

    logger* _logger;

    std::fstream _file_for_tree;

    std::fstream _file_for_key_value;

//    btree_disk_node _root;

public:

    size_t _position_root;//

private:


    btree_disk_node _current_node;

    //logger* get_logger() const noexcept override;



public:

    static size_t _count_of_node;//только растет

    // region constructors declaration

    explicit B_tree_disk(const std::string& file_path, const compare& cmp = compare(), logger* logger = nullptr);


    // endregion constructors declaration

    // region five declaration

    B_tree_disk(B_tree_disk&& other) noexcept =default;
    B_tree_disk& operator=(B_tree_disk&& other) noexcept =default;

    B_tree_disk(const B_tree_disk& other) =delete;
    B_tree_disk& operator=(const B_tree_disk& other) =delete;

    ~B_tree_disk() noexcept = default;

    // endregion five declaration

    // region iterators declaration

    class btree_disk_const_iterator
    {
        std::stack<std::pair<size_t , size_t>> _path;
        size_t _index;
        B_tree_disk<tkey,tvalue, compare, t>& _tree;
    public:
        using value_type = tree_data_type_const;
        using reference = value_type&;
        using pointer = value_type*;
        using iterator_category = std::forward_iterator_tag;
        using difference_type = ptrdiff_t;

        using self = btree_disk_const_iterator;

        friend class B_tree_disk;

        value_type operator*() noexcept;

        self& operator++();
        self operator++(int);

        self& operator--();
        self operator--(int);

        bool operator==(self& other) noexcept;
        bool operator!=(self& other) noexcept;

        explicit btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>>& path = std::stack<std::pair<size_t, size_t>>(), size_t index = 0);

    };

    friend class btree_disk_const_iterator;

    std::optional<tvalue> at(const tkey&);//либо пустое, либо tvalue//std::nullopt

    btree_disk_const_iterator begin();
    btree_disk_const_iterator end() ;

    //std::vector<tree_data_type_const> find_range(const tkey& lower, const tkey& upper) const;

    std::pair<btree_disk_const_iterator, btree_disk_const_iterator> find_range(const tkey& lower, const tkey& upper, bool include_lower = true, bool include_upper = false);

    /*
     * Does nothing if key exists
     * Second return value is true, when inserted
     */
    bool insert(const tree_data_type& data);

    /*
     * Updates value if key exists
     */
    bool update(const tree_data_type& data);

    /*
     * Return true if deleted
     */
    bool erase(const tkey& key);

    bool check_if_leaf(btree_disk_node, size_t);

    void delete_key_from_leaf(size_t, btree_disk_node, std::stack<std::pair<size_t, size_t>>);

    void merge(btree_disk_node &, btree_disk_node &, btree_disk_node &, size_t);

    bool is_valid() const noexcept;


    std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t, bool>>  find_path(const tkey& key);

public:

    btree_disk_node disk_read(size_t position);


    void check_tree(size_t pos, size_t depth);

    void disk_write(btree_disk_node& node);
private:

    std::pair<size_t, bool> find_index(const tkey &key, btree_disk_node& node) const noexcept;

    void insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept;

    void split_node(std::stack<std::pair<size_t, size_t>> path);

    btree_disk_node remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr = true) noexcept;

    void print_root_position() noexcept;

};

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node(size_t pos)
                                    :  _is_leaf(true), size(0), position_in_disk(_count_of_node)
{
    position_in_disk = pos;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::_last_position_in_key_value_file = 0;

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::is_valid() const noexcept
{

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::erase(const tkey& key)
{
    size_t ind = 0;
    size_t prev_index = 0;
    auto cur_node = disk_read(_position_root);
    std::stack<std::pair<size_t, size_t>> st = std::stack<std::pair<size_t, size_t>>();
    btree_disk_node parent;
    parent.position_in_disk = SIZE_MAX;
    //находим нужный ключ
    while(cur_node.position_in_disk != SIZE_MAX){
        ind = 0;
        for(; ind < cur_node.keys.size() && compare_keys(cur_node.keys[ind].first, key); ind++){}
        st.push(std::pair<size_t, size_t>(cur_node.position_in_disk, prev_index));
        if (ind < cur_node.keys.size() && cur_node.keys[ind].first == key){
            break;
        }

        parent = cur_node;
        if (ind >= cur_node.pointers.size()){
            return false;
        }
        cur_node = disk_read(cur_node.pointers[ind]);
        prev_index = ind;
    }

    if (cur_node.position_in_disk == SIZE_MAX){
        return false;
    }

    //проверяем, является ли листом
    bool is_leaf = check_if_leaf(cur_node, ind);

    //в зависимости от того лист или нет обрабатываем удаление
    tree_data_type* new_data;
    if (is_leaf){
        delete_key_from_leaf(ind, cur_node, st);
    } else {
        size_t index;
        btree_disk_node n;
        // Заменяем самым правым элементом из левого поддерева
        // ВРОДЕ БЫ если элемент не лист, то левое поддерево у него должно быть всегда
        n = disk_read(cur_node.pointers[index]);
        st.push(std::pair(n.position_in_disk, index));
        while(n.pointers.size() > 0 && n.pointers[n.pointers.size() - 1] != SIZE_MAX){
            size_t tmp = n.pointers.size() - 1;
            n = disk_read(n.pointers[n.pointers.size() - 1]);
            st.push(std::pair(n.position_in_disk, tmp));
        }
        new_data = &n.keys[n.keys.size() - 1];
        index = n.keys.size() - 1;

        cur_node.keys[index] = std::move(*new_data);
        disk_write(cur_node);
        delete_key_from_leaf(index, n, st);
    }
    return true;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::check_if_leaf(btree_disk_node cur_node, size_t key_index) {
    bool is_leaf = true;
    if (cur_node.pointers.size() > key_index){
        is_leaf = disk_read(cur_node.pointers[key_index]).position_in_disk == SIZE_MAX;
        if (cur_node.pointers.size() > key_index + 1){
            is_leaf = disk_read(cur_node.pointers[key_index + 1]).position_in_disk == SIZE_MAX;
        }
    }
    return is_leaf;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::delete_key_from_leaf(size_t index_of_key, B_tree_disk::btree_disk_node node,
                                                                 std::stack<std::pair<size_t, size_t>> st) {
    if (node.keys.size() > minimum_keys_in_node){
        node.keys.erase(node.keys.begin() + index_of_key);
        node.size--;
        disk_write(node);
        return;
    }
    btree_disk_node cur_node = disk_read(st.top().first);
    size_t i = st.top().second;
    st.pop();

    btree_disk_node parent = st.empty() ? btree_disk_node(SIZE_MAX) : disk_read(st.top().first);
    if (parent.position_in_disk == SIZE_MAX) {
        if (cur_node.keys.size() > 1) {
            // в корне может быть < mininmum_keys_in_node ключей, спокойно удаляем
            cur_node.keys.erase(cur_node.keys.begin() + index_of_key);
            cur_node.size--;
        } else {
            // в корне один элемент, удаляем его и делаем корнем одного из его потомков
            if (cur_node.pointers.size() > 0) {
                if (disk_read(cur_node.pointers[0]).position_in_disk != SIZE_MAX) {
                    _position_root = cur_node.pointers[0];
                } else {
                    if (cur_node.pointers.size() > 1) {
                        _position_root = cur_node.pointers[1];
                    } else {
                        _position_root = SIZE_MAX;
                    }
                }
            }
        }
        disk_write(cur_node);
        return;
    }


    size_t size_left_sibling = 0;
    size_t size_right_sibling = 0;

    auto left_sibling = disk_read(parent.pointers[i - 1]);
    auto right_sibling = disk_read(parent.pointers[i + 1]);
    if (i > 0 && left_sibling.position_in_disk != SIZE_MAX) {
        size_left_sibling = left_sibling.keys.size();
    }

    if (i < parent.pointers.size() && right_sibling.position_in_disk != SIZE_MAX) {
        size_right_sibling = right_sibling.keys.size();
    }

    if (size_left_sibling > minimum_keys_in_node) {
        node.keys[index_of_key] = parent.keys[i - 1];
        parent.keys[i - 1] = left_sibling.keys[size_left_sibling - 1];
        disk_write(node);
        disk_write(parent);
    } else if (size_right_sibling > minimum_keys_in_node) {
        node.keys[index_of_key] = parent.keys[i];
        parent.keys[i] = right_sibling.keys[0];
        disk_write(node);
        disk_write(parent);
    } else {
        //если у обоих братьев мало ключей, то просто сливаемся с одним из них
        node.keys.erase(node.keys.begin() + index_of_key);
        node.size--;
        if (size_left_sibling > 0) {
            merge(left_sibling, cur_node, parent, i - 1);
        } else if (size_right_sibling > 0) {
            merge(cur_node, right_sibling, parent, i);
        }
        disk_write(node);
    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::merge(B_tree_disk::btree_disk_node & left, B_tree_disk::btree_disk_node & right,
                                                  B_tree_disk::btree_disk_node & parent, size_t split_key_index) {
    btree_disk_node new_node(_count_of_node++);
    new_node.keys = left.keys;
    new_node.pointers = left.pointers;
    new_node.keys.push_back(parent.keys[split_key_index]);
    new_node.pointers.push_back(parent.pointers[split_key_index]);
    new_node.keys.insert(new_node.keys.end(), right.keys.begin(), right.keys.end() - 1);
    new_node.pointers.insert(new_node.pointers.end(), right.pointers.begin(), right.pointers.end() - 1);
    new_node.size = left.size + right.size + 1;
    parent.keys[split_key_index] = new_node.keys[new_node.keys.size() - 1];
    parent.pointers[split_key_index] = new_node.position_in_disk;
    disk_write(new_node);
    disk_write(parent);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::print_root_position() noexcept
{
    if(!_file_for_tree.is_open())
    {
        return;
    }
    _file_for_tree.seekg(sizeof(size_t), std::ios::beg);
    _file_for_tree.write(reinterpret_cast<const char*>(&_position_root), sizeof(size_t));
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::remove_array(btree_disk_node& node, size_t index, bool remove_left_ptr) noexcept
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::update(const B_tree_disk::tree_data_type &data)
{

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::insert(const B_tree_disk::tree_data_type &data)
{
    size_t prev_i;
    btree_disk_node prev;
    btree_disk_node prev_parent;
    auto cur_node = disk_read(_position_root);
    std::stack<std::pair<size_t, size_t>> path = std::stack<std::pair<size_t, size_t>>();

    btree_disk_node root = disk_read(_position_root);
    if (root.position_in_disk == SIZE_MAX){
        root = btree_disk_node();
        root.position_in_disk = _count_of_node++;
        _position_root = root.position_in_disk;
        root.keys.insert(root.keys.begin(), data);
        root.pointers.insert(root.pointers.begin(), SIZE_MAX);
        prev_i = 0;
        root.size = 1;
        disk_write(root);
    } else {
        auto cur_key = root.keys[0].first;
        size_t i = 0;
        prev_i = 0;
        while(cur_node.position_in_disk != SIZE_MAX){
            i = 0;
            prev_i = 0;
            cur_key = cur_node.keys[prev_i].first;
            for(; i < cur_node.keys.size() && compare_keys(cur_node.keys[i].first, data.first); prev_i = i++){

            }

            cur_key = cur_node.keys[prev_i].first;

            if (cur_key == data.first){
                return false;
            }
            prev_parent = prev;
            prev = cur_node;


            if (i < cur_node.pointers.size()){
                cur_node = disk_read(cur_node.pointers[i]);
            } else {
                cur_node.position_in_disk = SIZE_MAX;
            }
            path.push(std::move(std::pair<size_t, size_t>(prev.position_in_disk, prev_i)));
        }
        if (prev.keys.size() == maximum_keys_in_node){
            prev.keys.insert(prev.keys.begin() + i, data);
            prev.pointers.insert(prev.pointers.begin() + i, SIZE_MAX);
            prev.size++;
            disk_write(prev);
            split_node(path);
        } else {
            prev.keys.insert(prev.keys.begin() + i, data);
            prev.pointers.insert(prev.pointers.begin() + i, SIZE_MAX);
            prev.size++;
            disk_write(prev);
        }
    }
    return true;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::split_node(std::stack<std::pair<size_t, size_t>> path)
{
    if (path.empty()){
        return;
    }
    size_t node_to_split_position = path.top().first;
    path.pop();
    size_t parent_node_position = path.empty() ? SIZE_MAX : path.top().first;

    auto node_to_split = disk_read(node_to_split_position);
    auto parent_node = disk_read(parent_node_position);

    auto root = disk_read(_position_root);

    size_t middle_ind = (maximum_keys_in_node+1) / 2;

    tkey middle_key = node_to_split.keys[middle_ind].first;
    tvalue middle_value = node_to_split.keys[middle_ind].second;
    size_t middle_index;
    if (parent_node.position_in_disk == SIZE_MAX) {
        //если сплитим корень, то нужно создать новый
        root = btree_disk_node(false);
        root.position_in_disk = _count_of_node++;
        parent_node = root;
        middle_index = 0;
        _position_root = root.position_in_disk;
    } else {
        size_t i;
        for(i = 0; i < parent_node.keys.size(); i++){
            if (!compare_keys(parent_node.keys[i].first, middle_key)){
                break;
            }
        }
        middle_index = i;
    }
    btree_disk_node left_part = btree_disk_node();
    btree_disk_node right_part = btree_disk_node();
    left_part.position_in_disk = _count_of_node++;
    right_part.position_in_disk = _count_of_node++;

    const auto it_keys_begin = node_to_split.keys.begin();
    const auto it_keys_end = node_to_split.keys.end();
    const auto it_point_begin = node_to_split.pointers.begin();
    const auto it_point_end = node_to_split.pointers.end();

    left_part.keys.insert(left_part.keys.begin(), it_keys_begin, it_keys_begin + middle_ind);
    left_part.pointers.insert(left_part.pointers.begin(), it_point_begin, it_point_begin + middle_ind);
    left_part.size = middle_ind;

    right_part.keys.insert(right_part.keys.begin(), it_keys_begin + middle_ind + 1, it_keys_end);
    right_part.pointers.insert(right_part.pointers.begin(), it_point_begin + middle_ind + 1, it_point_end);
    right_part.size = node_to_split.keys.size() - middle_ind - 1;

    parent_node.keys.insert(parent_node.keys.begin() + middle_index, node_to_split.keys[middle_ind]);
    parent_node.pointers.insert(parent_node.pointers.begin() + middle_index, left_part.position_in_disk);
    parent_node.pointers.insert(parent_node.pointers.begin() + middle_index + 1, right_part.position_in_disk);

    parent_node.size++;
    //в результате insert в pointers могут оказаться лишние указатели. Делаем их невалидными.
    for(size_t i = parent_node.size + 1; i < parent_node.pointers.size(); i++){
        parent_node.pointers[i] = SIZE_MAX;
    }

    disk_write(left_part);
    disk_write(right_part);
    disk_write(parent_node);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::insert_array(btree_disk_node& node, size_t right_node, const tree_data_type& data, size_t index) noexcept
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<std::stack<std::pair<size_t, size_t>>, std::pair<size_t,bool>>  B_tree_disk<tkey, tvalue, compare, t>::find_path(const tkey& key)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<size_t, bool> B_tree_disk<tkey, tvalue, compare, t>::find_index(const tkey &key, btree_disk_node& node) const noexcept
{

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::serialize(std::fstream &stream, std::fstream& stream_for_data) const
{
    stream.write(reinterpret_cast<const char*>(&size), sizeof(size_t));
    stream.write(reinterpret_cast<const char*>(&_is_leaf), sizeof(bool));
    stream.write(reinterpret_cast<const char*>(&position_in_disk), sizeof(size_t));
    size_t i;
    for (i = 0; i < std::min(maximum_pointers_in_node, pointers.size()); i++){
        size_t value = pointers[i];
        stream.write(reinterpret_cast<const char*>(&value), sizeof(size_t));
    }
    for (; i < maximum_pointers_in_node; i++){
        size_t value = SIZE_MAX;
        stream.write(reinterpret_cast<const char*>(&value), sizeof(size_t));
    }

    positions_in_key_value_file.assign(keys.size(), 0);
    for (i = 0; i < keys.size(); i++){
        tree_data_type value = keys[i];
        value.first.serialize(stream_for_data);
        value.second.serialize(stream_for_data);
        positions_in_key_value_file[i] = _last_position_in_key_value_file;
        _last_position_in_key_value_file += value.first.serialize_size() + value.second.serialize_size();
        stream.write(reinterpret_cast<const char*>(&positions_in_key_value_file[i]), sizeof(size_t));
    }
//    for (; i < maximum_keys_in_node; i++){
//        tree_data_type value = tree_data_type();
//        value.first.serialize(stream_for_data);
//        value.second.serialize(stream_for_data);
//        positions_in_key_value_file[i] = _last_position_in_key_value_file;
//        _last_position_in_key_value_file += value.first.serialize_size() + value.second.serialize_size();
//    }
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::disk_write(btree_disk_node& node)
{
    _file_for_tree.seekg(node.position_in_disk * btree_disk_node::node_metadata_size);
    _file_for_key_value.seekg(btree_disk_node::_last_position_in_key_value_file);
    node.serialize(_file_for_tree, _file_for_key_value);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::deserialize(std::fstream &stream, std::fstream& stream_for_data)
{
    btree_disk_node res;
    stream.read(reinterpret_cast<char*>(&res.size), sizeof(size_t));
    stream.read(reinterpret_cast<char*>(&res._is_leaf), sizeof(bool));
    stream.read(reinterpret_cast<char*>(&res.position_in_disk), sizeof(size_t));
    size_t i;
    for (i = 0; i < maximum_pointers_in_node; i++){
        size_t value;
        stream.read(reinterpret_cast<char*>(&value), sizeof(size_t));
        res.pointers.push_back(value);
    }

    res.positions_in_key_value_file.clear();
    for(i = 0; i < res.size; i++){
        size_t position;
        stream.read(reinterpret_cast<char*>(&position), sizeof(size_t));
        res.positions_in_key_value_file.push_back(position);
    }

    for (i = 0; i < res.size; i++){
        tree_data_type value;
        stream_for_data.seekg(res.positions_in_key_value_file[i]);
        value.first = tkey::deserialize(stream_for_data);
        value.second = tvalue::deserialize(stream_for_data);
        res.keys.push_back(value);
    }
    return res;
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node B_tree_disk<tkey, tvalue, compare, t>::disk_read(size_t node_position)
{
    auto res = btree_disk_node();
    if (node_position >= _count_of_node){
        res.position_in_disk = SIZE_MAX;
        return res;
    }
    _file_for_tree.seekg(node_position * btree_disk_node::node_metadata_size);
    res = btree_disk_node::deserialize(_file_for_tree, _file_for_key_value);
    return res;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node(bool is_leaf) : _is_leaf(is_leaf), size(0), position_in_disk(_count_of_node)
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_node::btree_disk_node() : _is_leaf(true), size(0), position_in_disk(_count_of_node)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::compare_pairs(const B_tree_disk::tree_data_type &lhs,
                                                          const B_tree_disk::tree_data_type &rhs) const
{
    return compare_keys(lhs.first, rhs.first);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::compare_keys(const tkey &lhs, const tkey &rhs) const
{
    return compare()(lhs, rhs);
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
size_t B_tree_disk<tkey, tvalue, compare, t>::_count_of_node = 0;

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::B_tree_disk(const std::string& file_path, const compare& cmp, logger* logger):  compare(cmp)
{
    std::filesystem::path file_path_path = std::filesystem::path(file_path);
    std::filesystem::path key_data_path = file_path_path.parent_path() / (file_path_path.stem().string()
                                                                            + "key_value"
                                                                            + file_path_path.extension().string());


    std::ofstream(file_path_path, std::ios::binary).close();
    std::ofstream(key_data_path, std::ios::binary).close();

    _file_for_tree = std::fstream (file_path_path, std::ios::binary | std::ios::out | std::ios::in);
    _file_for_key_value = std::fstream (key_data_path, std::ios::binary | std::ios::out | std::ios::in);
    if (!_file_for_tree.is_open() /*|| !_file_for_key_value.is_open()*/){
        throw std::runtime_error("incorrect file");
    }
    _logger = logger;
    _position_root = SIZE_MAX;
    _count_of_node = 0;
}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
void B_tree_disk<tkey, tvalue, compare, t>::check_tree(size_t pos, size_t depth)
{

}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::btree_disk_const_iterator(B_tree_disk<tkey, tvalue, compare, t>& tree, const std::stack<std::pair<size_t, size_t>> &path, size_t index) : _tree(tree), _path(path), _index(index) {}


template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::begin()
{


}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator B_tree_disk<tkey, tvalue, compare, t>::end()
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++()
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator++(int)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self& B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--()
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator--(int)
{

}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator==(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
bool B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator!=(B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::self &other) noexcept
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::value_type B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator::operator*() noexcept
{
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::optional<tvalue> B_tree_disk<tkey, tvalue, compare, t>::at(const tkey & key)
{
    size_t i = 0;
    size_t prev_i = 0;
    auto cur_node = disk_read(_position_root);
    auto cur_key = cur_node.keys[i];
    while(cur_node.position_in_disk != SIZE_MAX){
        i = 0;
        for(; i < cur_node.keys.size() && compare_keys(cur_node.keys[i].first, key); i++){}

        if (i < cur_node.keys.size() && cur_node.keys[i].first == key){
            return std::optional<tvalue>(cur_node.keys[i].second);
        }

        cur_node = disk_read(cur_node.pointers[i]);
        prev_i = i;
    }
    return {};
}

template<serializable tkey, serializable tvalue, compator<tkey> compare, std::size_t t>
std::pair<typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator, typename B_tree_disk<tkey, tvalue, compare, t>::btree_disk_const_iterator> B_tree_disk<tkey, tvalue, compare, t>::find_range(const tkey &lower, const tkey &upper, bool include_lower, bool include_upper)
{

}

#endif //B_TREE_DISK_HPP
