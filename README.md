# fixed_string (mtp-libs)

A c++20 implementation of [proposal 3094: basic_fixed_string](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3094r5.html).

Available as both single header and [c++20 module](https://en.cppreference.com/w/cpp/language/modules).


# Example

See [tests](/test/fixed_string_test.cpp) for more examples.

```cpp
import mtp.fixed_string;
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

Exceptions can be disabled using a macro prior to including the header:

```cpp
#define MTP_NO_EXCEPTIONS
#include <mtp/fixed_string.hpp>
```


## CMake

Add subdirectory and link against `mtp::fixed_string`.

```cmake
add_subdirectory(PATH_TO_THIS_DIR)
target_link_libraries(MY_TARGET mtp::fixed_string)
```

Then include/import as usual:

```cpp
#include <mtp/fixed_string.hpp> // header-only
import mtp.fixed_string;        // module
```

Optional build options:

1. `MTP_BUILD_TEST`: build tests (default: on if this is the top level project)
2. `MTP_NO_EXCEPTIONS`: disable exceptions (default: off)
3. `MTP_BUILD_MODULE`: build as module instead of header-only (default: off)
4. `MTP_USE_STD_MODULE`: use [c++23 std module](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2465r3.pdf) (default: off)

Example module build (requires CMake 3.30+, Ninja 1.11+, Clang/Libc++ 18.1.2+):

```sh
cmake -S . -B build -G Ninja -DCMAKE_CXX_FLAGS="-std=c++2b -stdlib=libc++" -DMTP_BUILD_TEST=ON -DMTP_BUILD_MODULE=ON -DMTP_USE_STD_MODULE=ON
cmake --build build -j$(nproc)
ctest --test-dir build/test -j$(nproc)
```


# Links

1. [proposal 3094: basic_fixed_string](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p3094r5.html)
