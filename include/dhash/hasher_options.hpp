#pragma once

#include <bit>
#include <concepts>
#include <ranges>

#include <dhash/impl/utils.hpp>

namespace dhash {

namespace hasher_options {  // TODO: separate files

namespace __impl {

template<std::endian endian>
struct hash_integer_as_bytes_t {
  template<std::integral I, typename Hasher>
  void operator()(I i, Hasher&& hasher) {
    if constexpr (endian == std::endian::native) {
      hasher(std::string_view{reinterpret_cast<const char*>(&i), sizeof(i)});
    } else {
      static_assert(::dhash::__impl::always_false_v<Hasher>, "TODO: support other endians");
    }
  }
};

struct hash_range_as_items_t {
  template<std::ranges::range Range, typename Hasher>
  void operator()(Range range, Hasher&& hasher) {
    for (auto& item : range) {
      hasher(item);
    }
  }
};

}  // namespace __impl

template<std::endian endian>
constexpr inline __impl::hash_integer_as_bytes_t<endian> hash_integer_as_bytes{};

constexpr inline __impl::hash_range_as_items_t hash_range_as_items{};

// TODO: implement other usefull options
// - pair/tuple as items

};  // namespace hasher_options


}  // namespace dhash
