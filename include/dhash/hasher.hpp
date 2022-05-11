#pragma once

#include <boost/pfr.hpp>
#include <type_traits>
#include <utility>

#include <dhash/default_hash.hpp>
#include <dhash/impl/utils.hpp>

namespace dhash {

namespace __impl {

template<typename Hasher, typename Data>
void call_from_options(Hasher& /* hasher */, Data&& /* data */) {
  static_assert(always_false_v<Hasher>, "hash calculator not found");
}

template<typename Hasher, typename Data, typename Option, typename... Options>
void call_from_options(Hasher& hasher, Data&& data) {
  if constexpr (requires { Option{}(std::forward<Data>(data), hasher); }) {
    Option{}(std::forward<Data>(data), hasher);
    // TODO: error if can call other option
  } else {
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

}  // namespace dhash
