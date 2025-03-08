#include <catch2/catch.hpp>

#include <version>

#ifdef __cpp_char8_t
#  define ALL_CHAR_TYPES char, wchar_t, char8_t, char16_t, char32_t
#else
#  define ALL_CHAR_TYPES char, wchar_t, char16_t, char32_t
#endif

#ifdef MTP_USE_STD_MODULE
import std;
#else
#  include <algorithm>
#  include <array>
#  include <cstddef>
#  ifdef __cpp_lib_format
#    include <format>
#  endif
#  include <functional>
#  if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
#    include <ranges>
#  endif
#  include <sstream>
#  if !defined(MTP_NO_EXCEPTIONS) && defined(__EXCEPTIONS)
#    include <stdexcept>
#  endif
#  include <string_view>
#  include <utility>
#endif

#ifdef MTP_BUILD_MODULE
import mtp.fixed_string;
#else
#  include <mtp/fixed_string.hpp>
#endif

namespace {

using mtp::basic_fixed_string;
using mtp::fixed_string;

template <typename CharT>
constexpr auto
c_strcmp(CharT const* s1, CharT const* s2) noexcept -> bool
{
  while (*s1 && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return *s1 == *s2;
}

template <typename CharT, std::size_t N>
constexpr auto
make_array_from_cstr(CharT const (&c_str)[N]) -> std::array<CharT, N - 1>
{
  return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    return std::array{ c_str[Is]... };
  }(std::make_index_sequence<N - 1>{});
}

} // namespace

TEMPLATE_TEST_CASE("constructors", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  SECTION("constructability & triviality")
  {
    using empty_fixed_string = basic_fixed_string<CharT, 0>;
    static_assert(std::is_trivially_default_constructible_v<empty_fixed_string>);
    static_assert(std::is_trivially_copy_constructible_v<empty_fixed_string>);
    static_assert(std::is_trivially_copy_assignable_v<empty_fixed_string>);
    static_assert(std::is_move_constructible_v<empty_fixed_string>);
    static_assert(std::is_move_assignable_v<empty_fixed_string>);
    static_assert(std::is_trivially_destructible_v<empty_fixed_string>);
    static_assert(std::is_trivially_copyable_v<empty_fixed_string>);

    using nonempty_fixed_string = basic_fixed_string<CharT, 1>;
    static_assert(not std::is_default_constructible_v<nonempty_fixed_string>);
    static_assert(std::is_trivially_copy_constructible_v<nonempty_fixed_string>);
    static_assert(std::is_trivially_copy_assignable_v<nonempty_fixed_string>);
    static_assert(std::is_move_constructible_v<nonempty_fixed_string>);
    static_assert(std::is_move_assignable_v<nonempty_fixed_string>);
    static_assert(std::is_trivially_destructible_v<nonempty_fixed_string>);
    static_assert(std::is_trivially_copyable_v<nonempty_fixed_string>);
  }

  SECTION("default constructor")
  {
    constexpr CharT sl_0[] = { '\0' };
    constexpr auto fs_0 = basic_fixed_string<CharT, 0>{};
    static_assert(c_strcmp(fs_0.c_str(), sl_0));
  }

  SECTION("char(s) constructor")
  {
    constexpr CharT sl_1[] = { '1', '\0' };
    constexpr auto fs_1 = basic_fixed_string{ CharT{ '1' } };
    static_assert(c_strcmp(fs_1.c_str(), sl_1));

    constexpr CharT sl_2[] = { '1', '2', '\0' };
    constexpr auto fs_2 = basic_fixed_string{ CharT{ '1' }, CharT{ '2' } };
    static_assert(c_strcmp(fs_2.c_str(), sl_2));
  }

  SECTION("string literal constructor")
  {
    constexpr CharT sl[] = { '1', '2', '3', '\0' };
    constexpr auto fs = basic_fixed_string{ sl };
    static_assert(c_strcmp(fs.c_str(), sl));
  }

  SECTION("iterator pair constructor")
  {
    constexpr CharT sl[] = { '1', '2', '3', '4', '\0' };
    constexpr auto fs = basic_fixed_string<CharT, 4>{ sl, sl + 4 };
    static_assert(c_strcmp(fs.c_str(), sl));
  }

  SECTION("copy construction & assignment")
  {
    constexpr CharT sl[] = { '1', '2', '3', '4', '\0' };
    constexpr auto fs = basic_fixed_string{ sl };
    static_assert(c_strcmp(fs.c_str(), sl));

    constexpr decltype(fs) fs_0{ fs };
    static_assert(c_strcmp(fs_0.c_str(), sl));
    static_assert(c_strcmp(fs.c_str(), sl));

    constexpr decltype(fs) fs_1 = fs;
    static_assert(c_strcmp(fs_1.c_str(), sl));
    static_assert(c_strcmp(fs.c_str(), sl));
  }

  SECTION("move construction & assignment")
  {
    constexpr CharT sl[] = { '1', '2', '3', '4', '\0' };
    constexpr auto fs = basic_fixed_string{ sl };
    static_assert(c_strcmp(fs.c_str(), sl));

    constexpr decltype(fs) fs_0{ std::move(fs) };
    static_assert(c_strcmp(fs_0.c_str(), sl));
    static_assert(c_strcmp(fs.c_str(), sl));

    constexpr decltype(fs) fs_1 = std::move(fs);
    static_assert(c_strcmp(fs_1.c_str(), sl));
    static_assert(c_strcmp(fs.c_str(), sl));
  }

#if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
  SECTION("range constructor")
  {
    constexpr CharT sl[] = { '1', '2', '3', '4', '5', '\0' };

    // general constructor
    constexpr auto fs_0 =
        basic_fixed_string<CharT, 5>{ std::from_range, std::basic_string_view{ sl } };
    static_assert(c_strcmp(fs_0.c_str(), sl));

    // CTAD std::array constructor
    constexpr auto fs_1 = basic_fixed_string{ std::from_range, make_array_from_cstr(sl) };
    static_assert(c_strcmp(fs_1.c_str(), sl));
  }
#endif
}

TEMPLATE_TEST_CASE("capacity", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  constexpr auto fs_0 = basic_fixed_string<CharT, 0>{};
  constexpr auto fs_1 = basic_fixed_string{ CharT{ '1' } };
  constexpr auto fs_2 = basic_fixed_string{ CharT{ '1' }, CharT{ '2' } };

  SECTION("empty")
  {
    static_assert(fs_0.empty());
    static_assert(not fs_1.empty());
    static_assert(not fs_2.empty());
  }

  SECTION("size")
  {
    static_assert(fs_0.size() == 0);
    static_assert(fs_1.size() == 1);
    static_assert(fs_2.size() == 2);
  }

  SECTION("length")
  {
    static_assert(fs_0.length() == 0);
    static_assert(fs_1.length() == 1);
    static_assert(fs_2.length() == 2);
  }

  SECTION("max_size")
  {
    static_assert(fs_0.max_size() == 0);
    static_assert(fs_1.max_size() == 1);
    static_assert(fs_2.max_size() == 2);
  }
}

TEMPLATE_TEST_CASE("element access", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  static constexpr CharT chars[] = { CharT{ '1' }, CharT{ '2' }, CharT{} };
  constexpr auto fs_0 = basic_fixed_string<CharT, 0>{};
  constexpr auto fs_1 = basic_fixed_string{ chars[0] };
  constexpr auto fs_2 = basic_fixed_string{ chars[0], chars[1] };

  SECTION("indexing")
  {
    SECTION("in range")
    {
      // static_assert(fs_0.at(0) == CharT{}); // undefined behaviour
      static_assert(fs_1.at(0) == chars[0]);
      static_assert(fs_2.at(0) == chars[0] && fs_2.at(1) == chars[1]);

      static_assert(fs_0[0] == CharT{});
      static_assert(fs_1[0] == chars[0] && fs_1[1] == CharT{});
      static_assert(fs_2[0] == chars[0] && fs_2[1] == chars[1] && fs_2[2] == CharT{});
    }

#if !defined(MTP_NO_EXCEPTIONS) && defined(__EXCEPTIONS)
    SECTION("out of range")
    {
      CHECK_THROWS_AS(fs_0.at(0), std::out_of_range);
      CHECK_THROWS_AS(fs_1.at(1), std::out_of_range);
      CHECK_THROWS_AS(fs_2.at(2), std::out_of_range);
    }
#endif
  }

  SECTION("front & back")
  {
    // static_assert(fs_0.front() == CharT{}); // undefined behaviour
    static_assert(fs_1.front() == chars[0]);
    static_assert(fs_2.front() == chars[0]);

    // static_assert(fs_0.back() == CharT{}); // undefined behaviour
    static_assert(fs_1.back() == chars[0]);
    static_assert(fs_2.back() == chars[1]);
  }

  SECTION("pointer")
  {
    static_assert(std::equal(chars, chars + 0, fs_0.data()));
    static_assert(std::equal(chars, chars + 1, fs_1.data()));
    static_assert(std::equal(chars, chars + 2, fs_2.data()));
    static_assert(*(fs_0.data() + fs_0.size()) == CharT{});
    static_assert(*(fs_1.data() + fs_1.size()) == CharT{});
    static_assert(*(fs_2.data() + fs_2.size()) == CharT{});

    static_assert(std::equal(chars, chars + 0, fs_0.c_str()));
    static_assert(std::equal(chars, chars + 1, fs_1.c_str()));
    static_assert(std::equal(chars, chars + 2, fs_2.c_str()));
    static_assert(*(fs_0.c_str() + fs_0.size()) == CharT{});
    static_assert(*(fs_1.c_str() + fs_1.size()) == CharT{});
    static_assert(*(fs_2.c_str() + fs_2.size()) == CharT{});
  }

  SECTION("string_view")
  {
    constexpr auto sv_0 = std::basic_string_view{ chars, chars + 0 };
    constexpr auto sv_1 = std::basic_string_view{ chars, chars + 1 };
    constexpr auto sv_2 = std::basic_string_view{ chars, chars + 2 };

    static_assert(fs_0.view() == sv_0);
    static_assert(fs_1.view() == sv_1);
    static_assert(fs_2.view() == sv_2);

    static_assert(std::basic_string_view<CharT>{ fs_0 } == sv_0);
    static_assert(std::basic_string_view<CharT>{ fs_1 } == sv_1);
    static_assert(std::basic_string_view<CharT>{ fs_2 } == sv_2);
  }
}

TEMPLATE_TEST_CASE("swap", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  constexpr auto fs_123 = basic_fixed_string{ CharT{ '1' }, CharT{ '2' }, CharT{ '3' } };
  constexpr auto fs_abc = basic_fixed_string{ CharT{ 'a' }, CharT{ 'b' }, CharT{ 'c' } };

  SECTION("member swap")
  {
    static_assert([&]() {
      auto fs_0 = fs_123;
      auto fs_1 = fs_abc;
      fs_0.swap(fs_1);
      return c_strcmp(fs_0.c_str(), fs_abc.c_str()) and c_strcmp(fs_1.c_str(), fs_123.c_str());
    }());
  }

  SECTION("non-member swap")
  {
    static_assert([&]() {
      auto fs_0 = fs_123;
      auto fs_1 = fs_abc;

      using std::swap;
      swap(fs_0, fs_1);
      // std::ranges::swap(fs_0, fs_1);

      return c_strcmp(fs_0.c_str(), fs_abc.c_str()) and c_strcmp(fs_1.c_str(), fs_123.c_str());
    }());
  }
}

TEMPLATE_TEST_CASE("concatenation", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  constexpr auto c_1 = CharT{ 'a' };
  constexpr auto c_2 = CharT{ 'b' };
  constexpr auto c_3 = CharT{ 'c' };

  constexpr CharT sl_1[] = { c_1, CharT{} };
  constexpr CharT sl_3[] = { c_3, CharT{} };

  constexpr CharT sl_res[] = { c_1, c_2, c_3, CharT{} };

  SECTION("lhs")
  {
    constexpr auto fs = basic_fixed_string{ c_1 } + c_2 + sl_3;
    static_assert(c_strcmp(fs.c_str(), sl_res));
  }

  SECTION("rhs")
  {
    constexpr auto fs = sl_1 + (c_2 + basic_fixed_string{ c_3 });
    static_assert(c_strcmp(fs.c_str(), sl_res));
  }
}

TEMPLATE_TEST_CASE("comparison", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  SECTION("equality")
  {
    constexpr CharT sl_0[] = { CharT{} };
    constexpr CharT sl_1[] = { CharT{ 'a' }, CharT{} };
    constexpr CharT sl_2[] = { CharT{ 'a' }, CharT{ 'b' }, CharT{} };

    constexpr auto fs_0 = basic_fixed_string{ sl_0 };
    constexpr auto fs_1 = basic_fixed_string{ sl_1 };
    constexpr auto fs_2 = basic_fixed_string{ sl_2 };

    constexpr auto fs = basic_fixed_string{ sl_1 };
    static_assert(fs != fs_0 && fs == fs_1 && fs != fs_2);
    static_assert(fs != sl_0 && fs == sl_1 && fs != sl_2);
  }

#if defined(__cpp_lib_three_way_comparison) && defined(__cpp_impl_three_way_comparison)
  SECTION("sort")
  {
    constexpr auto sorted = std::array{
      basic_fixed_string{ CharT{ 'a' } },
      basic_fixed_string{ CharT{ 'b' } },
      basic_fixed_string{ CharT{ 'c' } },
    };
    constexpr auto unsorted = std::array{
      basic_fixed_string{ CharT{ 'c' } },
      basic_fixed_string{ CharT{ 'a' } },
      basic_fixed_string{ CharT{ 'b' } },
    };
    static_assert(unsorted != sorted);

    static_assert([&]() consteval {
      auto arr = unsorted;
      std::sort(arr.begin(), arr.end());
      return arr == sorted;
    }());
  }
#endif
}

TEMPLATE_TEST_CASE("hash", "[fixed_string]", ALL_CHAR_TYPES)
{
  using CharT = TestType;

  static constexpr CharT sl[] = { CharT{ '1' }, CharT{ '2' }, CharT{ '3' }, CharT{} };
  constexpr auto fs = basic_fixed_string{ sl };
  CHECK(std::hash<basic_fixed_string<CharT, 3>>{}(fs) ==
        std::hash<std::basic_string_view<CharT>>{}(sl));
}

#if __cpp_nontype_template_args >= 201911L // class type NTTPs
template <basic_fixed_string fixstr>
constexpr auto
prepend_hello() noexcept
{
  return "Hello" + fixstr;
}

TEST_CASE("nttp", "[fixed_string]")
{
  constexpr auto sl = "Hello, World!\n";
  constexpr auto fs = prepend_hello<", World!\n">();
  static_assert(c_strcmp(fs.c_str(), sl));
}
#endif

TEST_CASE("stream output", "[fixed_string]")
{
#if __cpp_deduction_guides >= 201907L && (!defined(__GNUC__) || (__GNUC__ >= 13))
  auto const fs = fixed_string{ "Hello, World!\n" };
#else
  auto const fs = fixed_string<14>{ "Hello, World!\n" };
#endif
  std::ostringstream ss;
  ss << fs;
  CHECK(ss.str() == "Hello, World!\n");
}

#ifdef __cpp_lib_format
TEST_CASE("format", "[fixed_string]")
{
#  if __cpp_deduction_guides >= 201907L && (!defined(__GNUC__) || (__GNUC__ >= 13))
  auto const fs = fixed_string{ "Hello, World!\n" };
#  else
  auto const fs = fixed_string<14>{ "Hello, World!\n" };
#  endif
  CHECK(std::format("{}", fs) == "Hello, World!\n");
}
#endif
