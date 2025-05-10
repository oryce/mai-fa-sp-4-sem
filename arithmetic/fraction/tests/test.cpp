
#include <client_logger.h>
#include <client_logger_builder.h>
#include <fraction.h>
#include <gtest/gtest.h>

logger *create_logger(std::vector<std::pair<std::string, logger::severity>> const &output_file_streams_setup,
                      const bool use_console_stream = true,
                      const logger::severity console_stream_severity = logger::severity::debug)
{
    logger_builder *builder = new client_logger_builder();

    builder->set_format("[%d|%t] %s: %m");

    if (use_console_stream)
    {
        builder->add_console_stream(console_stream_severity);
    }

    for (const auto &[fst, snd] : output_file_streams_setup)
    {
        builder->add_file_stream(fst, snd);
    }

    logger *built_logger = builder->build();

    delete builder;

    return built_logger;
}

fraction abs(fraction obj)
{
    obj = obj < fraction(0_bi, 1_bi) ? fraction(big_int(-1), 1_bi) * obj : obj;
    return obj;
};

fraction half_pi()
{
    return {big_int(15707963), big_int(10000000)};
}

TEST(easyTests, test0)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(0), big_int(1));
    EXPECT_TRUE(a.to_string() == "0/1");
    logger->debug(a.to_string());
}

TEST(easyTests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int("123456789"), big_int("741258963"));
    EXPECT_TRUE(a.to_string() == "13717421/82362107");
    logger->debug(a.to_string());
}

TEST(easyTests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int("0"), big_int("-741258963"));
    EXPECT_TRUE(a.to_string() == "0/1");
    logger->debug(a.to_string());
}

TEST(easyTests, test3)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int("52"), big_int("100"));
    const fraction b(big_int("51"), big_int("98"));
    EXPECT_TRUE(a < b);
    logger->debug(a.to_string() + "<" + b.to_string());
}

TEST(plusTests, test0)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(0), big_int(1));
    const fraction b(big_int(3), big_int(4));
    const fraction c = a + b;
    logger->debug(a.to_string() + " + " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "3/4");
}

TEST(plusTests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(3), big_int(4));
    const fraction b(big_int(3), big_int(4));
    const fraction c = a + b;
    logger->debug(a.to_string() + " + " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(c.to_string() == "3/2");
}

TEST(plusTests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(3), big_int(4));
    const fraction b(big_int(-3), big_int(4));
    const fraction c = a + b;
    logger->debug(a.to_string() + " + " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "0/1");
}

TEST(minusTests, test0)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(3), big_int(4));
    const fraction b(big_int(-3), big_int(4));
    const fraction c = a - b;
    logger->debug(a.to_string() + " - " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "3/2");
}

TEST(minusTests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(3), big_int(4));
    const fraction b(big_int(0), big_int(1));
    const fraction c = a - b;
    logger->debug(a.to_string() + " - " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "3/4");
}

TEST(minusTests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(0_bi, 1_bi);
    std::cout << a.to_string() << std::endl;
    const fraction b(big_int(3), big_int(4));
    std::cout << b.to_string() << std::endl;
    const fraction c = a - b;
    logger->debug(a.to_string() + " - " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "-3/4");
}

TEST(minusTests, test3)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int("12342435346438958724438792758934689457646434323544564567456353246467546742553890454890356"
                             "745895343687456894678934854493068450697557345353"),
                     big_int("42389428935349086840957804985309763636567574564")};
    const fraction b(big_int("54357893745346457544353"), big_int(mm));
    const auto c = a - b;
    logger->debug(a.to_string() + " - " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "7589258202735538614647726274633256197256245382387532078645125708292665264936599215603"
                                 "4920054642347300531727688972205432281983223102930209769105510223086001/"
                                 "260648982333694318543135119304209864368688475405349118253506373620");
}

TEST(multiplicationTests, test0)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(3), big_int(-4));
    const fraction b(big_int(-3), big_int(4));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(c.to_string() == "9/16");
    const fraction d(big_int(0), big_int(1));
    const fraction e = b * d;
    logger->debug(b.to_string() + " * " + d.to_string() + " = " + e.to_string());
    EXPECT_TRUE(e.to_string() == "0/1");
}

TEST(multiplicationTests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(3), big_int(4));
    const fraction b(big_int(3), big_int(4));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(a == b);
    EXPECT_TRUE(c.to_string() == "9/16");
}

TEST(multiplicationTests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(8), big_int(15));
    const fraction b(big_int(3), big_int(4));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "2/5");
}

TEST(multiplicationTests, test3)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int(mm), big_int(mm)};
    const fraction b(big_int(3), big_int(4));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "3/4");
}

TEST(multiplicationTests, test4)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int(mm), big_int(mm)};
    const fraction b(big_int(3), big_int(4));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "3/4");
}

TEST(multiplicationTests, test5)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int(mm), big_int(1)};
    const fraction b(big_int(1), big_int(mm));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "1/1");
}

TEST(multiplicationTests, test6)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int("1"), big_int("2")};
    const fraction b(big_int("-54357893745346457544353"), big_int(mm));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "-54357893745346457544353/36893488147419103230");
}

TEST(multiplicationTests, test7)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{
        big_int("12342435346438958724438792758934689457646434323544564567456353246467546742553890454890356"),
        big_int("42389428935349086840957804985309763636567574564")};
    const fraction b(big_int("-54357893745346457544353"), big_int(mm));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() ==
                "-16772719728013432807823688550228165957015723628678980339118151930686757104696122822485828806199777471"
                "4905489917/195486736750270738907351339478157398276516356554011838690129780215");
}

TEST(multiplicationTests, test8)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int("12342435346438958724438792758934689457646434323544564567456353246467546742553890454890356"
                             "745895343687456894678934854493068450697557345353"),
                     big_int("42389428935349086840957804985309763636567574564")};
    const fraction b(big_int("-54357893745346457544353"), big_int(mm));
    const fraction c = a * b;
    logger->debug(a.to_string() + " * " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "-745454210133930347014386157787918486978476605719065792849695641363855871319827680999"
                                 "37021421476770685675711001276665107694822674991463759028609416821992882401/"
                                 "86882994111231439514378373101403288122896158468449706084502124540");
}

TEST(divTests, test0)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{big_int(mm), big_int(mm)};
    const fraction b(big_int(3), big_int(4));
    const fraction c = a / b;
    logger->debug(a.to_string() + " / " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "4/3");
}

TEST(divTests, test1)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a(big_int(8), big_int(15));
    const fraction b(big_int(3), big_int(4));
    const fraction c = a / b;
    logger->debug(a.to_string() + " / " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() == "32/45");
}

TEST(divTests, test2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    constexpr auto mm = std::numeric_limits<uint64_t>::max();
    const fraction a{
        big_int("12342435346438958724438792758934689457646434323544564567456353246467546742553890454890356"),
        big_int("42389428935349086840957804985309763636567574564")};
    const fraction b(big_int("-54357893745346457544353"), big_int(mm));
    const fraction c = a / b;
    logger->debug(a.to_string() + " / " + b.to_string() + " = " + c.to_string());
    EXPECT_TRUE(c.to_string() ==
                "-18973145506838846536619315686583140493140613455968830196612814270731663162341498039008920883721710395"
                "643745/192016672832801689536585760436969883753775374717569729934765722053091");
}

TEST(trigonometricTests, cos)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    // const fraction a{big_int("1"), big_int("2")};
    // const auto c = a.cos();
    // EXPECT_TRUE(c.to_string() == "40439/46080");
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("1"), big_int("2")};
    const auto c = a.cos(epsilon);
    const auto expected = fraction(877583_bi, 1000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, arccos)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("2"), big_int("7")};
    const auto c = a.arccos(epsilon);
    const auto expected = fraction(1281044_bi, 1000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, sin)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("1"), big_int("2")};
    const auto c = a.sin(epsilon);
    const auto expected = fraction(479426_bi, 1000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, arcsin)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("12724951"), big_int("26542080")};
    const auto c = a.arcsin(epsilon);
    const auto expected = fraction(1_bi, 2_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;

    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, tg)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("0"), big_int("1")};
    const auto c = a.tg();
    EXPECT_TRUE(c.to_string() == "0/1");
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, arctg)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("1"), big_int("2")};
    const auto c = a.arctg(epsilon);
    const auto expected = fraction(4636476_bi, 10000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, ctg)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("1"), big_int("2")};
    const auto c = a.ctg(epsilon);
    const auto expected = fraction(18304877_bi, 10000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, arcctg)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const auto epsilon = fraction(1_bi, 1000000_bi);
    const fraction a{big_int("1"), big_int("2")};
    const auto c = a.arcctg(epsilon);
    // 1.10714872
    const auto expected = fraction(11071487_bi, 10000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, sec)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("1"), big_int("2")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.sec(epsilon);
    const auto expected = fraction(11394939_bi, 10000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, arcsec)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });
    // 1.04719755
    const fraction a{big_int("2"), big_int("1")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.arcsec(epsilon);
    const auto expected = fraction(1047197_bi, 1000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, cosec)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("1"), big_int("2")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.cosec(epsilon);
    // 2.0858296
    const auto expected = fraction(20858296_bi, 10000000_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(trigonometricTests, arccosec)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("2"), big_int("1")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto d = a.arccosec(epsilon);

    const auto expected = fraction(523598_bi, 1000000_bi);
    const auto diff = abs(d - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << d << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + d.to_string());
}

TEST(powTest, pow)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("7"), big_int("27")};
    const auto c = a.pow(5);
    EXPECT_TRUE(c.to_string() == "16807/14348907");
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(rootTests, root)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("4"), big_int("9")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.root(2, epsilon);
    const auto expected = fraction(2_bi, 3_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(logariphmTests, log2)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("1"), big_int("2")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.log2(epsilon);
    const auto expected = fraction(big_int(-1), 1_bi);
    const auto diff = abs(c - expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(logariphmTests, ln)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("1"), big_int("2")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.ln(epsilon);
    const auto expected = fraction(big_int(-6931472), big_int(10000000));
    const auto diff = abs(c) - abs(expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

TEST(logariphmTests, log10)
{
    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });

    const fraction a{big_int("1"), big_int("2")};
    const auto epsilon = fraction(1_bi, 1000000_bi);
    const auto c = a.lg(epsilon);
    const auto expected = fraction(big_int(-301029995), big_int(1000000000));
    const auto diff = abs(c) - abs(expected);

    EXPECT_TRUE(diff <= epsilon) << "Ожидалось: " << expected << "\nПолучено: " << c << "\nРазница: " << diff
                                 << "\nДопустимая ошибка: " << epsilon;
    logger->debug(a.to_string() + "  " + " = " + c.to_string());
}

auto main(int argc, char **argv) -> int
{
    testing::InitGoogleTest(&argc, argv);

    logger *logger = create_logger(std::vector<std::pair<std::string, logger::severity>>{
        {"bigint_logs.txt", logger::severity::information},
    });


    switch (RUN_ALL_TESTS())
    {
    case 0: {
        logger->information("Все тесты пройдены успешно :)) ");
        return 0;
    }
    case 1: {
        logger->information("Где-то что-то не то :(( Попробуй еще раз ");
        return 1;
    }
    default: {
        logger->information("Кажется тесты не запустились :(( Попробуй еще раз ");}
    }

    return 0;
}