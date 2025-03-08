#ifndef MTP_FIXED_STRING_HPP
#define MTP_FIXED_STRING_HPP

#ifndef MTP_EXPECTS
#  if defined(_MSC_VER) && !defined(__clang__)
#    define MTP_EXPECTS(cond) __assume(cond)
#  elif defined(__GNUC__) || defined(__clang__)
#    define MTP_EXPECTS(cond) ((cond) ? static_cast<void>(0) : __builtin_unreachable())
#  else
#    define MTP_EXPECTS(cond)
#  endif
#endif

#if !defined(MTP_NO_EXCEPTIONS) && defined(__EXCEPTIONS)
#  include <stdexcept>
#  define MTP_THROW(except) throw except
#else
#  define MTP_THROW(except)
#endif

#if __has_cpp_attribute(unlikely)
#  define MTP_UNLIKELY [[unlikely]]
#else
#  define MTP_UNLIKELY
#endif

#include <version>

#if defined(__cpp_lib_three_way_comparison) && defined(__cpp_impl_three_way_comparison)
#  include <compare>
#endif
#ifdef __cpp_lib_format
#  include <format>
#endif
#if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
#  include <ranges>
#endif

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <iterator>
#include <string_view>
#include <type_traits>

namespace mtp {

// -------------------------------------------------------------------------------------------------
// typedefs
// -------------------------------------------------------------------------------------------------

template <typename CharT, std::size_t N>
struct basic_fixed_string;

template <std::size_t N>
using fixed_string = basic_fixed_string<char, N>;

template <std::size_t N>
using fixed_wstring = basic_fixed_string<wchar_t, N>;

#ifdef __cpp_char8_t
template <std::size_t N>
using fixed_u8string = basic_fixed_string<char8_t, N>;
#endif

template <std::size_t N>
using fixed_u16string = basic_fixed_string<char16_t, N>;

template <std::size_t N>
using fixed_u32string = basic_fixed_string<char32_t, N>;

template <typename CharT, std::size_t N>
struct basic_fixed_string
{
  // -----------------------------------------------------------------------------------------------
  // member data
  // -----------------------------------------------------------------------------------------------

  CharT _data[N + 1];

  // -----------------------------------------------------------------------------------------------
  // member types
  // -----------------------------------------------------------------------------------------------

  using value_type = CharT;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using reference = value_type&;
  using const_reference = value_type const&;
  using const_iterator = const_pointer;
  using iterator = const_iterator;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  // -----------------------------------------------------------------------------------------------
  // capacity
  // -----------------------------------------------------------------------------------------------

  static constexpr std::integral_constant<size_type, N> size{};
  static constexpr std::integral_constant<size_type, N> length{};
  static constexpr std::integral_constant<size_type, N> max_size{};
  static constexpr std::bool_constant<(N == 0)> empty{};

  // -----------------------------------------------------------------------------------------------
  // construction and assignment
  // -----------------------------------------------------------------------------------------------

  basic_fixed_string()
    requires(N == 0)
  = default;
  basic_fixed_string(basic_fixed_string const&) = default;
  basic_fixed_string& operator=(basic_fixed_string const&) = default;
  basic_fixed_string(basic_fixed_string&&) = default;
  basic_fixed_string& operator=(basic_fixed_string&&) = default;
  ~basic_fixed_string() = default;

  template <std::convertible_to<CharT>... CharTs>
    requires(sizeof...(CharTs) == N && (... && !std::is_pointer_v<CharTs>))
  explicit constexpr basic_fixed_string(CharTs... chars) noexcept : _data{ chars..., CharT{} }
  {}

  consteval basic_fixed_string(CharT const (&str)[N + 1]) noexcept
  {
    MTP_EXPECTS(str[N] == CharT{});
    for (auto i = 0u; i < N + 1; ++i) {
      _data[i] = str[i];
    }
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
    requires(std::is_convertible_v<std::iter_value_t<I>, CharT>)
  constexpr basic_fixed_string(I first, S last)
  {
    MTP_EXPECTS(std::distance(first, last) == N);
    auto it = _data;
    for (; first != last; ++it, ++first) {
      *it = *first;
    }
    *(it++) = CharT{};
  }

#if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
  template <std::ranges::input_range R>
    requires(std::is_convertible_v<std::ranges::range_reference_t<R>, CharT>)
  constexpr basic_fixed_string(std::from_range_t, R&& rg)
  {
    MTP_EXPECTS(std::ranges::size(rg) == N);
    auto it = _data;
    for (auto&& c : std::forward<R>(rg)) {
      *(it++) = std::move(c);
    }
    *(it++) = CharT{};
  }
#endif

  // -----------------------------------------------------------------------------------------------
  // iterators
  // -----------------------------------------------------------------------------------------------

  [[nodiscard]] constexpr auto
  begin() const noexcept -> const_iterator
  {
    return data();
  }

  [[nodiscard]] constexpr auto
  end() const noexcept -> const_iterator
  {
    return data() + size();
  }

  [[nodiscard]] constexpr auto
  rbegin() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ end() };
  }

  [[nodiscard]] constexpr auto
  rend() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ begin() };
  }

  [[nodiscard]] constexpr auto
  cbegin() const noexcept -> const_iterator
  {
    return data();
  }

  [[nodiscard]] constexpr auto
  cend() const noexcept -> const_iterator
  {
    return data() + size();
  }

  [[nodiscard]] constexpr auto
  crbegin() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ end() };
  }

  [[nodiscard]] constexpr auto
  crend() const noexcept -> const_reverse_iterator
  {
    return const_reverse_iterator{ begin() };
  }

  // -----------------------------------------------------------------------------------------------
  // element access
  // -----------------------------------------------------------------------------------------------

  [[nodiscard]] constexpr auto
  at(size_type pos) const -> const_reference
  {
    if (pos >= size())
      MTP_UNLIKELY
      {
        MTP_THROW(std::out_of_range("mtp::basic_fixed_string::at"));
      }
    return data()[pos];
  }

  [[nodiscard]] constexpr auto
  operator[](size_type pos) const -> const_reference
  {
    MTP_EXPECTS(pos <= size());
    return data()[pos];
  }

  [[nodiscard]] constexpr auto
  front() const -> const_reference
  {
    MTP_EXPECTS(!empty());
    return data()[0];
  }

  [[nodiscard]] constexpr auto
  back() const -> const_reference
  {
    MTP_EXPECTS(!empty());
    return data()[size() - 1];
  }

  [[nodiscard]] constexpr auto
  data() const noexcept -> const_pointer
  {
    return _data;
  }

  [[nodiscard]] constexpr auto
  c_str() const noexcept -> const_pointer
  {
    return _data;
  }

  [[nodiscard]] constexpr auto
  view() const noexcept -> std::basic_string_view<CharT>
  {
    return std::basic_string_view<CharT>{ data(), size() };
  }

  [[nodiscard]] constexpr
  operator std::basic_string_view<CharT>() const noexcept
  {
    return view();
  }

  // -----------------------------------------------------------------------------------------------
  // modifiers
  // -----------------------------------------------------------------------------------------------

  constexpr auto
  swap(basic_fixed_string& fs) noexcept -> void
  {
    std::swap_ranges(_data, _data + size(), fs._data);
  }

  // -----------------------------------------------------------------------------------------------
  // string operators
  // -----------------------------------------------------------------------------------------------

  template <std::size_t N2>
  [[nodiscard]] friend constexpr auto
  operator+(basic_fixed_string<CharT, N> const& lhs,
            basic_fixed_string<CharT, N2> const& rhs) noexcept -> basic_fixed_string<CharT, N + N2>
  {
    CharT str[N + N2];
    auto it = str;
    for (auto c : lhs) {
      *(it++) = c;
    }
    for (auto c : rhs) {
      *(it++) = c;
    }
    return { str, it };
  }

  [[nodiscard]] friend constexpr auto
  operator+(basic_fixed_string<CharT, N> const& lhs, CharT rhs) noexcept
      -> basic_fixed_string<CharT, N + 1>
  {
    CharT str[N + 1];
    auto it = str;
    for (auto c : lhs) {
      *(it++) = c;
    }
    *(it++) = rhs;
    return { str, it };
  }

  [[nodiscard]] friend constexpr auto
  operator+(CharT lhs, basic_fixed_string<CharT, N> const& rhs) noexcept
      -> basic_fixed_string<CharT, 1 + N>
  {
    CharT str[N + 1];
    auto it = str;
    *(it++) = lhs;
    for (auto c : rhs) {
      *(it++) = c;
    }
    return { str, it };
  }

  template <std::size_t N2>
  [[nodiscard]] friend consteval auto
  operator+(basic_fixed_string<CharT, N> const& lhs, CharT const (&rhs)[N2]) noexcept
      -> basic_fixed_string<CharT, N + (N2 - 1)>
  {
    MTP_EXPECTS(rhs[N2 - 1] == CharT{});
    CharT str[N + (N2 - 1)];
    auto it = str;
    for (auto c : lhs) {
      *(it++) = c;
    }
    for (auto i = 0u; i < N2 - 1; ++i) {
      *(it++) = rhs[i];
    }
    return { str, it };
  }

  template <std::size_t N2>
  [[nodiscard]] friend consteval auto
  operator+(CharT const (&lhs)[N2], basic_fixed_string<CharT, N> const& rhs) noexcept
      -> basic_fixed_string<CharT, (N2 - 1) + N>
  {
    MTP_EXPECTS(lhs[N2 - 1] == CharT{});
    CharT str[(N2 - 1) + N];
    auto it = str;
    for (auto i = 0u; i < N2 - 1; ++i) {
      *(it++) = lhs[i];
    }
    for (auto c : rhs) {
      *(it++) = c;
    }
    return { str, it };
  }

  // -----------------------------------------------------------------------------------------------
  // comparison operators
  // -----------------------------------------------------------------------------------------------

  template <std::size_t N2>
  [[nodiscard]] friend constexpr auto
  operator==(basic_fixed_string const& lhs, basic_fixed_string<CharT, N2> const& rhs) noexcept
      -> bool
  {
    return lhs.view() == rhs.view();
  }

  template <std::size_t N2>
  [[nodiscard]] friend consteval auto
  operator==(basic_fixed_string const& lhs, CharT const (&rhs)[N2]) noexcept -> bool
  {
    MTP_EXPECTS(rhs[N2 - 1] == CharT{});
    return lhs.view() == std::basic_string_view<CharT>{ rhs, N2 - 1 };
  }

#if defined(__cpp_lib_three_way_comparison) && defined(__cpp_impl_three_way_comparison)
  template <std::size_t N2>
  [[nodiscard]] friend constexpr auto
  operator<=>(basic_fixed_string const& lhs, basic_fixed_string<CharT, N2> const& rhs) noexcept
      -> std::strong_ordering
  {
    return lhs.view() <=> rhs.view();
  }

  template <std::size_t N2>
  [[nodiscard]] friend consteval auto
  operator<=>(basic_fixed_string const& lhs, CharT const (&rhs)[N2]) noexcept
      -> std::strong_ordering
  {
    MTP_EXPECTS(rhs[N2 - 1] == CharT{});
    return lhs.view() <=> std::basic_string_view<CharT>{ rhs, N2 - 1 };
  }
#endif

  // -----------------------------------------------------------------------------------------------
  // input/output
  // -----------------------------------------------------------------------------------------------

  friend auto
  operator<<(std::basic_ostream<CharT>& os, basic_fixed_string const& fs) noexcept
      -> std::basic_ostream<CharT>&
  {
    return os << fs.view();
  }
};

// -------------------------------------------------------------------------------------------------
// concepts
// -------------------------------------------------------------------------------------------------

namespace concepts {

template <typename T, typename... Ts>
concept any_of = (false || ... || std::is_same_v<T, Ts>);

template <typename T>
concept char_type =
#ifdef __cpp_char8_t
    any_of<T, char, wchar_t, char8_t, char16_t, char32_t>;
#else
    any_of<T, char, wchar_t, char16_t, char32_t>;
#endif

} // namespace concepts

// -------------------------------------------------------------------------------------------------
// deduction guides
// -------------------------------------------------------------------------------------------------

template <concepts::char_type CharT, typename... CharTs>
  requires(true && ... && std::convertible_to<CharTs, CharT>)
basic_fixed_string(CharT, CharTs...) -> basic_fixed_string<CharT, 1 + sizeof...(CharTs)>;

template <concepts::char_type CharT, std::size_t N>
basic_fixed_string(CharT const (&)[N]) -> basic_fixed_string<CharT, N - 1>;

#if defined(__cpp_lib_containers_ranges) || defined(__cpp_lib_ranges_to_container)
template <concepts::char_type CharT, std::size_t N>
basic_fixed_string(std::from_range_t, std::array<CharT, N>) -> basic_fixed_string<CharT, N>;
#endif

// -------------------------------------------------------------------------------------------------
// non-member functions
// -------------------------------------------------------------------------------------------------

template <typename CharT, std::size_t N>
constexpr auto
swap(basic_fixed_string<CharT, N>& a, basic_fixed_string<CharT, N>& b) noexcept -> void
{
  return a.swap(b);
}

} // namespace mtp

// -------------------------------------------------------------------------------------------------
// hashing support
// -------------------------------------------------------------------------------------------------

template <std::size_t N>
struct std::hash<mtp::fixed_string<N>> : std::hash<std::string_view>
{};

template <std::size_t N>
struct std::hash<mtp::fixed_wstring<N>> : std::hash<std::wstring_view>
{};

#ifdef __cpp_char8_t
template <std::size_t N>
struct std::hash<mtp::fixed_u8string<N>> : std::hash<std::u8string_view>
{};
#endif

template <std::size_t N>
struct std::hash<mtp::fixed_u16string<N>> : std::hash<std::u16string_view>
{};

template <std::size_t N>
struct std::hash<mtp::fixed_u32string<N>> : std::hash<std::u32string_view>
{};

// -------------------------------------------------------------------------------------------------
// formatting support
// -------------------------------------------------------------------------------------------------

#ifdef __cpp_lib_format
template <typename CharT, std::size_t N>
struct std::formatter<mtp::basic_fixed_string<CharT, N>> : formatter<std::basic_string_view<CharT>>
{
  template <typename FmtCtx>
  auto
  format(mtp::basic_fixed_string<CharT, N> const& str, FmtCtx& ctx) const -> decltype(ctx.out())
  {
    return formatter<std::basic_string_view<CharT>>::format(std::basic_string_view<CharT>(str),
                                                            ctx);
  }
};
#endif

#endif // MTP_FIXED_STRING_HPP
