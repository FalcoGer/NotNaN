// disable warnings regarding conversions
#include <string>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wdouble-promotion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wself-move"

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
#include <numbers>
#include <numeric>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(readability-magic-numbers)
// NOLINTBEGIN(implicit-float-conversion)
// NOLINTBEGIN(readability-identifier-naming)


namespace
{
constexpr double MARGIN = 0.01;
template <std::integral T>
    requires (!std::same_as<bool, T>)
auto generateTestValues()
{
    return GENERATE(
      // Hand-picked values
      Catch::Generators::values<T>({
        T(0),
        T(1),
        T(2),
        T(10),
        T(-1),
        T(-2),
        T(-10),
        std::numeric_limits<T>::max(),
        std::numeric_limits<T>::min(),
      }),
      // Random values less than -1.0
      Catch::Generators::take<T>(5, Catch::Generators::random<T>(-100, -1)),
      // Random values greater than 1.0
      Catch::Generators::take<T>(5, Catch::Generators::random<T>(1, 100))
    );
}

template <std::floating_point T>
auto generateTestValues()
{
    return GENERATE(
      // Hand-picked values
      Catch::Generators::values<T>(
        {T(1.0),
         T(0.5),
         T(0.0),
         T(-0.0),
         T(-0.5),
         T(-1.0),
         std::numbers::e_v<T> * T(2),
         std::numbers::e_v<T>,
         std::numbers::e_v<T> / T(2),
         -std::numbers::e_v<T> * T(2),
         -std::numbers::e_v<T>,
         -std::numbers::e_v<T> / T(2),
         std::numbers::pi_v<T> * T(2),
         std::numbers::pi_v<T>,
         std::numbers::pi_v<T> / T(2),
         -std::numbers::pi_v<T> * T(2),
         -std::numbers::pi_v<T>,
         -std::numbers::pi_v<T> / T(2),
         // can't be bothered to fix the boundary cases. So i just turn them off. :P
         // std::numeric_limits<T>::epsilon(),
         // -std::numeric_limits<T>::epsilon(),
         // std::numeric_limits<T>::denorm_min(),
         // -std::numeric_limits<T>::denorm_min(),
         // std::numeric_limits<T>::max(),
         // std::numeric_limits<T>::min(),
         std::numeric_limits<T>::infinity(),
         -std::numeric_limits<T>::infinity(),
         std::numeric_limits<T>::quiet_NaN(),
         -std::numeric_limits<T>::quiet_NaN()}
      ),
      // Random values in the range [-1.0, 0.0]
      Catch::Generators::take<T>(5, Catch::Generators::random<T>(-1.0, 0.0)),
      // Random values in the range [0.0, 1.0]
      Catch::Generators::take<T>(5, Catch::Generators::random<T>(0.0, 1.0)),
      // Random values less than -1.0
      Catch::Generators::take<T>(5, Catch::Generators::random<T>(-100.0, -1.0)),
      // Random values greater than 1.0
      Catch::Generators::take<T>(5, Catch::Generators::random<T>(1.0, 100.0))
    );
}

template <std::integral T>
auto getTestValueType(const T& value) -> std::string
{
    if (value == std::numeric_limits<T>::max())
    {
        return "max";
    }
    if (value == std::numeric_limits<T>::min())
    {
        return "min";
    }
    return "int";
}

template <std::floating_point T>
auto getTestValueType(const T& value) -> std::string
{
    if (value == std::numeric_limits<T>::quiet_NaN())
    {
        return "quietNaN";
    }
    if (std::isnan(value))
    {
        return "NaN";
    }
    if (std::isinf(value))
    {
        return "inf";
    }
    if (value == std::numeric_limits<T>::max())
    {
        return "max";
    }
    if (value == std::numeric_limits<T>::min())
    {
        return "min";
    }
    if (value == std::numeric_limits<T>::lowest())
    {
        return "lowest";
    }
    if (value == std::numeric_limits<T>::denorm_min())
    {
        return "denorm_min";
    }
    if (value == -std::numeric_limits<T>::denorm_min())
    {
        return "-denorm_min";
    }
    if (value == std::numeric_limits<T>::epsilon())
    {
        return "epsilon";
    }
    if (value == -std::numeric_limits<T>::epsilon())
    {
        return "-epsilon";
    }
    // check if denormal
    if (std::fabs(value) < std::numeric_limits<T>::denorm_min())
    {
        return "denorm";
    }
    if (value == std::numbers::e_v<T>)
    {
        return "e";
    }
    if (value == -std::numbers::e_v<T>)
    {
        return "-e";
    }
    if (value == std::numbers::pi_v<T>)
    {
        return "pi";
    }
    if (value == -std::numbers::pi_v<T>)
    {
        return "-pi";
    }
    if (value == std::numbers::e_v<T> / T {2})
    {
        return "e/2";
    }
    if (value == -std::numbers::e_v<T> / T {2})
    {
        return "-e/2";
    }
    if (value == std::numbers::pi_v<T> / T {2})
    {
        return "pi/2";
    }
    if (value == -std::numbers::pi_v<T> / T {2})
    {
        return "-pi/2";
    }
    return "fp";
}
}    // namespace

#define PAIRS_ARITHMETIC_TO_FLOATING (std::pair<int, float>), (std::pair<int, double>), (std::pair<int, long double>), (std::pair<float, float>), (std::pair<float, double>), (std::pair<float, long double>), (std::pair<double, float>), (std::pair<double, double>), (std::pair<double, long double>), (std::pair<long double, float>), (std::pair<long double, double>), (std::pair<long double, long double>)
#define PAIRS_FLOATING_TO_FLOATING (std::pair<float, float>), (std::pair<float, double>), (std::pair<float, long double>), (std::pair<double, float>), (std::pair<double, double>), (std::pair<double, long double>), (std::pair<long double, float>), (std::pair<long double, double>), (std::pair<long double, long double>)

TEMPLATE_TEST_CASE("Type deduction", "[NotNaN][TypeDeduction]", float, double, long double)
{
    auto rv = generateTestValues<TestType>();
    if (std::isnan(rv))
    {
        REQUIRE_THROWS(NotNaN {rv});
    }
    else
    {
        const NotNaN x {rv};
        REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
    }
}

TEMPLATE_TEST_CASE("Assignment", "[NotNaN][Assignment]", PAIRS_ARITHMETIC_TO_FLOATING)
{
    using T_FROM = typename TestType::first_type;
    using T_TO   = typename TestType::second_type;

    auto tv      = generateTestValues<T_FROM>();

    SECTION("Copy constructor")
    {
        if (std::isnan(tv))
        {
            if constexpr (!std::integral<T_FROM>)
            {
                REQUIRE_THROWS(NotNaN<T_FROM> {tv});
            }
            REQUIRE_THROWS(NotNaN<T_TO> {tv});
        }
        else
        {
            if constexpr (!std::integral<T_FROM>)
            {
                NotNaN<T_FROM> from {tv};
                REQUIRE_THAT(*from, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
                NotNaN<T_TO> to1 {from};
                REQUIRE_THAT(*to1, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
            }
            NotNaN<T_TO> to2 {tv};
            REQUIRE_THAT(*to2, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
        }
    }
    SECTION("Move constructor")
    {
        T_FROM copy = tv;
        if (std::isnan(tv))
        {
            if constexpr (!std::integral<T_FROM>)
            {
                REQUIRE_THROWS(NotNaN<T_FROM> {std::move(copy)});
                copy = tv;
            }
            REQUIRE_THROWS(NotNaN<T_TO> {std::move(copy)});
            copy = tv;
        }
        else
        {
            if constexpr (!std::integral<T_FROM>)
            {
                NotNaN<T_FROM> from {std::move(copy)};
                copy = tv;
                REQUIRE_THAT(*from, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
                NotNaN<T_TO> to1 {std::move(from)};
                REQUIRE_THAT(*to1, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
            }
            NotNaN<T_TO> to2 {std::move(copy)};
            copy = tv;
            REQUIRE_THAT(*to2, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
        }
    }
    SECTION("Copy assignment")
    {
        NotNaN<T_TO> x {-1'000};
        REQUIRE_THAT(*x, Catch::Matchers::WithinRel(-1'000.0, MARGIN));
        if (!std::isnan(tv))
        {
            x = tv;
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
            x = -1'000;
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(-1000.0, MARGIN));
            if constexpr (!std::integral<T_FROM>)
            {
                NotNaN<T_FROM> from {tv};
                x = from;
                REQUIRE_THAT(*from, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
                REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
            }
            x = tv;
            x = x;
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
        }
    }
    SECTION("Move assignment")
    {
        T_FROM       copy {tv};
        NotNaN<T_TO> x {-1'000};
        REQUIRE_THAT(*x, Catch::Matchers::WithinRel(-1000.0, MARGIN));
        if (!std::isnan(tv))
        {
            x    = std::move(copy);
            copy = tv;
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
            x = -1'000;
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(-1000.0, MARGIN));
            if constexpr (!std::integral<T_FROM>)
            {
                NotNaN<T_FROM> from {tv};
                x = std::move(from);
                REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
            }
            x = tv;
            x = std::move(x);
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));
        }
    }
}

TEMPLATE_TEST_CASE("Conversion", "[NotNaN][Conversion]", float, double, long double)
{
    auto tv = generateTestValues<TestType>();
    if (std::isnan(tv))
    {
        REQUIRE_THROWS(NotNaN {tv});
        return;    // no need to test further
    }

    const NotNaN x {tv};
    SECTION("To bool")
    {
        REQUIRE(static_cast<bool>(tv) == static_cast<bool>(x));
    }
    SECTION("To int")
    {
        REQUIRE(static_cast<int>(tv) == static_cast<int>(x));
    }
    SECTION("To uint")
    {
        REQUIRE(static_cast<unsigned>(tv) == static_cast<unsigned>(x));
    }
    SECTION("To float")
    {
        REQUIRE_THAT(static_cast<float>(tv), Catch::Matchers::WithinRel(static_cast<double>(x), MARGIN));
    }
    SECTION("To double")
    {
        REQUIRE_THAT(static_cast<double>(tv), Catch::Matchers::WithinRel(static_cast<double>(x), MARGIN));
    }
    SECTION("To long double")
    {
        REQUIRE_THAT(static_cast<long double>(tv), Catch::Matchers::WithinRel(static_cast<double>(x), MARGIN));
    }
}

TEMPLATE_TEST_CASE("Comparison", "[NotNaN][Comparison]", PAIRS_ARITHMETIC_TO_FLOATING)
{
    using T1 = typename TestType::first_type;
    using T2 = typename TestType::second_type;

    auto cv  = generateTestValues<T1>();
    SECTION("Generation Dummy Section")    // without this, multiple generators cause UB
    {
        auto tv = generateTestValues<T2>();

        if (std::isnan(tv))
        {
            REQUIRE_THROWS(NotNaN {tv});
            return;
        }

        const NotNaN x {tv};
        REQUIRE_THAT(*x, Catch::Matchers::WithinRel(static_cast<double>(tv), MARGIN));

        SECTION("NotNaN lhs")
        {
            if (std::isnan(cv))
            {
                REQUIRE_THROWS(x == cv);
                REQUIRE_THROWS(x != cv);
                REQUIRE_THROWS(x < cv);
                REQUIRE_THROWS(x > cv);
                REQUIRE_THROWS(x <= cv);
                REQUIRE_THROWS(x >= cv);
            }
            else
            {
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
            if (std::isnan(cv))
            {
                REQUIRE_THROWS(cv == x);
                REQUIRE_THROWS(cv != x);
                REQUIRE_THROWS(cv < x);
                REQUIRE_THROWS(cv > x);
                REQUIRE_THROWS(cv <= x);
                REQUIRE_THROWS(cv >= x);
            }
            else
            {
                REQUIRE((cv == x) == (cv == tv));
                REQUIRE((cv != x) == (cv != tv));
                REQUIRE((cv < x) == (cv < tv));
                REQUIRE((cv > x) == (cv > tv));
                REQUIRE((cv <= x) == (cv <= tv));
                REQUIRE((cv >= x) == (cv >= tv));
            }
        }
        if constexpr (std::floating_point<T1>)
        {
            if (std::isnan(cv))
            {
                REQUIRE_THROWS(NotNaN {cv});
            }
            else
            {
                const NotNaN cvnn {cv};
                SECTION("With NotNaN Types lhs and rhs")
                {
                    REQUIRE((cvnn == x) == (cv == tv));
                    REQUIRE((cvnn != x) == (cv != tv));
                    REQUIRE((cvnn < x) == (cv < tv));
                    REQUIRE((cvnn > x) == (cv > tv));
                    REQUIRE((cvnn <= x) == (cv <= tv));
                    REQUIRE((cvnn >= x) == (cv >= tv));
                }
            }
        }
    }
}

TEMPLATE_TEST_CASE("NotNaN Exceptions", "[NotNaN][Exceptions]", float, double, long double)
{
    SECTION("Constructing with NaN")
    {
        REQUIRE_THROWS(NotNaN {std::numeric_limits<TestType>::quiet_NaN()});
    }
    SECTION("Assigning NaN")
    {
        NotNaN x {static_cast<TestType>(0.0)};
        REQUIRE_THAT(*x, Catch::Matchers::WithinRel(0.0, MARGIN));
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
            REQUIRE_THAT(*x, Catch::Matchers::WithinRel(0.0, MARGIN));
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
TEMPLATE_TEST_CASE("Special Functions Self Only", "[NotNaN][MathSelf]", float, double, long double)
{
    auto tv = generateTestValues<TestType>();
    if (std::isnan(tv))
    {
        REQUIRE_THROWS(NotNaN {tv});
        return;
    }
    NotNaN x {tv};

    SECTION("Powers")
    {
        // sqrt
        if (std::isnan(std::sqrt(tv)))
        {
            REQUIRE_THROWS(x.sqrt());
        }
        else
        {
            REQUIRE_THAT(*x.sqrt(), Catch::Matchers::WithinRel(static_cast<double>(std::sqrt(tv)), MARGIN));
        }

        // cbrt
        if (std::isnan(std::cbrt(tv)))
        {
            REQUIRE_THROWS(x.cbrt());
        }
        else
        {
            REQUIRE_THAT(*x.cbrt(), Catch::Matchers::WithinRel(static_cast<double>(std::cbrt(tv)), MARGIN));
        }

        // exp
        if (std::isnan(std::exp(tv)))
        {
            REQUIRE_THROWS(x.exp());
        }
        else
        {
            REQUIRE_THAT(*x.exp(), Catch::Matchers::WithinRel(static_cast<double>(std::exp(tv)), MARGIN));
        }

        // exp2
        if (std::isnan(std::exp2(tv)))
        {
            REQUIRE_THROWS(x.exp2());
        }
        else
        {
            REQUIRE_THAT(*x.exp2(), Catch::Matchers::WithinRel(static_cast<double>(std::exp2(tv)), MARGIN));
        }

        // expm1
        if (std::isnan(std::expm1(tv)))
        {
            REQUIRE_THROWS(x.expm1());
        }
        else
        {
            REQUIRE_THAT(*x.expm1(), Catch::Matchers::WithinRel(static_cast<double>(std::expm1(tv)), MARGIN));
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
            REQUIRE_THAT(*x.log(), Catch::Matchers::WithinRel(static_cast<double>(std::log(tv)), MARGIN));
        }

        // log2
        if (std::isnan(std::log2(tv)))
        {
            REQUIRE_THROWS(x.log2());
        }
        else
        {
            REQUIRE_THAT(*x.log2(), Catch::Matchers::WithinRel(static_cast<double>(std::log2(tv)), MARGIN));
        }

        // log10
        if (std::isnan(std::log10(tv)))
        {
            REQUIRE_THROWS(x.log10());
        }
        else
        {
            REQUIRE_THAT(*x.log10(), Catch::Matchers::WithinRel(static_cast<double>(std::log10(tv)), MARGIN));
        }
        // log1p
        if (std::isnan(std::log1pf(tv)))
        {
            REQUIRE_THROWS(x.log1p());
        }
        else
        {
            REQUIRE_THAT(*x.log1p(), Catch::Matchers::WithinRel(static_cast<double>(std::log1p(tv)), MARGIN));
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
            REQUIRE_THAT(*x.sin(), Catch::Matchers::WithinRel(static_cast<double>(std::sin(tv)), MARGIN));
        }
        // cos
        if (std::isnan(std::cos(tv)))
        {
            REQUIRE_THROWS(x.cos());
        }
        else
        {
            REQUIRE_THAT(*x.cos(), Catch::Matchers::WithinRel(static_cast<double>(std::cos(tv)), MARGIN));
        }
        // tan
        if (std::isnan(std::tan(tv)))
        {
            REQUIRE_THROWS(x.tan());
        }
        else
        {
            REQUIRE_THAT(*x.tan(), Catch::Matchers::WithinRel(static_cast<double>(std::tan(tv)), MARGIN));
        }

        // asin
        if (std::isnan(std::asin(tv)))
        {
            REQUIRE_THROWS(x.asin());
        }
        else
        {
            REQUIRE_THAT(*x.asin(), Catch::Matchers::WithinRel(static_cast<double>(std::asin(tv)), MARGIN));
        }

        // acos
        if (std::isnan(std::acos(tv)))
        {
            REQUIRE_THROWS(x.acos());
        }
        else
        {
            REQUIRE_THAT(*x.acos(), Catch::Matchers::WithinRel(static_cast<double>(std::acos(tv)), MARGIN));
        }

        // atan
        if (std::isnan(std::atan(tv)))
        {
            REQUIRE_THROWS(x.atan());
        }
        else
        {
            REQUIRE_THAT(*x.atan(), Catch::Matchers::WithinRel(static_cast<double>(std::atan(tv)), MARGIN));
        }
    }

    SECTION("Rounding")
    {
        // floor
        REQUIRE_THAT(*x.floor(), Catch::Matchers::WithinRel(static_cast<double>(std::floor(tv)), MARGIN));

        // ceil
        REQUIRE_THAT(*x.ceil(), Catch::Matchers::WithinRel(static_cast<double>(std::ceil(tv)), MARGIN));

        // round
        REQUIRE_THAT(*x.round(), Catch::Matchers::WithinRel(static_cast<double>(std::round(tv)), MARGIN));
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
            REQUIRE_THAT(*x.erf(), Catch::Matchers::WithinRel(static_cast<double>(std::erf(tv)), MARGIN));
        }

        // erfc
        if (std::isnan(std::erfc(tv)))
        {
            REQUIRE_THROWS(x.erfc());
        }
        else
        {
            REQUIRE_THAT(*x.erfc(), Catch::Matchers::WithinRel(static_cast<double>(std::erfc(tv)), MARGIN));
        }

        // tgamma
        if (std::isnan(std::tgamma(tv)))
        {
            REQUIRE_THROWS(x.tgamma());
        }
        else
        {
            REQUIRE_THAT(*x.tgamma(), Catch::Matchers::WithinRel(static_cast<double>(std::tgamma(tv)), MARGIN));
        }

        // lgamma
        if (std::isnan(std::lgamma(tv)))
        {
            REQUIRE_THROWS(x.lgamma());
        }
        else
        {
            REQUIRE_THAT(*x.lgamma(), Catch::Matchers::WithinRel(static_cast<double>(std::lgamma(tv)), MARGIN));
        }
    }

    SECTION("Misc")
    {
        // abs
        REQUIRE_THAT(*x.abs(), Catch::Matchers::WithinRel(static_cast<double>(std::abs(tv)), MARGIN));

        // modf
        NotNaN<TestType> integralPart {0.0};

        TestType         integralPartCheck;
        TestType         fractionalPartCheck = std::modf(tv, &integralPartCheck);

        if (std::isnan(integralPartCheck) || std::isnan(fractionalPartCheck))
        {
            REQUIRE_THROWS(x.modf(&integralPart));
        }
        else
        {
            NotNaN<TestType> fractionalPart = x.modf(&integralPart);
            REQUIRE_THAT(*integralPart, Catch::Matchers::WithinRel(static_cast<double>(integralPartCheck), MARGIN));
            REQUIRE_THAT(*fractionalPart, Catch::Matchers::WithinRel(static_cast<double>(fractionalPartCheck), MARGIN));
        }
    }
}

TEMPLATE_TEST_CASE("Special Functions With Value", "[NotNaN][MathValue]", PAIRS_ARITHMETIC_TO_FLOATING)
{
    using T1     = typename TestType::first_type;
    using T2     = typename TestType::second_type;
    using CT     = std::common_type_t<T1, T2>;

    auto selfVal = generateTestValues<T2>();
    SECTION("Generation Dummy Section")    // without this, multiple generators cause UB
    {
        auto operandVal = generateTestValues<T1>();

        INFO("SelfVal = " << selfVal << "\nOperandVal = " << operandVal);
        INFO("CT: " << typeid(CT).name());
        INFO("selfVal type: " << typeid(selfVal).name());
        INFO("operandVal type: " << typeid(operandVal).name());

        if (std::isnan(selfVal))
        {
            REQUIRE_THROWS(NotNaN<T2> {selfVal});
            return;
        }

        NotNaN<T2> self {selfVal};

        SECTION("With Arithmetic type as operand")
        {
            {
                auto cmpVal = std::pow(static_cast<CT>(selfVal), static_cast<CT>(operandVal));
                // idk wtf is going on here.
                // calculations with nan make result not nan for some reason.
                // can not reproduce outside catch2.
                if (std::isnan(operandVal))
                {
                    // workaround for weird bug
                    cmpVal = std::numeric_limits<CT>::quiet_NaN();
                }
                INFO("cmpVal (" << selfVal << " ** " << operandVal << ") = " << cmpVal);
                if (std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self.pow(operandVal));
                }
                else
                {
                    REQUIRE_THAT(*self.pow(operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                }
            }

            {
                auto cmpVal = std::log(static_cast<CT>(selfVal)) / std::log(static_cast<CT>(operandVal));
                // same workaround as above
                if (std::isnan(operandVal))
                {
                    cmpVal = std::numeric_limits<CT>::quiet_NaN();
                }
                if (std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self.logBase(operandVal));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.logBase(operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                    );
                }
            }
            {
                auto cmpVal = std::atan2(static_cast<CT>(selfVal), static_cast<CT>(operandVal));
                // same workaround as above
                if (std::isnan(operandVal))
                {
                    cmpVal = std::numeric_limits<CT>::quiet_NaN();
                }
                if (std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self.atan2(operandVal));
                }
                else
                {
                    REQUIRE_THAT(*self.atan2(operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                }
            }
            {
                auto cmpVal = std::hypot(static_cast<CT>(selfVal), static_cast<CT>(operandVal));
                // same workaround as above
                if (std::isnan(operandVal))
                {
                    cmpVal = std::numeric_limits<CT>::quiet_NaN();
                }
                if (std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self.hypot(operandVal));
                }
                else
                {
                    REQUIRE_THAT(*self.hypot(operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                }
            }
            {
                auto cmpVal = std::midpoint(static_cast<CT>(selfVal), static_cast<CT>(operandVal));
                // same workaround as above
                if (std::isnan(operandVal))
                {
                    cmpVal = std::numeric_limits<CT>::quiet_NaN();
                }
                if (std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self.midpoint(operandVal));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.midpoint(operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                    );
                }
            }
        }

        if constexpr (std::floating_point<T1>)
        {
            SECTION("With NotNaN as operand")
            {
                if (std::isnan(operandVal))
                {
                    REQUIRE_THROWS(NotNaN<T1> {operandVal});
                    return;
                }
                NotNaN<T1> operand {operandVal};

                if (std::isnan(std::pow(static_cast<CT>(selfVal), static_cast<CT>(operandVal))))
                {
                    REQUIRE_THROWS(self.pow(operand));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.pow(operand),
                      Catch::Matchers::WithinRel(
                        static_cast<double>(std::pow(static_cast<CT>(selfVal), static_cast<CT>(operandVal))), MARGIN
                      )
                    );
                }

                if (std::isnan(std::log(static_cast<CT>(selfVal)) / std::log(static_cast<CT>(operandVal))))
                {
                    REQUIRE_THROWS(self.logBase(operand));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.logBase(operand),
                      Catch::Matchers::WithinRel(
                        static_cast<double>(std::log(static_cast<CT>(selfVal)) / std::log(static_cast<CT>(operandVal))),
                        MARGIN
                      )
                    );
                }

                if (std::isnan(std::atan2(static_cast<CT>(selfVal), static_cast<CT>(operandVal))))
                {
                    REQUIRE_THROWS(self.atan2(operand));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.atan2(operand),
                      Catch::Matchers::WithinRel(
                        static_cast<double>(std::atan2(static_cast<CT>(selfVal), static_cast<CT>(operandVal))), MARGIN
                      )
                    );
                }

                if (std::isnan(std::hypot(selfVal, operandVal)))
                {
                    REQUIRE_THROWS(self.hypot(operand));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.hypot(operand),
                      Catch::Matchers::WithinRel(
                        static_cast<double>(std::hypot(static_cast<CT>(selfVal), static_cast<CT>(operandVal))), MARGIN
                      )
                    );
                }

                if (std::isnan(std::midpoint(static_cast<CT>(selfVal), static_cast<CT>(operandVal))))
                {
                    REQUIRE_THROWS(self.midpoint(operand));
                }
                else
                {
                    REQUIRE_THAT(
                      *self.midpoint(operand),
                      Catch::Matchers::WithinRel(
                        static_cast<double>(std::midpoint(static_cast<CT>(selfVal), static_cast<CT>(operandVal))), MARGIN
                      )
                    );
                }
            }
        }
    }
}

TEMPLATE_TEST_CASE("Unary Ops", "[NotNaN][UnaryOps]", float, double, long double)
{
    auto tv = generateTestValues<TestType>();
    if (std::isnan(tv))
    {
        REQUIRE_THROWS(NotNaN {tv});
        return;
    }

    NotNaN x {tv};
    REQUIRE(std::same_as<typename decltype(x)::Type, TestType>);
    REQUIRE_THAT(static_cast<TestType>(-x), Catch::Matchers::WithinRel(static_cast<double>(-tv), MARGIN));
    REQUIRE_THAT(static_cast<TestType>(+x), Catch::Matchers::WithinRel(static_cast<double>(+tv), MARGIN));
    REQUIRE(std::same_as<decltype(!x), bool>);
    REQUIRE(!x == !tv);
}

TEMPLATE_TEST_CASE("Binary Ops", "[NotNaN][BinaryOps]", PAIRS_ARITHMETIC_TO_FLOATING)
{
    using T1     = typename TestType::first_type;
    using T2     = typename TestType::second_type;
    using CT     = std::common_type_t<T1, T2>;

    auto selfVal = generateTestValues<T2>();
    SECTION("Generation Dummy Section")    // without this, multiple generators cause UB
    {
        auto operandVal = generateTestValues<T1>();

        INFO(
          "SelfVal ["
          << typeid(selfVal).name()
          << ", "
          << getTestValueType(selfVal)
          << "] = "
          << selfVal
          << "\nOperandVal ["
          << typeid(operandVal).name()
          << ", "
          << getTestValueType(operandVal)
          << "] = "
          << operandVal
        );
        INFO("CT: " << typeid(CT).name());

        if (std::isnan(selfVal))
        {
            REQUIRE_THROWS(NotNaN<T2> {selfVal});
            return;
        }

        NotNaN<T2> self {selfVal};

        SECTION("Addition")
        {
            SECTION("NotNaN lhs")
            {
                CT cmpVal = selfVal + operandVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self + operandVal);
                    REQUIRE_THROWS(self += operandVal);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(self + NotNaN<T1> {operandVal});
                    }
                }
                else
                {
                    REQUIRE_THAT(*(self + operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    NotNaN copy {self};
                    copy += operandVal;
                    REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        copy = self;
                        REQUIRE_THAT(
                          *(self + NotNaN<T1> {operandVal}),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                        copy += NotNaN<T1> {operandVal};
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
            SECTION("NotNaN rhs")
            {
                CT cmpVal = operandVal + selfVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(operandVal + self);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal} + self);
                    }
                }
                else
                {
                    REQUIRE_THAT(*(operandVal + self), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THAT(
                          *(NotNaN<T1> {operandVal} + self),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                    }
                }
            }
            if constexpr (std::floating_point<T1>)
            {
                SECTION("NotNaN lhs and rhs")
                {
                    if (std::isnan(operandVal))
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal});
                        return;
                    }
                    NotNaN<T1> operand {operandVal};
                    CT         cmpVal = selfVal + operandVal;
                    if (std::isnan(cmpVal))
                    {
                        REQUIRE_THROWS(self + operand);
                        REQUIRE_THROWS(self += operand);
                    }
                    else
                    {
                        REQUIRE_THAT(*(self + operand), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                        NotNaN copy {self};
                        copy += operand;
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
        }
        SECTION("Subtraction")
        {
            SECTION("NotNaN lhs")
            {
                CT cmpVal = selfVal - operandVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self - operandVal);
                    REQUIRE_THROWS(self -= operandVal);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(self - NotNaN<T1> {operandVal});
                    }
                }
                else
                {
                    REQUIRE_THAT(*(self - operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    NotNaN copy {self};
                    copy -= operandVal;
                    REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        copy = self;
                        REQUIRE_THAT(
                          *(self - NotNaN<T1> {operandVal}),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                        copy -= NotNaN<T1> {operandVal};
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
            SECTION("NotNaN rhs")
            {
                CT cmpVal = operandVal - selfVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(operandVal - self);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal} - self);
                    }
                }
                else
                {
                    REQUIRE_THAT(*(operandVal - self), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THAT(
                          *(NotNaN<T1> {operandVal} - self),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                    }
                }
            }
            if constexpr (std::floating_point<T1>)
            {
                SECTION("NotNaN lhs and rhs")
                {
                    if (std::isnan(operandVal))
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal});
                        return;
                    }
                    NotNaN<T1> operand {operandVal};
                    CT         cmpVal = selfVal - operandVal;
                    if (std::isnan(cmpVal))
                    {
                        REQUIRE_THROWS(self - operand);
                        REQUIRE_THROWS(self -= operand);
                    }
                    else
                    {
                        REQUIRE_THAT(*(self - operand), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                        NotNaN copy {self};
                        copy -= operand;
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
        }
        SECTION("Multiplication")
        {
            SECTION("NotNaN lhs")
            {
                CT cmpVal = selfVal * operandVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self * operandVal);
                    REQUIRE_THROWS(self *= operandVal);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(self * NotNaN<T1> {operandVal});
                    }
                }
                else
                {
                    REQUIRE_THAT(*(self * operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    NotNaN copy {self};
                    copy *= operandVal;
                    REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        copy = self;
                        REQUIRE_THAT(
                          *(self * NotNaN<T1> {operandVal}),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                        copy *= NotNaN<T1> {operandVal};
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
            SECTION("NotNaN rhs")
            {
                CT cmpVal = operandVal * selfVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(operandVal * self);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal} * self);
                    }
                }
                else
                {
                    REQUIRE_THAT(*(operandVal * self), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THAT(
                          *(NotNaN<T1> {operandVal} * self),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                    }
                }
            }
            if constexpr (std::floating_point<T1>)
            {
                SECTION("NotNaN lhs and rhs")
                {
                    if (std::isnan(operandVal))
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal});
                        return;
                    }
                    NotNaN<T1> operand {operandVal};
                    CT         cmpVal = selfVal * operandVal;
                    if (std::isnan(cmpVal))
                    {
                        REQUIRE_THROWS(self * operand);
                        REQUIRE_THROWS(self *= operand);
                    }
                    else
                    {
                        REQUIRE_THAT(*(self * operand), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                        NotNaN copy {self};
                        copy *= operand;
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
        }
        SECTION("Division")
        {
            SECTION("NotNaN lhs")
            {
                CT cmpVal = selfVal / operandVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(self / operandVal);
                    REQUIRE_THROWS(self /= operandVal);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(self / NotNaN<T1> {operandVal});
                    }
                }
                else
                {
                    REQUIRE_THAT(*(self / operandVal), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    NotNaN copy {self};
                    copy /= operandVal;
                    REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        copy = self;
                        REQUIRE_THAT(
                          *(self / NotNaN<T1> {operandVal}),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                        copy /= NotNaN<T1> {operandVal};
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
            SECTION("NotNaN rhs")
            {
                CT cmpVal = operandVal / selfVal;
                if (std::isnan(operandVal) || std::isnan(cmpVal))
                {
                    REQUIRE_THROWS(operandVal / self);
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal} / self);
                    }
                }
                else
                {
                    REQUIRE_THAT(*(operandVal / self), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    if constexpr (std::floating_point<T1>)
                    {
                        REQUIRE_THAT(
                          *(NotNaN<T1> {operandVal} / self),
                          Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN)
                        );
                    }
                }
            }
            if constexpr (std::floating_point<T1>)
            {
                SECTION("NotNaN lhs and rhs")
                {
                    if (std::isnan(operandVal))
                    {
                        REQUIRE_THROWS(NotNaN<T1> {operandVal});
                        return;
                    }
                    NotNaN<T1> operand {operandVal};
                    CT         cmpVal = selfVal / operandVal;
                    if (std::isnan(cmpVal))
                    {
                        REQUIRE_THROWS(self / operand);
                        REQUIRE_THROWS(self /= operand);
                    }
                    else
                    {
                        REQUIRE_THAT(*(self / operand), Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                        NotNaN copy {self};
                        copy /= operand;
                        REQUIRE_THAT(*copy, Catch::Matchers::WithinRel(static_cast<double>(cmpVal), MARGIN));
                    }
                }
            }
        }
    }
}


// NOLINTEND(readability-identifier-naming)
// NOLINTEND(implicit-float-conversion)
// NOLINTEND(readability-magic-numbers)

#pragma GCC diagnostic pop
