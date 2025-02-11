#pragma once

#include <cmath>
#include <concepts>
#include <format>
#include <functional>
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
