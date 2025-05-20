//
// Created by Des Caldnd on 2/28/2025.
//
#include "b_tree_disk.hpp"
#include "gtest/gtest.h"

#include <list>
#include <random>
#include <vector>
#include <client_logger_builder.h>
struct SerializableInt  {
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
    bool operator!=(const SerializableString& other) const {
        return value != other.value;
    }
};

template <typename tkey, typename tvalue>
struct test_data
{
    tkey key;
    tvalue value;
    size_t depth, index;

    test_data(size_t d, size_t i, tkey k, tvalue v) : depth(d), index(i), key(k), value(v) {}
};

template<typename tkey, typename tvalue, typename comp, size_t t>
bool infix_const_iterator_test(
        B_tree_disk<tkey, tvalue, comp, t> &tree,
std::vector<test_data<tkey, tvalue>> const &expected_result)
{
    auto end_infix = tree.end();
    auto it = tree.begin();

    for (auto const &item: expected_result)
    {
        auto data = *it;

        if ((*it).first != item.key || (*it).second != item.value || it.depth() != item.depth || it.index() != item.index)
        {
            return false;
        }

        ++it;
    }

    return true;
}

TEST(bTreeDiskPositiveTests, test1)
{

    B_tree_disk<SerializableInt, SerializableString> tree("temp.txt");
    std::vector<test_data<SerializableInt, SerializableString>> expected_result =
        {
                test_data<SerializableInt, SerializableString>(1, 0, SerializableInt(1), SerializableString("a")),
                test_data<SerializableInt, SerializableString>(1, 1, SerializableInt(2), SerializableString("b")),
                test_data<SerializableInt, SerializableString>(0, 0, SerializableInt(3), SerializableString("d")),
                test_data<SerializableInt, SerializableString>(1, 0, SerializableInt(4), SerializableString("e")),
                test_data<SerializableInt, SerializableString>(1, 1, SerializableInt(15), SerializableString("c")),
                test_data<SerializableInt, SerializableString>(1, 2, SerializableInt(27), SerializableString("f"))
        };



    tree.emplace(1, std::string("a"));
    auto nnode = tree.disk_read(0);
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(27, std::string("f"));

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));;
}


//TEST(bTreeDiskPositiveTests, test2)
//{
//
//    B_tree_disk<SerializableInt, SerializableString, std::less<>, 3> tree("temp.txt");
//    std::vector<test_data<SerializableInt, SerializableString>> expected_result =
//            {
//                    test_data<SerializableInt, SerializableString>(1, 0, SerializableInt(1), SerializableString("a")),
//                    test_data<SerializableInt, SerializableString>(1, 1, SerializableInt(2), SerializableString("b")),
//                    test_data<SerializableInt, SerializableString>(1, 2, SerializableInt(3), SerializableString("d")),
//                    test_data<SerializableInt, SerializableString>(0, 0, SerializableInt(4), SerializableString("e")),
//                    test_data<SerializableInt, SerializableString>(1, 0, SerializableInt(15), SerializableString("c")),
//                    test_data<SerializableInt, SerializableString>(1, 1, SerializableInt(27), SerializableString("f"))
//            };
//
//    tree.emplace(1, std::string("a"));
//    tree.emplace(2, std::string("b"));
//    tree.emplace(15, std::string("c"));
//    tree.emplace(3, std::string("d"));
//    tree.emplace(4, std::string("e"));
//    tree.emplace(27, std::string("f"));
//
//    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));;
//}
//
//TEST(bTreeDiskPositiveTests, test3)
//{
//    B_tree_disk<SerializableInt, SerializableString, std::less<>, 4> tree("temp.txt");
//    std::vector<test_data<SerializableInt, SerializableString>> expected_result =
//            {
//                    test_data<SerializableInt, SerializableString>(1, 0, SerializableInt(2), SerializableString("b")),
//                    test_data<SerializableInt, SerializableString>(1, 1, SerializableInt(3), SerializableString("d")),
//                    test_data<SerializableInt, SerializableString>(1, 2, SerializableInt(4), SerializableString("e")),
//                    test_data<SerializableInt, SerializableString>(0, 0, SerializableInt(15), SerializableString("c")),
//                    test_data<SerializableInt, SerializableString>(1, 0, SerializableInt(45), SerializableString("k")),
//                    test_data<SerializableInt, SerializableString>(1, 1, SerializableInt(101), SerializableString("j")),
//                    test_data<SerializableInt, SerializableString>(1, 2, SerializableInt(456), SerializableString("h")),
//                    test_data<SerializableInt, SerializableString>(1, 3, SerializableInt(534), SerializableString("m"))
//            };
//
//    tree.emplace(SerializableInt(1), SerializableString("a"));
//    tree.emplace(SerializableInt(2), SerializableString("b"));
//    tree.emplace(SerializableInt(15), SerializableString("c"));
//    tree.emplace(SerializableInt(3), SerializableString("d"));
//    tree.emplace(SerializableInt(4), SerializableString("e"));
//    tree.emplace(SerializableInt(100), SerializableString("f"));
//    tree.emplace(SerializableInt(24), SerializableString("g"));
//    tree.emplace(SerializableInt(456), SerializableString("h"));
//    tree.emplace(SerializableInt(101), SerializableString("j"));
//    tree.emplace(SerializableInt(45), SerializableString("k"));
//    tree.emplace(SerializableInt(193), SerializableString("l"));
//    tree.emplace(SerializableInt(534), SerializableString("m"));
//
//    tree.erase(SerializableInt(1));
//    tree.erase(SerializableInt(100));
//    tree.erase(SerializableInt(193));
//    tree.erase(SerializableInt(24));
//
//    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));
//}


int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}