# NotNaN
A c++ templated class for floating point values that can never be NaN

## Requirements
- c++20 or higher, because of std::format (>= gcc-13).
- Catch2 for testing.

## Usage
It's very easy. Simply create the wrapper type. The template type parameter can be deduced.

```cpp
NotNaN x {14.4F};
auto y = x + -3.1415;
```

All standard operations are available with both NotNaN types and built-in arithmetic types.
- Binary operations: `+`, `-`, `*`, `/`
- Compound assignment: `+=`, `-=`, `*=`, `/=`
- Unary operations: `!`, `-`, `+`
- Comparisons: `<`, `<=`, `>`, `>=`, `==`, `!=`. As ever, direct comparisons of floating point values with == need to be done with care.

Streaming operations:
- `ostream << NotNaN`, behaves as with float.
- `istream >> NotNaN`, behaves as with float. NaN will throw.

Formatting:
Formatting is supported via std::formatter. Works with std::format and std::print.

If at any time NaN is assigned or calculated or compared with, the class with throw an exception, either `std::invalid_argument` or `std::runtime_error`.

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
