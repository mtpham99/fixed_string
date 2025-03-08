# fixed_string (mtp-libs)

A c++20 implementation of [proposal 3094: basic_fixed_string](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3094r5.html).


# Example

See [tests](/test/fixed_string_test.cpp) for more examples.

```cpp
#include <mtp/fixed_string.hpp>
using mtp::fixed_string;

// Allows passing string literals as non-type template parameteres (NTTPs)
template <fixed_string... fixstrs>
constexpr auto concat_strs() {
  return (fixed_string<0>{} + ... + fixstrs);
}

auto main() -> int {
  constexpr auto fs = concat_strs<"Hello", ", ", "World", "!\n">();
  static_assert(fs == "Hello, World!\n");

  return 0;
}
```


# Build

## Single header

Drop [fixed_string.hpp](/include/mtp/fixed_string.hpp) directly in your project where your compiler can find it.


## CMake

Add subdirectory and link against `mtp::fixed_string`.

```cmake
add_subdirectory(PATH_TO_THIS_DIR)
target_link_libraries(MY_TARGET mtp::fixed_string)
```

Optional build options:

1. `MTP_BUILD_TEST`: build tests (default: on if this is the top level project)
2. `MTP_NO_EXCEPTIONS`: disable exceptions (default: off)


# Test

Tests are implmented using [Catch2 (2.x)](https://github.com/catchorg/Catch2/tree/v2.13.10). Easiest way to build them is using the provided `CMakeLists.txt`. Catch2 is automatically downloaded if not found by CMake.

Tests are enabled via the `MTP_BUILD_TEST` option (on by default if building from this directory).

```sh
cmake -S PATH_TO_THIS_DIR -B OUTPUT_BUILD_DIR -DCMAKE_CXX_FLAGS="-std=c++2a" -DMTP_BUILD_TEST=ON
cmake --build OUTPUT_BUILD_DIR
ctest --test-dir OUTPUT_BUILD_DIR/test -j$(nproc)
```

Has been tested using:

1. GCC (11.5, 14.2) w/ c++20, c++23
2. Clang w/ libc++ (14.0, 19.1) w/ c++20, c++23


# Links

1. [proposal 3094: basic_fixed_string](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3094r5.html)
