//
// Created by Des Caldnd on 2/28/2025.
//
#include "b_tree_disk.hpp"
struct SerializableInt {
    int value;

    void serialize(std::fstream& s) const {
        s.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }

    static SerializableInt deserialize(std::fstream& s) {
        SerializableInt obj;
        s.read(reinterpret_cast<char*>(&obj.value), sizeof(obj.value));
        return obj;
    }

    size_t serialize_size() const {
        return sizeof(value);
    }

    bool operator<(const SerializableInt& other) const {
        return value < other.value;
    }

    bool operator==(const SerializableInt& other){
        return value == other.value;
    }
};

struct SerializableString {
    std::string value;

    void serialize(std::fstream& s) const {
        // Сначала записываем длину строки
        size_t size = value.size();
        s.write(reinterpret_cast<const char*>(&size), sizeof(size));

        // Затем записываем сами данные (если строка не пустая)
        if (!value.empty()) {
            s.write(value.data(), static_cast<std::streamsize>(size));
        }
    }

    static SerializableString deserialize(std::fstream& s) {
        SerializableString obj;

        // Читаем длину строки
        size_t size = 0;
        s.read(reinterpret_cast<char*>(&size), sizeof(size));

        // Читаем данные строки
        if (size > 0) {
            obj.value.resize(size);
            s.read(obj.value.data(), static_cast<std::streamsize>(size));
        }

        return obj;
    }

    size_t serialize_size() const {
        // Размер = размер size_t (длина строки) + размер данных строки
        return sizeof(size_t) + value.size();
    }
};

int main()
{
    B_tree_disk<SerializableInt, SerializableString> tree("temp.txt");
    tree.insert(std::pair(SerializableInt(0), SerializableString("aboba0")));
    tree.insert(std::pair(SerializableInt(1), SerializableString("aboba1")));
    tree.insert(std::pair(SerializableInt(2), SerializableString("aboba2")));
    auto root = tree.disk_read(tree._position_root);
    tree.insert(std::pair(SerializableInt(3), SerializableString("aboba3")));
    root = tree.disk_read(tree._position_root);
    tree.insert(std::pair(SerializableInt(4), SerializableString("aboba4")));
    tree.insert(std::pair(SerializableInt(5), SerializableString("aboba5")));
    tree.insert(std::pair(SerializableInt(6), SerializableString("aboba6")));
    root = tree.disk_read(tree._position_root);
    tree.erase(SerializableInt(6));
    root = tree.disk_read(tree._position_root);
//    auto node1 = tree.disk_read(0);
    auto node2 = tree.disk_read(5);
    auto node8 = tree.disk_read(2);
//    auto node3 = tree.disk_read(2);
    auto node4 = tree.disk_read(2);
    auto node5 = tree.disk_read(4);
    auto node6 = tree.disk_read(5);

    tree.insert(std::pair(SerializableInt(4), SerializableString("aboba4")));

    auto value = tree.at(SerializableInt(2));
}