# NotNaN
A c++ templated class for floating point values that can never be NaN.
NaN usually means that some computation has gone wrong. The problem is that testing for NaN may be forgotten, making debugging difficult.

## Requirements
- c++20 or higher, because of std::format (>= gcc-13).
- Catch2 for testing.

## Usage
It's very easy. Simply create the wrapper type. The template type parameter can be deduced.

```cpp
NotNaN x {14.4F};
auto y = x + -3.1415;
```

To get the value back

All standard operations are available with both NotNaN types and built-in arithmetic types.
- Binary operations: `+`, `-`, `*`, `/`
- Compound assignment: `+=`, `-=`, `*=`, `/=`
- Unary operations: `!`, `-`, `+`
- Comparisons: `<`, `<=`, `>`, `>=`, `==`, `!=`. As ever, direct comparisons of floating point values with == need to be done with care.
- Conversions: Use `static_cast` to convert to another NotNaN type (for example `NotNaN<float>` to `NotNaN<double>`) or to a built-in arithmetic type (for example `int` or `bool`)
- Dereference: Use the dereference operator `*` to get a copy of the value. This is not a reference, as to avoid being able to assign to it, possibly giving it the NaN value.
- Math operations from cmath as member functions. They will throw if the result is NaN: `.sqrt`, `.cbrt`, `.log10`, `.exp2`, `sin`, etc. `.logBase(base)` for convenience. Also `.midpoint(other)`.

Streaming operations:
- `ostream << NotNaN`, behaves as with float.
- `istream >> NotNaN`, behaves as with float. NaN will throw.

Formatting:

Formatting is supported via `std::formatter`. Works with `std::format` (c++20) and `std::print` (c++23).
See [The formatting documentation](https://en.cppreference.com/w/cpp/utility/format/spec) for details.

If at any time NaN is assigned, calculated, or compared with, then the class with throw an exception, either `std::invalid_argument` or `std::runtime_error`.

All functions marked `noexcept` should not throw. Conversely, those not marked `noexcept` may throw.

For example operator+ may throw:

```cpp
NotNaN x{std::numeric_limits<float>::infinity()}; // OK
NotNaN y{-std::numeric_limits<float>::infinity()} // OK
NotNaN z = x + y; // will throw std::runtime_error
```

## Testing
Catch2 unit tests are provided in `tests/notnan_test.cpp`.

To build and run the tests:
```bash
mkdir build
cd build
cmake -G ninja ..
ninja
ctest --output-on-failure
```
