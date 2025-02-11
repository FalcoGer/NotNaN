#include "NotNaN.hpp"
#include <iostream>
#include <limits>
#include <format>

auto main() -> int
{
    // NOLINTBEGIN(readability-magic-numbers) // example code

    NotNaN f1 {12.F};
    std::cout << std::format("{:>+7.2F}", f1) << '\n';
    std::cout << f1 << '\n';
    auto f2  = f1 + 10.0;
    f2      += 1.0;
    std::cout << std::format("{}", f2) << '\n';
    std::cout << std::format("{}", f2 + 3 == 26.F) << '\n';

    f2 -= 20.F;
    std::cout << std::format("{}", f2) << '\n';

    f2 *= 2.F;
    std::cout << std::format("{}", f2) << '\n';

    auto f3 = 10 / f2;
    std::cout << std::format("{}", f3) << '\n';

    const float  INF = std::numeric_limits<float>::infinity();
    NotNaN f4 {INF};
    std::cout << std::format("{}", f4) << '\n';

    try
    {
        auto f5 = f4 - f4;
        std::cout << std::format("{}", f5) << '\n';
    }
    catch (const std::exception& ex)
    {
        std::cout << std::format("Exception: {}", ex.what()) << '\n';
    }
    // NOLINTEND(readability-magic-numbers)
}
