#pragma once

#include <cmath>
#include <concepts>
#include <format>
#include <functional>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <type_traits>

template <typename T>
    requires std::floating_point<T>
class NotNaN
{
  private:
    T m_value;

  protected:
    // NotNaN lhs (implied), arithmetic rhs
    template <typename U, typename BinaryOp>
        requires (std::is_arithmetic_v<std::remove_cvref_t<U>> && std::regular_invocable<BinaryOp, T, U>)
    [[nodiscard]]
    constexpr auto arithmeticHelper(const U& rhs, const BinaryOp& operation) const
    {
        return arithmeticHelper(m_value, rhs, operation);
    }

    // arithmetic lhs and arithmetic rhs
    template <typename U, typename BinaryOp>
        requires (std::is_arithmetic_v<std::remove_cvref_t<U>>)
    [[nodiscard]]
    static constexpr auto arithmeticHelper(const auto& lhs, const U& rhs, const BinaryOp& operation)
        requires std::regular_invocable<BinaryOp, decltype(lhs), U>
                 && std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        if (std::isnan(lhs))
        {
            throw std::invalid_argument(std::format("lhs is {}", lhs));
        }
        if (std::isnan(rhs))
        {
            throw std::invalid_argument(std::format("rhs is {}", rhs));
        }

        using CT = std::common_type_t<decltype(lhs), U>;
        // no static_cast<T>(lhs | rhs) to force compiler warnings on caller site for incompatible types
        CT val   = operation(lhs, rhs);
        if (std::isnan(val))
        {
            throw std::runtime_error(std::format("Result is {}", val));
        }
        return val;
    }

    // NotNaN lhs (implied), NotNaN rhs
    template <std::floating_point F, typename BinaryOp>
        requires std::regular_invocable<BinaryOp, T, F>
    [[nodiscard]]
    constexpr auto arithmeticHelper(const NotNaN<F>& rhs, const BinaryOp& operation) const
    {
        return arithmeticHelper(m_value, rhs, operation);
    }

    // arithmetic lhs, NotNaN rhs
    template <std::floating_point F, typename BinaryOp>
    [[nodiscard]]
    static constexpr auto arithmeticHelper(const auto& lhs, const NotNaN<F>& rhs, const BinaryOp& operation)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        using CT = std::common_type_t<decltype(lhs), F>;
        // no static_cast<T>(lhs | rhs) to force compiler warnings on caller site for incompatible types
        CT val   = operation(lhs, *rhs);
        if (std::isnan(val))
        {
            throw std::runtime_error(std::format("Result is {}", val));
        }
        return val;
    }

  public:
    using Type = T;

    // Default constructor
    NotNaN()   = delete;    // force initialization
    ~NotNaN()  = default;

    // Constructor with value
    constexpr explicit NotNaN(const T& value) : m_value {value}
    {
        if (std::isnan(value))
        {
            throw std::invalid_argument(std::format("Can not construct with {}", value));
        }
    }

    // Copy constructor from other NotNaN
    template <std::floating_point F>
        requires (!std::same_as<T, F>)
    constexpr explicit NotNaN(const NotNaN<F>& other) noexcept : m_value(*other)
    {
        // empty
    }

    constexpr NotNaN(const NotNaN& other) noexcept = default;

    // Move constructor from other NotNaN
    template <std::floating_point F>
    constexpr NotNaN(NotNaN<F>&& other) noexcept
        requires (!std::same_as<T, F>)
            : m_value(*other)
    {
        // empty
    }

    constexpr NotNaN(NotNaN&& other) noexcept = default;

    // Copy assignment from other NotNaN
    template <std::floating_point F>
    constexpr auto operator= (const NotNaN<F>& other) noexcept -> NotNaN&
        requires (!std::same_as<T, F>)
    {
        // no self check required because we know T != F
        m_value = *other;
        return *this;
    }

    constexpr auto operator= (const NotNaN& other) noexcept -> NotNaN& = default;

    // Move assignment from other NotNaN
    template <std::floating_point F>
    constexpr auto operator= (NotNaN<F>&& other) noexcept -> NotNaN&
        requires (!std::same_as<T, F>)
    {
        // no self check required because we know T != F
        m_value = *other;
        return *this;
    }

    constexpr auto operator= (NotNaN&& other) noexcept -> NotNaN& = default;

    // Copy assignment from value
    constexpr auto operator= (const auto& value) -> NotNaN&
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(value)>>
    {
        if (std::isnan(value))
        {
            throw std::invalid_argument(std::format("Can not assign {}", value));
        }
        // no static_cast<T>(value) to force compiler warnings on caller site for incompatible types
        m_value = value;
        return *this;
    }

    // Getting the value
    [[nodiscard]]
    constexpr auto operator* () const noexcept -> T
    {
        return m_value;
    }

    [[nodiscard]]
    explicit operator T () const noexcept
    {
        return m_value;
    }

    template <typename A>
    [[nodiscard]]
    explicit operator A () const noexcept
        requires std::convertible_to<T, A>
    {
        return static_cast<A>(m_value);
    }

    template <std::floating_point F>
    [[nodiscard]]
    explicit operator NotNaN<F> () const noexcept
        requires (!std::same_as<T, F>)
    {
        return {m_value};
    }

    // Comparison
    [[nodiscard]]
    friend constexpr auto operator<=> (const auto& lhs, const NotNaN& rhs)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        if (std::isnan(lhs))
        {
            throw std::invalid_argument(std::format("Can not compare with {}", lhs));
        }
        // no static_cast<T>(lhs) to force compiler warnings on caller site for incompatible types
        return lhs <=> *rhs;
    }
    [[nodiscard]]
    constexpr auto operator<=> (const auto& rhs) const
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
    {
        if (std::isnan(rhs))
        {
            throw std::invalid_argument(std::format("Can not compare with {}", rhs));
        }
        // no static_cast<T>(value) to force compiler warnings on caller site for incompatible types
        return m_value <=> rhs;
    }
    template <std::floating_point F>
    [[nodiscard]]
    constexpr auto operator<=> (const NotNaN<F>& rhs) const noexcept
    {
        // no static_cast<T>(value) to force compiler warnings on caller site for incompatible types
        return m_value <=> *rhs;
    }

    [[nodiscard]]
    friend constexpr auto operator== (const auto& lhs, const NotNaN& rhs) -> bool
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        if (std::isnan(lhs))
        {
            throw std::invalid_argument(std::format("Can not compare with {}", lhs));
        }
        // no static_cast<T>(lhs) to force compiler warnings on caller site for incompatible types
        return lhs == *rhs;
    }

    [[nodiscard]]
    constexpr auto operator== (const auto& rhs) const -> bool
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
    {
        if (std::isnan(rhs))
        {
            throw std::invalid_argument(std::format("Can not compare with {}", rhs));
        }
        // no static_cast<T>(value) to force compiler warnings on caller site for incompatible types
        return m_value == rhs;
    }
    template <std::floating_point F>
    [[nodiscard]]
    constexpr auto operator== (const NotNaN<F> rhs) const noexcept -> bool
    {
        // no static_cast<T>(value) to force compiler warnings on caller site for incompatible types
        return m_value == *rhs;
    }

    // Addition
    friend constexpr auto operator+ (const auto& lhs, const NotNaN& rhs)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        return NotNaN(arithmeticHelper(lhs, rhs, std::plus {}));
    }
    constexpr auto operator+ (const auto& rhs) const
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        return NotNaN(arithmeticHelper(rhs, std::plus {}));
    }
    constexpr auto operator+= (const auto& rhs)
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        m_value = arithmeticHelper(rhs, std::plus {});
        return *this;
    }

    // Subtraction
    friend constexpr auto operator- (const auto& lhs, const NotNaN& rhs)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        return NotNaN(arithmeticHelper(lhs, rhs, std::minus {}));
    }
    constexpr auto operator- (const auto& rhs) const
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        return NotNaN(arithmeticHelper(rhs, std::minus {}));
    }
    constexpr auto operator-= (const auto& rhs)
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        m_value = arithmeticHelper(rhs, std::minus {});
        return *this;
    }

    // Multiplication
    friend constexpr auto operator* (const auto& lhs, const NotNaN& rhs)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        return NotNaN(arithmeticHelper(lhs, rhs, std::multiplies {}));
    }
    constexpr auto operator* (const auto& rhs) const
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        return NotNaN(arithmeticHelper(rhs, std::multiplies {}));
    }
    constexpr auto operator*= (const auto& rhs)
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        m_value = arithmeticHelper(rhs, std::multiplies {});
        return *this;
    }

    // Division
    friend constexpr auto operator/ (const auto& lhs, const NotNaN& rhs)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(lhs)>>
    {
        return NotNaN(arithmeticHelper(lhs, rhs, std::divides {}));
    }
    constexpr auto operator/ (const auto& rhs) const
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        return NotNaN(arithmeticHelper(rhs, std::divides {}));
    }
    constexpr auto operator/= (const auto& rhs)
        requires (
          std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
          || std::convertible_to<std::remove_cvref_t<decltype(rhs)>, NotNaN>
        )
    {
        m_value = arithmeticHelper(rhs, std::divides {});
        return *this;
    }

    // Unary operators
    constexpr auto operator+ () const noexcept -> NotNaN { return NotNaN(+m_value); }
    constexpr auto operator- () const noexcept -> NotNaN { return NotNaN(-m_value); }

    // Boolean nonsense
    [[nodiscard]]
    constexpr auto operator!() const noexcept -> bool
    {
        return !static_cast<bool>(m_value);
    }

    // special functions
    // logs and powers

    [[nodiscard]]
    constexpr auto sqrt() const -> NotNaN
    {
        return NotNaN {std::sqrt(m_value)};
    }
    [[nodiscard]]
    constexpr auto cbrt() const -> NotNaN
    {
        return NotNaN {std::cbrt(m_value)};
    }

    [[nodiscard]]
    constexpr auto pow(const auto& rhs) const
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
    {
        using CT = std::common_type_t<T, std::remove_cvref_t<decltype(rhs)>>;
        if (std::isnan(rhs))
        {
            throw std::invalid_argument(std::format("Can not compare with {}", rhs));
        }
        return NotNaN<CT> {std::pow(static_cast<CT>(m_value), static_cast<CT>(rhs))};
    }

    template <std::floating_point F>
    [[nodiscard]]
    constexpr auto pow(const NotNaN<F>& rhs) const
    {
        using CT = std::common_type_t<T, F>;
        return NotNaN<CT> {std::pow(static_cast<CT>(m_value), static_cast<CT>(rhs))};
    }

    // natural log (base e)
    [[nodiscard]]
    constexpr auto log() const -> NotNaN
    {
        return NotNaN {std::log(m_value)};
    }

    [[nodiscard]]
    constexpr auto log2() const -> NotNaN
    {
        return NotNaN {std::log2(m_value)};
    }

    [[nodiscard]]
    constexpr auto log10() const -> NotNaN
    {
        return NotNaN {std::log10(m_value)};
    }

    [[nodiscard]]
    constexpr auto logBase(const auto& base)
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(base)>>
    {
        using CT = std::common_type_t<T, std::remove_cvref_t<decltype(base)>>;
        if (std::isnan(base))
        {
            throw std::invalid_argument(std::format("Base is {}", base));
        }
        return NotNaN<CT> {std::log(static_cast<CT>(m_value)) / std::log(static_cast<CT>(base))};
    }

    template <typename F>
    [[nodiscard]]
    constexpr auto logBase(const NotNaN<F>& base)
    {
        using CT = std::common_type_t<T, F>;
        return NotNaN<CT> {std::log(static_cast<CT>(m_value)) / std::log(static_cast<CT>(base))};
    }

    [[nodiscard]]
    constexpr auto log1p() const -> NotNaN
    {
        return NotNaN {std::log1p(m_value)};
    }

    // e ^ m_value
    [[nodiscard]]
    constexpr auto exp() const -> NotNaN
    {
        return NotNaN {std::exp(m_value)};
    }

    [[nodiscard]]
    constexpr auto exp2() const -> NotNaN
    {
        return NotNaN {std::exp2(m_value)};
    }

    [[nodiscard]]
    constexpr auto expm1() const -> NotNaN
    {
        return NotNaN {std::expm1(m_value)};
    }

    // values and rounding

    [[nodiscard]]
    constexpr auto abs() const -> NotNaN
    {
        return NotNaN {std::abs(m_value)};
    }

    [[nodiscard]]
    constexpr auto floor() const -> NotNaN
    {
        return NotNaN {std::floor(m_value)};
    }

    [[nodiscard]]
    constexpr auto ceil() const -> NotNaN
    {
        return NotNaN {std::ceil(m_value)};
    }

    [[nodiscard]]
    constexpr auto round() const -> NotNaN
    {
        return NotNaN {std::round(m_value)};
    }

    [[nodiscard]]
    constexpr auto modf(NotNaN<T>* const iptr) const -> NotNaN
    {
        T integralPart;
        T fractionalPart = std::modf(m_value, &integralPart);
        *iptr            = NotNaN(integralPart);
        return NotNaN {fractionalPart};
    }

    // trigonometry

    [[nodiscard]]
    constexpr auto sin() const -> NotNaN
    {
        return NotNaN {std::sin(m_value)};
    }

    [[nodiscard]]
    constexpr auto cos() const -> NotNaN
    {
        return NotNaN {std::cos(m_value)};
    }

    [[nodiscard]]
    constexpr auto tan() const -> NotNaN
    {
        return NotNaN {std::tan(m_value)};
    }

    [[nodiscard]]
    constexpr auto asin() const -> NotNaN
    {
        return NotNaN {std::asin(m_value)};
    }

    [[nodiscard]]
    constexpr auto acos() const -> NotNaN
    {
        return NotNaN {std::acos(m_value)};
    }

    [[nodiscard]]
    constexpr auto atan() const -> NotNaN
    {
        return NotNaN {std::atan(m_value)};
    }

    [[nodiscard]]
    constexpr auto atan2(const auto& y) const
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(y)>>
    {
        using CT = std::common_type_t<T, std::remove_cvref_t<decltype(y)>>;
        if (std::isnan(y))
        {
            throw std::invalid_argument(std::format("y is {}", y));
        }
        return NotNaN<CT> {std::atan2(static_cast<CT>(m_value), static_cast<CT>(y))};
    }

    template <typename F>
    [[nodiscard]]
    constexpr auto atan2(const NotNaN<F>& y) const
    {
        using CT = std::common_type_t<T, F>;
        return NotNaN<CT> {std::atan2(static_cast<CT>(m_value), static_cast<CT>(y))};
    }

    [[nodiscard]]
    constexpr auto hypot(const auto& y) const
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(y)>>
    {
        using CT = std::common_type_t<T, std::remove_cvref_t<decltype(y)>>;
        if (std::isnan(y))
        {
            throw std::invalid_argument(std::format("y is {}", y));
        }
        return NotNaN<CT> {std::hypot(static_cast<CT>(m_value), static_cast<CT>(y))};
    }

    template <typename F>
    [[nodiscard]]
    constexpr auto hypot(const NotNaN<F>& y) const
    {
        using CT = std::common_type_t<T, F>;
        return NotNaN<CT> {std::hypot(static_cast<CT>(m_value), static_cast<CT>(y))};
    }

    // error function and complementary error function
    [[nodiscard]]
    constexpr auto erf() const -> NotNaN
    {
        return NotNaN(std::erf(m_value));
    }
    [[nodiscard]]
    constexpr auto erfc() const -> NotNaN
    {
        return NotNaN {std::erfc(m_value)};
    }

    // gamma
    [[nodiscard]]
    constexpr auto tgamma() const -> NotNaN
    {
        return NotNaN {std::tgamma(m_value)};
    }
    [[nodiscard]]
    constexpr auto lgamma() const -> NotNaN
    {
        return NotNaN {std::lgamma(m_value)};
    }

    // midpoint
    [[nodiscard]]
    constexpr auto midpoint(const auto& rhs) const
        requires std::is_arithmetic_v<std::remove_cvref_t<decltype(rhs)>>
    {
        using CT = std::common_type_t<T, std::remove_cvref_t<decltype(rhs)>>;
        if (std::isnan(rhs))
        {
            throw std::invalid_argument(std::format("rhs is {}", rhs));
        }
        return NotNaN<CT> {std::midpoint(static_cast<CT>(m_value), static_cast<CT>(rhs))};
    }

    template <typename F>
    [[nodiscard]]
    constexpr auto midpoint(const NotNaN<F>& rhs) const
    {
        using CT = std::common_type_t<T, F>;
        return NotNaN<CT> {std::midpoint(static_cast<CT>(m_value), static_cast<CT>(rhs))};
    }

    // Stream operators
    friend auto operator<< (std::ostream& ostream, const NotNaN& value) -> std::ostream&
    {
        ostream << *value;
        return ostream;
    }
    friend auto operator>> (std::istream& istream, NotNaN value) -> std::istream&
    {
        T val;
        istream >> val;
        value = val;    // copy assignment checks for NaN
        return istream;
    }
};

// Formatter
template <std::floating_point T>
struct std::formatter<NotNaN<T>> : std::formatter<T>
{
    template <class FormatContext>
    auto format(const NotNaN<T>& t, FormatContext& ctx) const
    {
        return std::formatter<T>::format(*t, ctx);
    }
};
