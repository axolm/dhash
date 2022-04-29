#pragma once

#include <iostream>  // TODO
#include <type_traits>

#include <boost/pfr.hpp>

namespace twab::hash {

struct use_pfr {};

// TODO: hasher options.
// - std containers
// - ints/string to bytes
// - endians ?

template<typename HashAlgorightm>
class hasher {
public:
  explicit hasher(HashAlgorightm&& hash_algorithm): hash_algorithm_(std::forward<HashAlgorightm>(hash_algorithm)) {
  }

  auto result() && {
    return std::move(hash_algorithm_).result();
  }

  template<typename Data>
  void operator()(Data&& data) {
    if constexpr (requires { hash_algorithm_.append(std::forward<Data>(data)); }) {
      hash_algorithm_.append(std::forward<Data>(data));
    } else if constexpr (requires { requires std::same_as<typename std::decay_t<Data>::hash, use_pfr>; }) {
      boost::pfr::for_each_field(std::forward<Data>(data), [this](auto& field) { operator()(field); });
    } else {  // TODO: else if free func
      std::forward<Data>(data).hash(*this);
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

  void append(uint64_t x) {
    state_ ^= x;
  }

  static xor_hash_algorithm from_previous_result(result_t state) {
    return xor_hash_algorithm(state);
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
auto xor_hash(Data&& data, xor_hash_algorithm algorithm = {}) {
  hasher hasher(std::move(algorithm));
  hasher(std::forward<Data>(data));
  return std::move(hasher).result();
}

}  // namespace twab::hash
