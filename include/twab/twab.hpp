#pragma once

#include <iostream>  // TODO
#include <type_traits>

#include <boost/pfr.hpp>

namespace twab::hash {

struct use_pfr {};

namespace __impl {

struct md5_hasher {
  std::string result() {
    return "abacaba";
  }

  void operator()(int x) {
    std::cerr << "int x: " << x << std::endl;
  }

  void operator()(std::string s) {
    std::cerr << "string s: " << s << std::endl;
  }
};

template<typename Hasher, typename Data>
auto calculate_hash(Hasher hasher, Data&& data) {
  if constexpr (requires { requires std::same_as<typename std::decay_t<Data>::hash, use_pfr>; }) {
    boost::pfr::for_each_field(data, [&hasher](auto& field) { hasher(field); });
  } else {
    std::forward<Data>(data).hash(hasher);
  }
  return std::move(hasher).result();
}
}  // namespace __impl

template<typename Data>
auto md5(Data&& data) {
  return __impl::calculate_hash(__impl::md5_hasher{}, std::forward<Data>(data));
}

}  // namespace twab::hash
