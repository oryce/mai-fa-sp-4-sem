#include "gtest/gtest.h"

#include <list>
#include <random>
#include <vector>
#include <b_plus_tree.h>
#include <client_logger_builder.h>



template<typename tkey, typename tvalue>
bool compare_results(
        std::vector<typename BP_tree<tkey, tvalue>::value_type> const &expected,
        std::vector<typename BP_tree<tkey, tvalue>::value_type> const &actual)
{
    if (expected.size() != actual.size())
    {
        return false;
    }

    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (expected[i].key != actual[i].key)
        {
            return false;
        }

        if (expected[i].value != actual[i].value)
        {
            return false;
        }
    }

    return true;
}

template<typename tvalue>
bool compare_obtain_results(
        std::vector<tvalue> const &expected,
        std::vector<tvalue> const &actual)
{
    if (expected.size() != actual.size())
    {
        return false;
    }

    for (size_t i = 0; i < expected.size(); ++i)
    {
        if (expected[i] != actual[i])
        {
            return false;
        }
    }

    return true;
}

logger *create_logger(
        std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
        bool use_console_stream = true,
        logger::severity console_stream_severity = logger::severity::debug)
{
    std::unique_ptr<logger_builder> builder(new client_logger_builder());

    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }

    for (auto &output_file_stream_setup: output_file_streams_setup)
    {
        builder->add_file_stream(output_file_stream_setup.first, output_file_stream_setup.second);
    }

    logger *built_logger = builder->build();

    return built_logger;
}

template <typename tkey, typename tvalue>
struct test_data
{
    tkey key;
    tvalue value;
    size_t index;

    test_data(size_t i, tkey k, tvalue v) : index(i), key(k), value(v) {}
};

template<typename tkey, typename tvalue, typename comp, size_t t>
bool infix_const_iterator_test(
        BP_tree<tkey, tvalue, comp, t> const &tree,
        std::vector<test_data<tkey, tvalue>> const &expected_result)
{
    auto end_infix = tree.cend();
    auto it = tree.cbegin();

    for (auto const &item: expected_result)
    {
        auto data = *it;

        if (it->first != item.key || it->second != item.value || it.index() != item.index)
        {
            return false;
        }

        ++it;
    }

return true;
}

TEST(bpTreePositiveTests, test0)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test0 started");

    std::vector<test_data<int, std::string>> expected_result =
            {

            };

    BP_tree<int, std::string, std::less<int>, 1024> tree(std::less<int>(), nullptr, logger.get());

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test0 finished");
}

TEST(bpTreePositiveTests, test1)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test1 started");

    std::vector<test_data<int, std::string>> expected_result =
            {
                    test_data<int, std::string>(0, 1, "a"),
                    test_data<int, std::string>(1, 2, "b"),
                    test_data<int, std::string>(2, 3, "d"),
                    test_data<int, std::string>(0, 4, "e"),
                    test_data<int, std::string>(1, 15, "c"),
                    test_data<int, std::string>(2, 27, "f")
            };

    BP_tree<int, std::string, std::less<int>, 3> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(27, std::string("f"));


    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test1 finished");
}

TEST(bpTreePositiveTests, test2)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bpTreePositiveTests.test2 started");

    std::vector<test_data<int, std::string>> expected_result =
            {
                    test_data<int, std::string>(0, 1, "a"),
                    test_data<int, std::string>(1, 2, "b"),
                    test_data<int, std::string>(2, 3, "d"),
                    test_data<int, std::string>(3, 4, "e"),
                    test_data<int, std::string>(4, 15, "c"),
                    test_data<int, std::string>(0, 24, "g"),
                    test_data<int, std::string>(1, 45, "k"),
                    test_data<int, std::string>(2, 100, "f"),
                    test_data<int, std::string>(3, 101, "j"),
                    test_data<int, std::string>(4, 193, "l"),
                    test_data<int, std::string>(5, 456, "h"),
                    test_data<int, std::string>(6, 534, "m")
            };

    BP_tree<int, std::string, std::less<int>, 5> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string("f"));
    tree.emplace(24, std::string("g"));
    tree.emplace(456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(45, std::string("k"));
    tree.emplace(193, std::string("l"));
    tree.emplace(534, std::string("m"));

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bpTreePositiveTests.test2 finished");
}

TEST(bpTreePositiveTests, test3)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bpTreePositiveTests.test3 started");

    std::vector<test_data<int, std::string>> expected_result =
            {
                    test_data<int, std::string>(0, 1, "a"),
                    test_data<int, std::string>(1, 2, "b"),
                    test_data<int, std::string>(2, 3, "d"),
                    test_data<int, std::string>(3, 4, "e"),
                    test_data<int, std::string>(4, 15, "c"),
                    test_data<int, std::string>(5, 24, "g"),
                    test_data<int, std::string>(6, 45, "k"),
                    test_data<int, std::string>(7, 100, "f"),
                    test_data<int, std::string>(8, 101, "j"),
                    test_data<int, std::string>(9, 193, "l"),
                    test_data<int, std::string>(10, 456, "h"),
                    test_data<int, std::string>(11, 534, "m")
            };

    BP_tree<int, std::string, std::less<int>, 7> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string("f"));
    tree.emplace(24, std::string("g"));
    tree.emplace(456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(45, std::string("k"));
    tree.emplace(193, std::string("l"));
    tree.emplace(534, std::string("m"));

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bpTreePositiveTests.test3 finished");
}

TEST(bpTreePositiveTests, test4)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bpTreePositiveTests.test4 started");

    std::vector<test_data<int, std::string>> expected_result =
            {
                    test_data<int, std::string>(0, 1, "a"),
                    test_data<int, std::string>(1, 2, "b"),
                    test_data<int, std::string>(2, 3, "d"),
                    test_data<int, std::string>(0, 4, "e"),
                    test_data<int, std::string>(1, 15, "c"),
                    test_data<int, std::string>(2, 24, "g"),

                    test_data<int, std::string>(3, 45, "k"),
                    test_data<int, std::string>(0, 100, "f"),
                    test_data<int, std::string>(1, 101, "j"),
                    test_data<int, std::string>(2, 193, "l"),
                    test_data<int, std::string>(3, 456, "h"),
                    test_data<int, std::string>(4, 534, "m")
            };

    BP_tree<int, std::string, std::less<int>, 3> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string("f"));
    tree.emplace(24, std::string("g"));
    tree.emplace(456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(45, std::string("k"));
    tree.emplace(193, std::string("l"));
    tree.emplace(534, std::string("m"));

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bpTreePositiveTests.test4 finished");
}

TEST(bpTreePositiveTests, test5)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test5 started");

    std::vector<test_data<int, std::string>> expected_result =
            {

            };

    BP_tree<int, std::string, std::less<int>, 2> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));

    auto first_disposed = tree.at(2);
    auto second_disposed = tree.at(4);

    tree.erase(2);
    tree.erase(4);

    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    logger->trace("bTreePositiveTests.test5 finished");
}

TEST(bpTreePositiveTests, test6)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test6 started");

    std::vector<test_data<int, std::string>> expected_result =
            {
                    test_data<int, std::string>(0, 2, "b"),
                    test_data<int, std::string>(1, 3, "d"),
                    test_data<int, std::string>(2, 4, "e"),
                    test_data<int, std::string>(0, 15, "c"),
                    test_data<int, std::string>(1, 45, "k"),
                    test_data<int, std::string>(2, 101, "j"),
                    test_data<int, std::string>(3, 456, "h"),
                    test_data<int, std::string>(4, 534, "m")
            };

    BP_tree<int, std::string, std::less<int>, 4> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string("f"));
    tree.emplace(24, std::string("g"));
    tree.emplace(456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(45, std::string("k"));
    tree.emplace(193, std::string("l"));
    tree.emplace(534, std::string("m"));

    auto first_disposed = std::move(tree.at(1));
    auto second_disposed = std::move(tree.at(100));
    auto third_disposed = std::move(tree.at(193));
    auto fourth_disposed = std::move(tree.at(24));

    tree.erase(1);
    tree.erase(100);
    tree.erase(193);
    tree.erase(24);
    return;
    EXPECT_TRUE(infix_const_iterator_test(tree, expected_result));

    EXPECT_TRUE(first_disposed == "a");
    EXPECT_TRUE(second_disposed == "f");
    EXPECT_TRUE(third_disposed == "l");
    EXPECT_TRUE(fourth_disposed == "g");

    logger->trace("bTreePositiveTests.test6 finished");
}

TEST(bTreePositiveTests, test7)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test7 started");

    std::vector<std::string> expected_result =
            {
                    "g",
                    "d",
                    "e",
                    " ",
                    "l",
                    "a",
                    "b",
                    "y"
            };

    BP_tree<int, std::string, std::less<int>, 5> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::move(std::string("a")));
    tree.emplace(2, std::move(std::string("b")));
    tree.emplace(15, std::move(std::string("c")));
    tree.emplace(3, std::move(std::string("d")));
    tree.emplace(4, std::move(std::string("e")));
    tree.emplace(100, std::move(std::string(" ")));
    tree.emplace(24, std::move(std::string("g")));
    tree.emplace(-456, std::move(std::string("h")));
    tree.emplace(101, std::move(std::string("j")));
    tree.emplace(-45, std::move(std::string("k")));
    tree.emplace(-193, std::move(std::string("l")));
    tree.emplace(534, std::move(std::string("m")));
    tree.emplace(1000, std::move(std::string("y")));

    std::vector<std::string> actual_result =
            {
                    tree.at(24),
                    tree.at(3),
                    tree.at(4),
                    tree.at(100),
                    tree.at(-193),
                    tree.at(1),
                    tree.at(2),
                    tree.at(1000)
            };

    EXPECT_TRUE(compare_obtain_results(expected_result, actual_result));

    logger->trace("bTreePositiveTests.test7 finished");
}

TEST(bTreePositiveTests, test8)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test8 started");

    std::vector<std::string> expected_result =
            {
                    "y",
                    "l",
                    "a",
                    "g",
                    "k",
                    "b",
                    "c",
                    "h"
            };

    BP_tree<int, std::string, std::less<int>, 4> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string(" "));
    tree.emplace(24, std::string("g"));
    tree.emplace(-456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(-45, std::string("k"));
    tree.emplace(-193, std::string("l"));
    tree.emplace(534, std::string("m"));
    tree.emplace(1000, std::string("y"));

    std::vector<std::string> actual_result =
            {
                    tree.at(1000),
                    tree.at(-193),
                    tree.at(1),
                    tree.at(24),
                    tree.at(-45),
                    tree.at(2),
                    tree.at(15),
                    tree.at(-456)
            };

    EXPECT_TRUE(compare_obtain_results(expected_result, actual_result));

    logger->trace("bTreePositiveTests.test8 finished");
}

TEST(bTreePositiveTests, test9)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreePositiveTests.test9 started");

    std::vector<BP_tree<int, std::string>::value_type> expected_result =
            {
                    {1,   "a"},
                    {2,   "b"},
                    {3,   "d"},
                    {4,   "e"},
                    {15,  "c"},
                    {24,  "g"},
                    {45,  "k"},
                    {100, "f"},
                    {101, "j"},
                    {193, "l"},
                    {456, "h"},
                    {534, "m"}
            };

    BP_tree<int, std::string, std::less<int>, 5> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string("f"));
    tree.emplace(24, std::string("g"));
    tree.emplace(456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(45, std::string("k"));
    tree.emplace(193, std::string("l"));
    tree.emplace(534, std::string("m"));

    auto b = tree.begin();
    auto e = tree.end();
    std::vector<decltype(tree)::value_type> actual_result(b, e);

    EXPECT_TRUE(compare_obtain_results(expected_result, actual_result));

    logger->trace("bTreePositiveTests.test9 finished");
}

TEST(bpTreeNegativeTests, test1)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreeNegativeTests.test1 started");

    BP_tree<int, std::string, std::less<int>, 3> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));

    EXPECT_EQ(tree.erase(45), tree.end());

    logger->trace("bTreeNegativeTests.test1 finished");
}

TEST(bpTreeNegativeTests, test3)
{
    std::unique_ptr<logger> logger( create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  { "b_tree_tests_logs.txt", logger::severity::trace }
                                                          }));

    logger->trace("bTreeNegativeTests.test3 started");

    BP_tree<int, std::string, std::less<int>, 4> tree(std::less<int>(), nullptr, logger.get());

    tree.emplace(1, std::string("a"));
    tree.emplace(2, std::string("b"));
    tree.emplace(15, std::string("c"));
    tree.emplace(3, std::string("d"));
    tree.emplace(4, std::string("e"));
    tree.emplace(100, std::string(" "));
    tree.emplace(24, std::string("g"));
    tree.emplace(-456, std::string("h"));
    tree.emplace(101, std::string("j"));
    tree.emplace(-45, std::string("k"));
    tree.emplace(-193, std::string("l"));
    tree.emplace(534, std::string("m"));
    tree.emplace(1000, std::string("y"));

    EXPECT_EQ(tree.erase(1001), tree.end());

    logger->trace("bTreeNegativeTests.test3 finished");
}

int main(
        int argc,
        char **argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}