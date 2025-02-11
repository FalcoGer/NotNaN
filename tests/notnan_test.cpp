#include "../NotNaN.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <concepts>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(implicit-float-conversion)
// NOLINTBEGIN(readability-identifier-naming)

// disable warnings regarding conversions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"

TEMPLATE_TEST_CASE("Type deduction", "[NotNaN]", float, double, long double)
{
    const NotNaN x {TestType {}};
    REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
}

TEMPLATE_TEST_CASE("Assignment", "[NotNaN]", float, double, long double)
{
    auto       rv = GENERATE(Catch::Generators::take(25, Catch::Generators::random(-100, 100)));
    const auto tv = static_cast<TestType>(rv);

    SECTION("Copy constructor")
    {
        SECTION("With arithmetic values")
        {
            NotNaN x {tv};
            REQUIRE(x == tv);
            NotNaN<float> y {tv};
            REQUIRE(y == static_cast<TestType>(tv));
        }
        SECTION("With NotNaN values")
        {
            NotNaN x {tv};
            REQUIRE(x == tv);
            NotNaN y {x};
            REQUIRE(y == tv);
            REQUIRE(std::same_as<typename decltype(y)::Type, typename decltype(x)::Type>);
            NotNaN<float> z {x};    // not same type by design
            REQUIRE(z == static_cast<float>(tv));
        }
    }
    SECTION("Move constructor")
    {
        SECTION("With arithmetic values")
        {
            NotNaN x {std::move(tv)};
            REQUIRE(x == tv);
            REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
            NotNaN<float> y {std::move(tv)};    // not same type by design
            REQUIRE(y == static_cast<float>(tv));
        }
        SECTION("With NotNaN values")
        {
            NotNaN x {tv};
            REQUIRE(x == tv);
            NotNaN y {std::move(x)};
            REQUIRE(y == tv);
            REQUIRE(std::same_as<typename decltype(y)::Type, typename decltype(x)::Type>);
            NotNaN<float> z {std::move(x)};    // not same type by design
            REQUIRE(z == static_cast<float>(tv));
        }
    }
    SECTION("Copy assignment")
    {
        SECTION("With arithmetic values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE(x == static_cast<TestType>(0.0));
            x = tv;
            REQUIRE(x == tv);
            NotNaN<float> y {static_cast<float>(0.0)};
            REQUIRE(y == 0.F);
            y = tv;
            REQUIRE(y == static_cast<float>(tv));
        }
        SECTION("With NotNaN values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE(x == static_cast<TestType>(0.0));
            const NotNaN y {tv};
            x = y;
            REQUIRE(x == tv);
            REQUIRE(std::same_as<typename decltype(x)::Type, typename decltype(y)::Type>);
        }
        SECTION("With Self")
        {
            NotNaN x {tv};
            REQUIRE(x == tv);
            x = x;
            REQUIRE(x == tv);
        }
    }
    SECTION("Move assignment")
    {
        SECTION("With arithmetic values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE(x == static_cast<TestType>(0.0));
            x = std::move(tv);
            REQUIRE(x == tv);
            NotNaN<float> y {static_cast<float>(0.0)};
            REQUIRE(y == static_cast<float>(0.0));
            y = std::move(tv);
            REQUIRE(y == static_cast<float>(tv));
        }
        SECTION("With NotNaN values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE(x == static_cast<TestType>(0.0));
            const NotNaN y {tv};
            x = std::move(y);
            REQUIRE(x == tv);
            REQUIRE(std::same_as<typename decltype(x)::Type, typename decltype(y)::Type>);
        }
        SECTION("With Self")
        {
            NotNaN x {tv};
            REQUIRE(x == tv);
            x = std::move(x);
            REQUIRE(x == tv);
        }
    }
}

TEMPLATE_TEST_CASE("Conversions", "[NotNaN]", float, double, long double)
{
    auto         gv = GENERATE(-4, 0, 1, 62);
    const auto   tv = static_cast<TestType>(gv);

    const NotNaN x {tv};
    SECTION("To bool")
    {
        REQUIRE(static_cast<bool>(tv) == static_cast<bool>(x));
    }
    SECTION("To int")
    {
        REQUIRE(static_cast<int>(tv) == static_cast<int>(x));
    }
    SECTION("To float")
    {
        REQUIRE(static_cast<float>(tv) == static_cast<float>(x));
    }
    SECTION("To double")
    {
        REQUIRE(static_cast<double>(tv) == static_cast<double>(x));
    }
    SECTION("To long double")
    {
        REQUIRE(static_cast<long double>(tv) == static_cast<long double>(x));
    }
}

TEMPLATE_TEST_CASE("Comparison", "[NotNaN]", float, double, long double)
{
    auto         gv  = GENERATE(-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5);
    auto         gv2 = GENERATE(-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5);
    const auto   tv  = static_cast<TestType>(gv);

    const NotNaN x {tv};
    REQUIRE(x == tv);

    SECTION("NotNaN lhs")
    {
        SECTION("With int")
        {
            auto cv = static_cast<int>(gv2);
            REQUIRE((x == cv) == (tv == cv));
            REQUIRE((x != cv) == (tv != cv));
            REQUIRE((x < cv) == (tv < cv));
            REQUIRE((x > cv) == (tv > cv));
            REQUIRE((x <= cv) == (tv <= cv));
            REQUIRE((x >= cv) == (tv >= cv));
        }
        SECTION("With float")
        {
            auto cv = static_cast<float>(gv2);
            REQUIRE((x == cv) == (tv == cv));
            REQUIRE((x != cv) == (tv != cv));
            REQUIRE((x < cv) == (tv < cv));
            REQUIRE((x > cv) == (tv > cv));
            REQUIRE((x <= cv) == (tv <= cv));
            REQUIRE((x >= cv) == (tv >= cv));
        }
        SECTION("With double")
        {
            auto cv = static_cast<double>(gv2);
            REQUIRE((x == cv) == (tv == cv));
            REQUIRE((x != cv) == (tv != cv));
            REQUIRE((x < cv) == (tv < cv));
            REQUIRE((x > cv) == (tv > cv));
            REQUIRE((x <= cv) == (tv <= cv));
            REQUIRE((x >= cv) == (tv >= cv));
        }
        SECTION("With long double")
        {
            auto cv = static_cast<long double>(gv2);
            REQUIRE((x == cv) == (tv == cv));
            REQUIRE((x != cv) == (tv != cv));
            REQUIRE((x < cv) == (tv < cv));
            REQUIRE((x > cv) == (tv > cv));
            REQUIRE((x <= cv) == (tv <= cv));
            REQUIRE((x >= cv) == (tv >= cv));
        }
    }
    SECTION("NotNaN rhs")
    {
        SECTION("With int")
        {
            auto cv = static_cast<int>(gv2);
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
        SECTION("With float")
        {
            auto cv = static_cast<float>(gv2);
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
        SECTION("With double")
        {
            auto cv = static_cast<double>(gv2);
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
        SECTION("With long double")
        {
            auto cv = static_cast<long double>(gv2);
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
    }
    SECTION("With NotNaN Types lhs and rhs")
    {
        SECTION("With float")
        {
            NotNaN<float> cv {static_cast<float>(gv2)};
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
        SECTION("With double")
        {
            NotNaN<double> cv {static_cast<double>(gv2)};
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
        SECTION("With long double")
        {
            NotNaN<long double> cv {static_cast<long double>(gv2)};
            REQUIRE((cv == x) == (cv == tv));
            REQUIRE((cv != x) == (cv != tv));
            REQUIRE((cv < x) == (cv < tv));
            REQUIRE((cv > x) == (cv > tv));
            REQUIRE((cv <= x) == (cv <= tv));
            REQUIRE((cv >= x) == (cv >= tv));
        }
    }
}

TEMPLATE_TEST_CASE("NotNaN Exceptions", "[NotNaN]", float, double, long double)
{
    SECTION("Constructing with NaN")
    {
        REQUIRE_THROWS(NotNaN {std::numeric_limits<TestType>::quiet_NaN()});
    }
    SECTION("Assigning NaN")
    {
        NotNaN x {static_cast<TestType>(0.0)};
        REQUIRE(x == static_cast<TestType>(0.0));
        REQUIRE_THROWS(x = std::numeric_limits<TestType>::quiet_NaN());
    }
    SECTION("Calculating NaN")
    {
        SECTION("0 / 0")
        {
            // Two NotNaN
            REQUIRE_THROWS(NotNaN<TestType> {0.0} / NotNaN<TestType> {0.0});
            // One NotNaN lhs
            REQUIRE_THROWS(NotNaN<TestType> {0.0} / 0.0F);
            REQUIRE_THROWS(NotNaN<TestType> {0.0} / 0.0);
            REQUIRE_THROWS(NotNaN<TestType> {0.0} / 0.0L);
            // One NotNaN rhs
            REQUIRE_THROWS(0.0F / NotNaN<TestType> {0.0});
            REQUIRE_THROWS(0.0 / NotNaN<TestType> {0.0});
            REQUIRE_THROWS(0.0L / NotNaN<TestType> {0.0});
        }
        SECTION("Inf + (-Inf)")
        {
            // Two NotNaN
            REQUIRE_THROWS(
              NotNaN<TestType> {std::numeric_limits<TestType>::infinity()}
              + NotNaN<TestType> {-std::numeric_limits<TestType>::infinity()}
            );
            // One NotNaN lhs
            REQUIRE_THROWS(
              NotNaN<TestType> {std::numeric_limits<TestType>::infinity()} + -std::numeric_limits<TestType>::infinity()
            );
            // One NotNaN rhs
            REQUIRE_THROWS(
              std::numeric_limits<TestType>::infinity() + NotNaN<TestType> {-std::numeric_limits<TestType>::infinity()}
            );
        }
        SECTION("Inf - Inf")
        {
            // Two NotNaN
            REQUIRE_THROWS(
              NotNaN<TestType> {std::numeric_limits<TestType>::infinity()}
              - NotNaN<TestType> {std::numeric_limits<TestType>::infinity()}
            );
            // One NotNaN lhs
            REQUIRE_THROWS(
              NotNaN<TestType> {std::numeric_limits<TestType>::infinity()} - std::numeric_limits<TestType>::infinity()
            );
            // One NotNaN rhs
            REQUIRE_THROWS(
              std::numeric_limits<TestType>::infinity() - NotNaN<TestType> {std::numeric_limits<TestType>::infinity()}
            );
        }
        SECTION("Inf * 0.0")
        {
            // Two NotNaN
            REQUIRE_THROWS(NotNaN<TestType> {std::numeric_limits<TestType>::infinity()} * NotNaN<TestType> {0.0});
            REQUIRE_THROWS(NotNaN<TestType> {0.0} * NotNaN<TestType> {std::numeric_limits<TestType>::infinity()});
            // One NotNaN lhs
            REQUIRE_THROWS(NotNaN<TestType> {std::numeric_limits<TestType>::infinity()} * 0.F);
            REQUIRE_THROWS(NotNaN<TestType> {std::numeric_limits<TestType>::infinity()} * 0.0);
            REQUIRE_THROWS(NotNaN<TestType> {std::numeric_limits<TestType>::infinity()} * 0.L);
            // One NotNaN rhs
            REQUIRE_THROWS(0.F * NotNaN<TestType> {std::numeric_limits<TestType>::infinity()});
            REQUIRE_THROWS(0.0 * NotNaN<TestType> {std::numeric_limits<TestType>::infinity()});
            REQUIRE_THROWS(0.L * NotNaN<TestType> {std::numeric_limits<TestType>::infinity()});
        }
        SECTION("All Ops with NaN rhs")
        {
            REQUIRE_THROWS(NotNaN<TestType> {0.0} + std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(NotNaN<TestType> {0.0} - std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(NotNaN<TestType> {0.0} * std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(NotNaN<TestType> {0.0} / std::numeric_limits<TestType>::quiet_NaN());
        }
        SECTION("All ops with NaN lhs")
        {
            REQUIRE_THROWS(std::numeric_limits<TestType>::quiet_NaN() + NotNaN<TestType> {0.0});
            REQUIRE_THROWS(std::numeric_limits<TestType>::quiet_NaN() - NotNaN<TestType> {0.0});
            REQUIRE_THROWS(std::numeric_limits<TestType>::quiet_NaN() * NotNaN<TestType> {0.0});
            REQUIRE_THROWS(std::numeric_limits<TestType>::quiet_NaN() / NotNaN<TestType> {0.0});
        }
        SECTION("All assignment operations with NaN")
        {
            NotNaN<TestType> x {0.0};
            REQUIRE(x == static_cast<TestType>(0.0));
            REQUIRE_THROWS(x = std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x += std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x -= std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x *= std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x /= std::numeric_limits<TestType>::quiet_NaN());
        }
    }
}

TEMPLATE_TEST_CASE("Unary Ops", "[NotNaN]", float, double, long double)
{
    auto       rv = GENERATE(Catch::Generators::take(100, Catch::Generators::random(-100, 100)));
    const auto tv = static_cast<TestType>(rv);
    NotNaN     x {tv};
    REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
    REQUIRE(static_cast<TestType>(-x) == -tv);
    REQUIRE(static_cast<TestType>(+x) == +tv);
    REQUIRE(static_cast<TestType>(!x) == !tv);
}

TEMPLATE_TEST_CASE("Binary Ops", "[NotNaN]", float, double, long double)
{
    auto         gv  = GENERATE(Catch::Generators::take(15, Catch::Generators::random(-100, 100)));
    auto         gv2 = GENERATE(Catch::Generators::take(15, Catch::Generators::random(-100, 100)));
    const auto   tv  = static_cast<TestType>(gv);
    const NotNaN x {tv};

    SECTION("Addition")
    {
        SECTION("With int")
        {
            auto operand = static_cast<int>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x + operand), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));

            NotNaN c {x};
            auto val = (c += operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
        }
        SECTION("With float")
        {
            auto operand = static_cast<float>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x + operand), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x + operand2), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));

            NotNaN c {x};
            auto val = (c += operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 += operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
        }
        SECTION("With double")
        {
            auto operand = static_cast<double>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x + operand), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x + operand2), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));

            NotNaN c {x};
            auto val = (c += operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 += operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
        }
        SECTION("With long double")
        {
            auto operand = static_cast<long double>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x + operand), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x + operand2), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 + x), Catch::Matchers::WithinAbs(operand + tv, 0.0001));

            NotNaN c {x};
            auto val = (c += operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 += operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv + operand, 0.0001));
        }
    }
    SECTION("Subtraction")
    {
        SECTION("With int")
        {
            auto operand = static_cast<int>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x - operand), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));

            NotNaN c {x};
            auto val = (c -= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
        }
        SECTION("With float")
        {
            auto operand = static_cast<float>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x - operand), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x - operand2), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));

            NotNaN c {x};
            auto val = (c -= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 -= operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
        }
        SECTION("With double")
        {
            auto operand = static_cast<double>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x - operand), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x - operand2), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));

            NotNaN c {x};
            auto val = (c -= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 -= operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
        }
        SECTION("With long double")
        {
            auto operand = static_cast<long double>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x - operand), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x - operand2), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 - x), Catch::Matchers::WithinAbs(operand - tv, 0.0001));

            NotNaN c {x};
            auto val = (c -= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 -= operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv - operand, 0.0001));
        }
    }
    SECTION("Multiplication")
    {
        SECTION("With int")
        {
            auto operand = static_cast<int>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x * operand), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));

            NotNaN c {x};
            auto val = (c *= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
        }
        SECTION("With float")
        {
            auto operand = static_cast<float>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x * operand), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x * operand2), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));

            NotNaN c {x};
            auto val = (c *= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 *= operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
        }
        SECTION("With double")
        {
            auto operand = static_cast<double>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x * operand), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x * operand2), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));

            NotNaN c {x};
            auto val = (c *= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 *= operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
        }
        SECTION("With long double")
        {
            auto operand = static_cast<long double>(gv2);
            REQUIRE_THAT(static_cast<TestType>(x * operand), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(static_cast<TestType>(x * operand2), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            REQUIRE_THAT(static_cast<TestType>(operand2 * x), Catch::Matchers::WithinAbs(operand * tv, 0.0001));

            NotNaN c {x};
            auto val = (c *= operand);
            REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
            REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
            NotNaN c2 {x};
            auto   val2 = (c2 *= operand2);
            REQUIRE(std::same_as<decltype(c2), std::remove_reference_t<decltype(val2)>>);
            REQUIRE_THAT(static_cast<TestType>(c2), Catch::Matchers::WithinAbs(tv * operand, 0.0001));
        }
    }
    SECTION("Division")
    {
        SECTION("With int")
        {
            auto operand = static_cast<int>(gv2);
            if (operand == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand);
                REQUIRE_THROWS(operand / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand));
            }
            else
            {
                if (operand != 0)
                {
                    REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                }
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
        }
        SECTION("With float")
        {
            auto operand = static_cast<float>(gv2);
            if (operand == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand);
                REQUIRE_THROWS(operand / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand));
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
            auto operand2 = NotNaN {operand};
            REQUIRE(operand2 == operand);
            if (operand2 == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand2);
                REQUIRE_THROWS(operand2 / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand2));
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand2), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand2 / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand2);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
        }
        SECTION("With double")
        {
            auto operand = static_cast<double>(gv2);
            if (operand == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand);
                REQUIRE_THROWS(operand / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand));
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
            auto operand2 = NotNaN {operand};
            REQUIRE(operand2 == operand);
            if (operand2 == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand2);
                REQUIRE_THROWS(operand2 / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand2));
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand2), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand2 / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand2);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
        }
        SECTION("With long double")
        {
            auto operand = static_cast<long double>(gv2);
            if (operand == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand);
                REQUIRE_THROWS(operand / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand));
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
            auto operand2 = NotNaN {operand};
            REQUIRE(operand2 == operand);
            if (operand2 == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand2);
                REQUIRE_THROWS(operand2 / x);
                NotNaN c {x};
                REQUIRE_THROWS((c /= operand2));
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand2), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand2 / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto val = (c /= operand2);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
        }
    }
}


// NOLINTEND(readability-identifier-naming)
// NOLINTEND(implicit-float-conversion)
// NOLINTEND(readability-magic-numbers)

#pragma GCC diagnostic pop
