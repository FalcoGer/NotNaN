#include "../NotNaN.hpp"
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <cmath>
#include <concepts>
#include <limits>
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
    auto       rv = GENERATE(Catch::Generators::take(25, Catch::Generators::random(-10.0, 10.0)));
    const auto tv = static_cast<TestType>(rv);

    SECTION("Copy constructor")
    {
        SECTION("With arithmetic values")
        {
            NotNaN x {tv};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            NotNaN<float> y {tv};
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(static_cast<TestType>(tv), 0.0001));
        }
        SECTION("With NotNaN values")
        {
            NotNaN x {tv};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            NotNaN y {x};
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(tv, 0.0001));
            REQUIRE(std::same_as<typename decltype(y)::Type, typename decltype(x)::Type>);
            NotNaN<float> z {x};    // not same type by design
            REQUIRE_THAT(*z, Catch::Matchers::WithinAbs(static_cast<float>(tv), 0.0001));
        }
    }
    SECTION("Move constructor")
    {
        SECTION("With arithmetic values")
        {
            NotNaN x {std::move(tv)};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
            NotNaN<float> y {std::move(tv)};    // not same type by design
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(static_cast<float>(tv), 0.0001));
        }
        SECTION("With NotNaN values")
        {
            NotNaN x {tv};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            NotNaN y {std::move(x)};
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(tv, 0.0001));
            REQUIRE(std::same_as<typename decltype(y)::Type, typename decltype(x)::Type>);
            NotNaN<float> z {std::move(x)};    // not same type by design
            REQUIRE_THAT(*z, Catch::Matchers::WithinAbs(static_cast<float>(tv), 0.0001));
        }
    }
    SECTION("Copy assignment")
    {
        SECTION("With arithmetic values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(static_cast<TestType>(0.0), 0.0001));
            x = tv;
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            NotNaN<float> y {static_cast<float>(0.0)};
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(0.F, 0.0001));
            y = tv;
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(static_cast<float>(tv), 0.0001));
        }
        SECTION("With NotNaN values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(static_cast<TestType>(0.0), 0.0001));
            const NotNaN y {tv};
            x = y;
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            REQUIRE(std::same_as<typename decltype(x)::Type, typename decltype(y)::Type>);
        }
        SECTION("With Self")
        {
            NotNaN x {tv};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            x = x;
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
        }
    }
    SECTION("Move assignment")
    {
        SECTION("With arithmetic values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(static_cast<TestType>(0.0), 0.0001));
            x = std::move(tv);
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            NotNaN<float> y {static_cast<float>(0.0)};
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(static_cast<float>(0.0), 0.0001));
            y = std::move(tv);
            REQUIRE_THAT(*y, Catch::Matchers::WithinAbs(static_cast<float>(tv), 0.0001));
        }
        SECTION("With NotNaN values")
        {
            NotNaN<TestType> x {static_cast<TestType>(0.0)};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(static_cast<TestType>(0.0), 0.0001));
            const NotNaN y {tv};
            x = std::move(y);
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            REQUIRE(std::same_as<typename decltype(x)::Type, typename decltype(y)::Type>);
        }
        SECTION("With Self")
        {
            NotNaN x {tv};
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
            x = std::move(x);
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));
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
        REQUIRE_THAT(static_cast<float>(tv), Catch::Matchers::WithinAbs(static_cast<float>(x), 0.0001));
    }
    SECTION("To double")
    {
        REQUIRE_THAT(static_cast<double>(tv), Catch::Matchers::WithinAbs(static_cast<double>(x), 0.0001));
    }
    SECTION("To long double")
    {
        REQUIRE_THAT(static_cast<long double>(tv), Catch::Matchers::WithinAbs(static_cast<long double>(x), 0.0001));
    }
}

TEMPLATE_TEST_CASE("Comparison", "[NotNaN]", float, double, long double)
{
    auto         gv  = GENERATE(-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5);
    auto         gv2 = GENERATE(-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5);
    const auto   tv  = static_cast<TestType>(gv);

    const NotNaN x {tv};
    REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(tv, 0.0001));

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
        REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(static_cast<TestType>(0.0), 0.0001));
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
            REQUIRE_THAT(*x, Catch::Matchers::WithinAbs(static_cast<TestType>(0.0), 0.0001));
            REQUIRE_THROWS(x = std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x += std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x -= std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x *= std::numeric_limits<TestType>::quiet_NaN());
            REQUIRE_THROWS(x /= std::numeric_limits<TestType>::quiet_NaN());
        }
        SECTION("Special functions")
        {
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.sqrt());
            REQUIRE_THROWS(NotNaN<TestType> {10.0}.pow(std::numeric_limits<TestType>::quiet_NaN()));
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.pow(0.5));
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.pow(NotNaN {0.5}));
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.log());
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.log2());
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.log10());
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.logBase(2.0));
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.logBase(NotNaN<TestType> {2.0}));
            REQUIRE_THROWS(NotNaN<TestType> {5.0}.asin());
            REQUIRE_THROWS(NotNaN<TestType> {5.0}.acos());
            REQUIRE_THROWS(NotNaN<TestType> {-1.0}.tgamma());
        }
    }
}
TEMPLATE_TEST_CASE("Special Functions", "[NotNaN]", float, double, long double)
{
    auto       rv = GENERATE(Catch::Generators::take(100, Catch::Generators::random(-10.0, 10.0)));
    const auto tv = static_cast<TestType>(rv);
    NotNaN     x {tv};

    SECTION("Powers")
    {
        // sqrt
        if (std::isnan(std::sqrt(tv)))
        {
            REQUIRE_THROWS(x.sqrt());
        }
        else
        {
            REQUIRE_THAT(*x.sqrt(), Catch::Matchers::WithinAbs(std::sqrt(tv), 0.0001));
        }

        // cbrt
        if (std::isnan(std::cbrt(tv)))
        {
            REQUIRE_THROWS(x.cbrt());
        }
        else
        {
            REQUIRE_THAT(*x.cbrt(), Catch::Matchers::WithinAbs(std::cbrt(tv), 0.0001));
        }

        SECTION("POW")
        {
            TestType exp = GENERATE(Catch::Generators::take(10, Catch::Generators::random(-5, 5)));
            if (std::isnan(std::pow(tv, exp)))
            {
                REQUIRE_THROWS(x.pow(exp));
                REQUIRE_THROWS(x.pow(NotNaN{exp}));
            }
            else
            {
                REQUIRE_THAT(*x.pow(exp), Catch::Matchers::WithinAbs(std::pow(tv, exp), 0.0001));
                REQUIRE_THAT(*x.pow(NotNaN{exp}), Catch::Matchers::WithinAbs(std::pow(tv, exp), 0.0001));
            }
        }

        // exp
        if (std::isnan(std::exp(tv)))
        {
            REQUIRE_THROWS(x.exp());
        }
        else
        {
            REQUIRE_THAT(*x.exp(), Catch::Matchers::WithinAbs(std::exp(tv), 0.0001));
        }

        // exp2
        if (std::isnan(std::exp2(tv)))
        {
            REQUIRE_THROWS(x.exp2());
        }
        else
        {
            REQUIRE_THAT(*x.exp2(), Catch::Matchers::WithinAbs(std::exp2(tv), 0.0001));
        }

        // expm1
        if (std::isnan(std::expm1(tv)))
        {
            REQUIRE_THROWS(x.expm1());
        }
        else
        {
            REQUIRE_THAT(*x.expm1(), Catch::Matchers::WithinAbs(std::expm1(tv), 0.0001));
        }
    }

    SECTION("Logarithms")
    {
        // log
        if (std::isnan(std::log(tv)))
        {
            REQUIRE_THROWS(x.log());
        }
        else
        {
            REQUIRE_THAT(*x.log(), Catch::Matchers::WithinAbs(std::log(tv), 0.0001));
        }

        // log2
        if (std::isnan(std::log2(tv)))
        {
            REQUIRE_THROWS(x.log2());
        }
        else
        {
            REQUIRE_THAT(*x.log2(), Catch::Matchers::WithinAbs(std::log2(tv), 0.0001));
        }

        // log10
        if (std::isnan(std::log10(tv)))
        {
            REQUIRE_THROWS(x.log10());
        }
        else
        {
            REQUIRE_THAT(*x.log10(), Catch::Matchers::WithinAbs(std::log10(tv), 0.0001));
        }
        // log1p
        if (std::isnan(std::log1pf(tv)))
        {
            REQUIRE_THROWS(x.log1p());
        }
        else
        {
            REQUIRE_THAT(*x.log1p(), Catch::Matchers::WithinAbs(std::log1p(tv), 0.0001));
        }

        // logBase (logarithm with a random base)
        SECTION("LogBase")
        {
            TestType base = GENERATE(Catch::Generators::take(10, Catch::Generators::random(-10.0, 10.0)));
            if (std::isnan(std::log(tv) / std::log(base)))
            {
                REQUIRE_THROWS(x.logBase(base));
                REQUIRE_THROWS(x.logBase(NotNaN{base}));
            }
            else
            {
                REQUIRE_THAT(*x.logBase(base), Catch::Matchers::WithinAbs(std::log(tv) / std::log(base), 0.0001));
                REQUIRE_THAT(*x.logBase(NotNaN{base}), Catch::Matchers::WithinAbs(std::log(tv) / std::log(base), 0.0001));
            }
        }
    }

    SECTION("Trigonometry")
    {
        // sin
        if (std::isnan(std::sin(tv)))
        {
            REQUIRE_THROWS(x.sin());
        }
        else
        {
            REQUIRE_THAT(*x.sin(), Catch::Matchers::WithinAbs(std::sin(tv), 0.0001));
        }
        // cos
        if (std::isnan(std::cos(tv)))
        {
            REQUIRE_THAT(*x.cos(), Catch::Matchers::WithinAbs(std::cos(tv), 0.0001));
        }
        // tan
        if (std::isnan(std::tan(tv)))
        {
            REQUIRE_THROWS(x.tan());
        }
        else
        {
            REQUIRE_THAT(*x.tan(), Catch::Matchers::WithinAbs(std::tan(tv), 0.0001));
        }

        // asin
        if (std::isnan(std::asin(tv)))
        {
            REQUIRE_THROWS(x.asin());
        }
        else
        {
            REQUIRE_THAT(*x.asin(), Catch::Matchers::WithinAbs(std::asin(tv), 0.0001));
        }

        // acos
        if (std::isnan(std::acos(tv)))
        {
            REQUIRE_THROWS(x.acos());
        }
        else
        {
            REQUIRE_THAT(*x.acos(), Catch::Matchers::WithinAbs(std::acos(tv), 0.0001));
        }

        // atan
        if (std::isnan(std::atan(tv)))
        {
            REQUIRE_THROWS(x.atan());
        }
        else
        {
            REQUIRE_THAT(*x.atan(), Catch::Matchers::WithinAbs(std::atan(tv), 0.0001));
        }

        SECTION("atan2")
        {
            TestType y = GENERATE(Catch::Generators::take(10, Catch::Generators::random(-10.0, 10.0)));
            if (std::isnan(std::atan2(tv, y)))
            {
                REQUIRE_THROWS(x.atan2(y));
                REQUIRE_THROWS(x.atan2(NotNaN{y}));
            }
            else
            {
                REQUIRE_THAT(*x.atan2(y), Catch::Matchers::WithinAbs(std::atan2(tv, y), 0.0001));
                REQUIRE_THAT(*x.atan2(NotNaN(y)), Catch::Matchers::WithinAbs(std::atan2(tv, y), 0.0001));
            }
        }
        SECTION("hypot")
        {
            TestType y = GENERATE(Catch::Generators::take(10, Catch::Generators::random(-10.0, 10.0)));
            if (std::isnan(std::hypot(tv, y)))
            {
                REQUIRE_THROWS(x.hypot(y));
                REQUIRE_THROWS(x.hypot(NotNaN{y}));
            }
            else
            {
                REQUIRE_THAT(*x.hypot(y), Catch::Matchers::WithinAbs(std::hypot(tv, y), 0.0001));
                REQUIRE_THAT(*x.hypot(NotNaN(y)), Catch::Matchers::WithinAbs(std::hypot(tv, y), 0.0001));
            }
        }
    }

    SECTION("Rounding")
    {
        // floor
        REQUIRE_THAT(*x.floor(), Catch::Matchers::WithinAbs(std::floor(tv), 0.0001));

        // ceil
        REQUIRE_THAT(*x.ceil(), Catch::Matchers::WithinAbs(std::ceil(tv), 0.0001));

        // round
        REQUIRE_THAT(*x.round(), Catch::Matchers::WithinAbs(std::round(tv), 0.0001));
    }

    SECTION("Functions")
    {
        // erf
        if (std::isnan(std::erf(tv)))
        {
            REQUIRE_THROWS(x.erf());
        }
        else
        {
            REQUIRE_THAT(*x.erf(), Catch::Matchers::WithinAbs(std::erf(tv), 0.0001));
        }

        // erfc
        if (std::isnan(std::erfc(tv)))
        {
            REQUIRE_THROWS(x.erfc());
        }
        else
        {
            REQUIRE_THAT(*x.erfc(), Catch::Matchers::WithinAbs(std::erfc(tv), 0.0001));
        }

        // tgamma
        if (std::isnan(std::tgamma(tv)))
        {
            REQUIRE_THROWS(x.tgamma());
        }
        else
        {
            REQUIRE_THAT(*x.tgamma(), Catch::Matchers::WithinAbs(std::tgamma(tv), 0.0001));
        }

        // lgamma
        if (std::isnan(std::lgamma(tv)))
        {
            REQUIRE_THROWS(x.lgamma());
        }
        else
        {
            REQUIRE_THAT(*x.lgamma(), Catch::Matchers::WithinAbs(std::lgamma(tv), 0.0001));
        }
    }

    SECTION("Misc")
    {
        // abs
        REQUIRE_THAT(*x.abs(), Catch::Matchers::WithinAbs(std::abs(tv), 0.0001));

        SECTION("MODPOINT")
        {
            TestType y = GENERATE(Catch::Generators::take(10, Catch::Generators::random(-100.0, 100.0)));
            NotNaN other {static_cast<TestType>(y)};
            REQUIRE_THAT(*x.midpoint(other), Catch::Matchers::WithinAbs(std::midpoint(tv, y), 0.0001));
        }

        // modf
        NotNaN<TestType> integralPart{0.0};
        NotNaN<TestType> fractionalPart = x.modf(&integralPart);

        TestType         integralPartCheck;
        TestType fractionalPartCheck = std::modf(tv, &integralPartCheck);

        REQUIRE_THAT(*integralPart, Catch::Matchers::WithinAbs(integralPartCheck, 0.0001));
        REQUIRE_THAT(*fractionalPart, Catch::Matchers::WithinAbs(fractionalPartCheck, 0.0001));
    }
}

TEMPLATE_TEST_CASE("Unary Ops", "[NotNaN]", float, double, long double)
{
    auto       rv = GENERATE(Catch::Generators::take(100, Catch::Generators::random(-100, 100)));
    const auto tv = static_cast<TestType>(rv);
    NotNaN     x {tv};
    REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
    REQUIRE_THAT(static_cast<TestType>(-x), Catch::Matchers::WithinAbs(-tv, 0.0001));
    REQUIRE_THAT(static_cast<TestType>(+x), Catch::Matchers::WithinAbs(+tv, 0.0001));
    REQUIRE_THAT(static_cast<TestType>(!x), Catch::Matchers::WithinAbs(!tv, 0.0001));
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
            auto   val = (c += operand);
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
            auto   val = (c += operand);
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
            auto   val = (c += operand);
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
            auto   val = (c += operand);
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
            auto   val = (c -= operand);
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
            auto   val = (c -= operand);
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
            auto   val = (c -= operand);
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
            auto   val = (c -= operand);
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
            auto   val = (c *= operand);
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
            auto   val = (c *= operand);
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
            auto   val = (c *= operand);
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
            auto   val = (c *= operand);
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
                REQUIRE_THROWS(c /= operand);
            }
            else
            {
                if (operand != 0)
                {
                    REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                }
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand);
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
                REQUIRE_THROWS(c /= operand);
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(*operand2, Catch::Matchers::WithinAbs(operand, 0.0001));
            if (operand2 == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand2);
                REQUIRE_THROWS(operand2 / x);
                NotNaN c {x};
                REQUIRE_THROWS(c /= operand2);
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand2), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand2 / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand2);
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
                REQUIRE_THROWS(c /= operand);
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(*operand2, Catch::Matchers::WithinAbs(operand, 0.0001));
            if (operand2 == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand2);
                REQUIRE_THROWS(operand2 / x);
                NotNaN c {x};
                REQUIRE_THROWS(c /= operand2);
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand2), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand2 / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand2);
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
                REQUIRE_THROWS(c /= operand);
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand);
                REQUIRE(std::same_as<decltype(c), std::remove_reference_t<decltype(val)>>);
                REQUIRE_THAT(static_cast<TestType>(c), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
            }
            auto operand2 = NotNaN {operand};
            REQUIRE_THAT(*operand2, Catch::Matchers::WithinAbs(operand, 0.0001));
            if (operand2 == 0 && x == 0)
            {
                REQUIRE_THROWS(x / operand2);
                REQUIRE_THROWS(operand2 / x);
                NotNaN c {x};
                REQUIRE_THROWS(c /= operand2);
            }
            else
            {
                REQUIRE_THAT(static_cast<TestType>(x / operand2), Catch::Matchers::WithinAbs(tv / operand, 0.0001));
                REQUIRE_THAT(static_cast<TestType>(operand2 / x), Catch::Matchers::WithinAbs(operand / tv, 0.0001));

                NotNaN c {x};
                auto   val = (c /= operand2);
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
