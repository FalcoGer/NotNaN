#include "NotNaN.hpp"
#include <iostream>
#include <limits>
#include <print>

auto main() -> int
{
    // NOLINTBEGIN(readability-magic-numbers) // example code

    NotNaN f1 {12.F};
    std::println("{:>+7.2F}", f1);
    std::cout << f1 << '\n';
    auto f2  = f1 + 10.0;
    f2      += 1.0;
    std::println("{}", f2);
    std::println("{}", f2 + 3 == 26.F);

    f2 -= 20.F;
    std::println("{}", f2);

    f2 *= 2.F;
    std::println("{}", f2);

    auto f3 = 10 / f2;
    std::println("{}", f3);

    const float  INF = std::numeric_limits<float>::infinity();
    NotNaN f4 {INF};
    std::println("{}", f4);

    try
    {
        auto f5 = f4 - f4;
        std::println("{}", f5);
    }
    catch (const std::exception& ex)
    {
        std::println("Exception: {}", ex.what());
    }
    // NOLINTEND(readability-magic-numbers)
}
