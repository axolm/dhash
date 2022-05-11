#pragma once

#include <boost/functional/hash.hpp>
#include <cstdint>

namespace dhash::algorithms {

class boost_hash {
public:
  // TODO: with seed constructor

  using result_t = size_t;

  template<typename T>
  void append(T&& x) requires(requires { boost::hash_value(x); }) {
    boost::hash_combine(state_, boost::hash_value(x));
  }

  result_t result() && {
    return state_;
  }

private:
  result_t state_{};
};

}  // namespace dhash::algorithms
