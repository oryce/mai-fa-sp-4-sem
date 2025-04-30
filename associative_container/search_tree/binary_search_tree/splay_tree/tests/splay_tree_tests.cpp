#include "gtest/gtest.h"
#include <splay_tree.h>
#include <logger_builder.h>
#include <client_logger_builder.h>
#include <iostream>

logger *create_logger(
        std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
        bool use_console_stream = true,
        logger::severity console_stream_severity = logger::severity::debug)
{
    std::unique_ptr<logger_builder> builder(new client_logger_builder());

    builder->set_format("[%d %t][%s] %m");

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

bool compare_results(
        std::vector<typename binary_search_tree<int, std::string>::value_type> &expected,
        std::vector<typename binary_search_tree<int, std::string>::value_type> &actual)
{

    if (expected.size() != actual.size())
    {
        return false;
    }

    for (auto&& val : std::views::zip(expected, actual))
    {
        if (std::get<0>(val).first != std::get<1>(val).first)
        {
            return false;
        }

        if (std::get<0>(val).second != std::get<1>(val).second)
        {
            return false;
        }
    }

    return true;
}

template<typename tkey, typename tvalue>
struct test_data
{
    tkey key;
    tvalue value;
    size_t depth;

    test_data(size_t dep, tkey k, tvalue v) : depth(dep), key(k), value(v) {}
};

template<typename tkey, typename tvalue>
bool infix_iterator_test(
        splay_tree<tkey, tvalue> const &tree,
        std::vector<test_data<tkey, tvalue>> &expected_result)
{
    auto end_infix = tree.cend_infix();
    auto it = tree.cbegin_infix();

    auto vec_begin = expected_result.begin(), vec_end = expected_result.end();

    for (; vec_begin != vec_end; ++vec_begin)
    {
        auto &item = *vec_begin;
        if (it.depth() != item.depth || it->first != item.key || it->second != item.value)
        {
            return false;
        }

        ++it;
    }

    return true;
}

template<
        typename tkey,
        typename tvalue>
bool prefix_iterator_test(
        splay_tree<tkey, tvalue> const &tree,
        std::vector<test_data<tkey, tvalue>> &expected_result)
{
    auto end_prefix = tree.cend_prefix();
    auto it = tree.cbegin_prefix();

    for (auto const &item: expected_result)
    {
        if (it.depth() != item.depth || it->first != item.key || it->second != item.value)
        {
            return false;
        }

        ++it;
    }

    return true;
}

template<
        typename tkey,
        typename tvalue>
bool postfix_iterator_test(
        splay_tree<tkey, tvalue> const &tree,
        std::vector<test_data<tkey, tvalue>> &expected_result)
{
    std::string line;
    auto end_postfix = tree.cend_postfix();
    auto it = tree.cbegin_postfix();

    for (auto &item: expected_result)
    {
        if (it.depth() != item.depth || it->first != item.key || it->second != item.value)
        {
            return false;
        }

        ++it;
    }

    return true;
}

TEST(splayTreePositiveTests, test1)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                       {
                                           {
                                               "splay_tree_tests_logs.txt",
                                               logger::severity::trace
                                           },
                                       }));

    logger->trace("splayTreePositiveTests.test1 started");

    auto splay = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay->emplace(5, "a");
    splay->emplace(2, "b");
    splay->emplace(15, "c");
    splay->emplace(3, "d");
    splay->emplace(14, "e");
    splay->emplace(1, "l");

    std::vector<test_data<int, std::string>> expected_result =
        {
            test_data<int, std::string>(0, 1, "l"),
            test_data<int, std::string>(2, 2, "b"),
            test_data<int, std::string>(3, 3, "d"),
            test_data<int, std::string>(4, 5, "a"),
            test_data<int, std::string>(1, 14, "e"),
            test_data<int, std::string>(2, 15, "c")
        };

    EXPECT_TRUE(infix_iterator_test(*splay, expected_result));

    logger->trace("splayTreePositiveTests.test1 finished");
}

TEST(splayTreePositiveTests, test2)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test2 started");

    auto splay = std::make_unique<splay_tree<int, int>>(std::less<int>(), nullptr, logger.get());

    splay->emplace(1, 5);
    splay->emplace(2, 12);
    splay->emplace(15, 1);
    splay->emplace(3, 67);
    splay->emplace(4, 45);

    std::vector<test_data<int, int>> expected_result =
        {
                test_data<int, int>(0, 4, 45),
                test_data<int, int>(1, 3, 67),
                test_data<int, int>(2, 2, 12),
                test_data<int, int>(3, 1, 5),
                test_data<int, int>(1, 15, 1)
        };

    EXPECT_TRUE(prefix_iterator_test(*splay, expected_result));

    logger->trace("splayTreePositiveTests.test2 finished");
}

TEST(splayTreePositiveTests, test3)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test3 started");

    auto splay = std::make_unique<splay_tree<std::string, int>>(std::less<std::string>(), nullptr, logger.get());

    splay->emplace("a", 1);
    splay->emplace("b", 2);
    splay->emplace("c", 15);
    splay->emplace("d", 3);
    splay->emplace("e", 4);

    std::vector<test_data<std::string, int>> expected_result =
        {
                test_data<std::string, int>(4, "a", 1),
                test_data<std::string, int>(3, "b", 2),
                test_data<std::string, int>(2, "c", 15),
                test_data<std::string, int>(1, "d", 3),
                test_data<std::string, int>(0, "e", 4)
        };

    EXPECT_TRUE(postfix_iterator_test(*splay, expected_result));

    logger->trace("splayTreePositiveTests.test3 finished");
}

TEST(splayTreePositiveTests, test4)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test4 started");

    auto splay1 = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay1->emplace(6, "a");
    splay1->emplace(8, "c");
    splay1->emplace(15, "x");
    splay1->emplace(4, "j");
    splay1->emplace(1, "i");
    splay1->emplace(5, "b");

    std::vector<test_data<int, std::string>> expected_result =
        {
            test_data<int, std::string>(2, 1, "i"),
            test_data<int, std::string>(1, 4, "j"),
            test_data<int, std::string>(0, 5, "b"),
            test_data<int, std::string>(1, 6, "a"),
            test_data<int, std::string>(3, 8, "c"),
            test_data<int, std::string>(2, 15, "x")
        };

    splay_tree<int, std::string> splay2(std::move(*splay1));

    EXPECT_TRUE(infix_iterator_test(splay2, expected_result));

    logger->trace("splayTreePositiveTests.test4 finished");
}

TEST(splayTreePositiveTests, test5)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test5 started");

    auto splay1 = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay1->emplace(6, "a");
    splay1->emplace(8, "c");
    splay1->emplace(15, "x");
    splay1->emplace(4, "j");
    splay1->emplace(1, "i");
    splay1->emplace(5, "b");

    std::vector<test_data<int, std::string>> expected_result =
        {
            test_data<int, std::string>(2, 1, "i"),
            test_data<int, std::string>(1, 4, "j"),
            test_data<int, std::string>(0, 5, "b"),
            test_data<int, std::string>(1, 6, "a"),
            test_data<int, std::string>(3, 8, "c"),
            test_data<int, std::string>(2, 15, "x")
        };

    splay_tree<int, std::string> splay2 = std::move(*splay1);

    EXPECT_TRUE(infix_iterator_test(splay2, expected_result));

    logger->trace("splayTreePositiveTests.test5 finished");
}

TEST(splayTreePositiveTests, test6)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test6 started");

    auto splay1 = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay1->emplace(6, "a");
    splay1->emplace(8, "c");
    splay1->emplace(15, "x");
    splay1->emplace(4, "j");
    splay1->emplace(1, "i");
    splay1->emplace(5, "b");

    splay1->erase(5);

    std::vector<test_data<int, std::string>> expected_result =
        {
            test_data<int, std::string>(1, 1, "i"),
            test_data<int, std::string>(0, 4, "j"),
            test_data<int, std::string>(1, 6, "a"),
            test_data<int, std::string>(3, 8, "c"),
            test_data<int, std::string>(2, 15, "x"),
        };

    EXPECT_TRUE(infix_iterator_test(*splay1, expected_result));

    logger->trace("splayTreePositiveTests.test6 finished");
}

TEST(splayTreePositiveTests, test7)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test7 started");

    auto splay1 = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay1->emplace(6, "a");
    splay1->emplace(8, "c");
    splay1->emplace(15, "x");
    splay1->emplace(4, "j");
    splay1->emplace(3, "i");
    splay1->emplace(2, "l");
    splay1->emplace(5, "b");

    splay1->erase(3);

    std::vector<test_data<int, std::string>> expected_result =
        {
            test_data<int, std::string>(0, 2, "l"),
            test_data<int, std::string>(2, 4, "j"),
            test_data<int, std::string>(1, 5, "b"),
            test_data<int, std::string>(2, 6, "a"),
            test_data<int, std::string>(4, 8, "c"),
            test_data<int, std::string>(3, 15, "x")
        };

    EXPECT_TRUE(infix_iterator_test(*splay1, expected_result));

    logger->trace("splayTreePositiveTests.test7 finished");
}

TEST(splayTreePositiveTests, test8)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test8 started");

    auto splay1 = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay1->emplace(6, "a");
    splay1->emplace(8, "c");
    splay1->emplace(15, "x");
    splay1->emplace(11, "j");
    splay1->emplace(19, "i");
    splay1->emplace(12, "l");
    splay1->emplace(17, "b");
    splay1->emplace(18, "e");

    splay1->erase(12);

    std::vector<test_data<int, std::string>> expected_result =
        {
            test_data<int, std::string>(2, 6, "a"),
            test_data<int, std::string>(1, 8, "c"),
            test_data<int, std::string>(0, 11, "j"),
            test_data<int, std::string>(2, 15, "x"),
            test_data<int, std::string>(1, 17, "b"),
            test_data<int, std::string>(2, 18, "e"),
            test_data<int, std::string>(3, 19, "i"),
        };

    EXPECT_TRUE(infix_iterator_test(*splay1, expected_result));

    logger->trace("splayTreePositiveTests.test8 finished");
}

TEST(splayTreePositiveTests, test9)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test9 started");

    auto splay1 = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay1->emplace(6, "l");
    splay1->emplace(8, "c");
    splay1->emplace(15, "l");
    splay1->emplace(11, "o");
    splay1->emplace(9, "h");
    splay1->emplace(2, "e");
    splay1->emplace(4, "b");
    splay1->emplace(18, "e");

    std::vector<std::string> vector;

    vector.push_back(splay1->at(9));
    vector.push_back(splay1->at(2));
    vector.push_back(splay1->at(15));
    vector.push_back(splay1->at(6));
    vector.push_back(splay1->at(11));

    std::string actual_result;

    for (auto const &item: vector)
    {
        actual_result.append(item + " ");
    }

    EXPECT_EQ(actual_result, "h e l l o ");

    logger->trace("splayTreePositiveTests.test9 finished");
}

TEST(splayTreePositiveTests, test10)
{
    std::unique_ptr<logger> logger (create_logger(std::vector<std::pair<std::string, logger::severity>>
                                                          {
                                                                  {
                                                                          "splay_tree_tests_logs.txt",
                                                                          logger::severity::trace
                                                                  },
                                                          }));

    logger->trace("splayTreePositiveTests.test10 started");

    auto splay = std::make_unique<splay_tree<int, std::string>>(std::less<int>(), nullptr, logger.get());

    splay->emplace(6, "l");
    splay->emplace(8, "c");
    splay->emplace(15, "l");
    splay->emplace(11, "o");
    splay->emplace(9, "h");
    splay->emplace(2, "e");
    splay->emplace(4, "b");
    splay->emplace(18, "e");

    auto b = splay->lower_bound(2);
    auto e = splay->upper_bound(10);
    std::vector<typename splay_tree<int, std::string>::value_type> actual_result(b, e);

    std::vector<typename splay_tree<int, std::string>::value_type> expected_result =
        {
            { 2, "e" },
            { 4, "b" },
            { 6, "l" },
            { 8, "c" },
            { 9, "h" }
        };
    auto it = splay->begin();
    while(it != splay->end_infix()){
        std::cout << it.get_data()->data.first << " ";
        it++;
    }
    EXPECT_TRUE(compare_results(expected_result, actual_result));
    //2 4 6 8 9 11 15 18
    logger->trace("splayTreePositiveTests.test10 finished");
}

int main(
    int argc,
    char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    
    return RUN_ALL_TESTS();
}