#pragma once

// TODO: fix includes
#include <bit>
#include <concepts>
#include <iostream>
#include <string_view>
#include <type_traits>

#include <boost/functional/hash.hpp>
#include <boost/pfr.hpp>

#include <openssl/sha.h>
#include <ranges>


namespace dhash {
struct default_hash {};

// TODO: separate files

// TODO: hasher options.
// - std containers
// - ints/string to bytes
// - endians ?

namespace __impl {
template<typename...>
inline constexpr bool false_v = false;

template<typename Hasher, typename Data>
void call_from_options(Hasher& /* hasher */, Data&& /* data */) {
  static_assert(false_v<Hasher>, "hash calculator not found");
}

template<typename Hasher, typename Data, typename Option, typename... Options>
void call_from_options(Hasher& hasher, Data&& data) {
  if constexpr (requires { Option{}(std::forward<Data>(data), hasher); }) {
    Option{}(std::forward<Data>(data), hasher);
  } else {  // TODO: error, not try
    call_from_options<Hasher, Data, Options...>(hasher, std::forward<Data>(data));
  }
}
}  // namespace __impl

template<typename HashAlgorightm, typename... Options>
class hasher {
public:
  explicit hasher(HashAlgorightm&& hash_algorithm, const Options&... /* options */):
      hash_algorithm_(std::forward<HashAlgorightm>(hash_algorithm)) {
  }

  auto result() && {
    return std::move(hash_algorithm_).result();
  }

  template<typename Data>
  void operator()(Data&& data) {
    if constexpr (requires { hash_algorithm_.append(std::forward<Data>(data)); }) {
      hash_algorithm_.append(std::forward<Data>(data));
    } else if constexpr (requires { requires std::same_as<typename std::decay_t<Data>::hash, default_hash>; }) {
      boost::pfr::for_each_field(std::forward<Data>(data), [this](auto& field) { operator()(field); });
    } else if constexpr (requires { std::forward<Data>(data).hash(*this); }) {  // TODO: else if free func
      std::forward<Data>(data).hash(*this);
    } else {
      __impl::call_from_options<hasher, Data, Options...>(*this, std::forward<Data>(data));
    }
  }

private:
  HashAlgorightm hash_algorithm_;
};

template<typename Hasher, typename Data>
auto calculate_hash(Hasher hasher, Data&& data) {
  hasher(std::forward<Data>(data));
  return std::move(hasher).result();
}

class xor_hash_algorithm {
public:
  using result_t = uint64_t;

  xor_hash_algorithm() = default;

  static xor_hash_algorithm from_previous_result(result_t state) {
    return xor_hash_algorithm(state);
  }

  void append(uint64_t x) {
    state_ ^= x;
  }

  result_t result() && {
    return state_;
  }

private:
  explicit xor_hash_algorithm(result_t state): state_(state) {
  }

  result_t state_{0};
};

template<typename Data>
auto xor_hash(Data&& data) {
  xor_hash_algorithm algorithm;
  hasher hasher(std::move(algorithm));
  hasher(std::forward<Data>(data));
  return std::move(hasher).result();
}

class boost_hash_algorithm {
public:
  using result_t = size_t;

  template<typename T>
  void append(T&& x) requires(requires { boost::hash_value(x); }) {
    boost::hash_combine(state_, boost::hash_value(x));
  }

  result_t result() && {
    return state_;
  }

private:
  result_t state_{0};
};

namespace algorithms {
class sha512 {
public:
  // TODO: separate class `digest` with conversion to hex methods ?
  using result_t = std::array<unsigned char, SHA512_DIGEST_LENGTH>;

  void append(std::string_view bytes) {
    SHA512_Update(&ctx_, bytes.data(), bytes.size());
  }

  result_t result() && {
    result_t res;
    SHA512_Final(res.data(), &ctx_);
    return res;
  }

private:
  SHA512_CTX ctx_ = [] {
    SHA512_CTX ctx;
    SHA512_Init(&ctx);
    return ctx;
  }();
};
}  // namespace algorithms

namespace hasher_options {

namespace __impl {

template<std::endian endian>
struct hash_integers_as_bytes_t {
  template<std::integral I, typename Hasher>
  void operator()(I i, Hasher&& hasher) {
    if constexpr (endian == std::endian::native) {
      hasher(std::string_view{reinterpret_cast<const char*>(&i), sizeof(i)});
    } else {
      static_assert(::dhash::__impl::false_v<Hasher>, "TODO: support other endians");
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
constexpr inline __impl::hash_integers_as_bytes_t<endian> hash_integers_as_bytes{};

constexpr inline __impl::hash_range_as_items_t hash_range_as_items{};

// TODO: other options

};  // namespace hasher_options

}  // namespace dhash
