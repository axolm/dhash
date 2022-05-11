#include <catch2/catch.hpp>
#include <dhash/dhash.hpp>
#include <iostream>

struct VectorWithStrangeHash : public std::vector<std::string> {
  using std::vector<std::string>::vector;

  template<typename Hasher>
  void hash(Hasher& hasher) {
    auto copy = *this;
    std::sort(copy.begin(), copy.end());
    for (auto& s : copy) {
      hasher(s);
    }
  }
};

TEST_CASE("VectorWithStrangeHash") {
  auto calc_vector_hash = [](auto v) {
    dhash::hasher hasher(dhash::algorithms::sha512{});
    hasher(v);
    return std::move(hasher).result();
  };
  REQUIRE(calc_vector_hash(VectorWithStrangeHash{"a", "b", "c"}) ==
          calc_vector_hash(VectorWithStrangeHash{"c", "b", "a"}));
}

struct OnlyUints {
  uint64_t x1;
  uint64_t x2;
  using hash = dhash::default_hash;
};


TEST_CASE("BoostHashImpl") {
  auto only_uints_hash = [] {
    dhash::hasher hasher(dhash::algorithms::boost_hash{});
    hasher(OnlyUints{.x1 = 228, .x2 = 42});
    return std::move(hasher).result();
  }();
  auto pair_hash = [] {
    return boost::hash_value(std::pair<uint64_t, uint64_t>{228, 42});
  }();
  REQUIRE(only_uints_hash == pair_hash);
}

namespace std {
template<typename T>
requires(!requires { std::hash<T>{}; }) struct hash<T> {
  size_t operator()(const T& data) const {
    dhash::hasher hasher(dhash::algorithms::boost_hash{});
    hasher(data);
    return std::move(hasher).result();
  }
};
}  // namespace std

struct MyStruct {
  uint64_t x1;
  uint64_t x2;
  std::string s;
  using hash = dhash::default_hash;
  bool operator==(const MyStruct& rhs) const = default;
};

TEST_CASE("BoostHashExtendedHashMap") {
  std::unordered_map<MyStruct, std::string> map;
  map[{1, 2, ""}] = "abacaba";
  map[{2, 3, ""}] = "abacaba";
  REQUIRE(map.size() == 2);
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

TEST_CASE("CustomXorHashAlgorithm") {
  uint64_t xor_hash_result = 0;
  {
    xor_hash_algorithm algorithm;
    dhash::hasher hasher(std::move(algorithm));
    hasher(OnlyUints{.x1 = 228, .x2 = 231});
    xor_hash_result = std::move(hasher).result();
    REQUIRE(xor_hash_result == 3);
  }

  {
    auto xor_hash_algorithm = xor_hash_algorithm::from_previous_result(xor_hash_result);
    dhash::hasher hasher(std::move(xor_hash_algorithm));
    hasher(OnlyUints{.x1 = 10, .x2 = 18});
    REQUIRE(std::move(hasher).result() == 27);
  }
}


TEST_CASE("Sha512Algorithm") {
  auto digest_to_hex_str = [](const auto& digest) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto ch : digest) {
      ss << std::setw(2) << int(ch);
    }
    return std::move(ss).str();
  };

  {
    dhash::hasher hasher(dhash::algorithms::sha512{});
    hasher("abacaba");
    REQUIRE(digest_to_hex_str(std::move(hasher).result()) ==
            "224efa649e799be3bc7f8fe49955380a2416cc423ed14d7fb2393bb7695c6fb6"
            "35bd83727ddbaf23ed59ae0a82b1528a0eb703204c0f0d38814243bfba4b9f18");
  }

  dhash::hasher hasher(dhash::algorithms::sha512{},
                       dhash::hasher_options::hash_integer_as_bytes<std::endian::native>,
                       dhash::hasher_options::hash_range_as_items);
  hasher("abacaba");
  hasher(OnlyUints{.x1 = 1, .x2 = 2});
  hasher(std::array<bool, 3>{true, false, true});
  hasher(std::vector<uint32_t>{1, 2, 3, 4, 5});

  REQUIRE(digest_to_hex_str(std::move(hasher).result()) ==
          "30099592f395b61b1c4cb0adf141d0275253d557fc973b4506734f9fc5f6e4ae"
          "0d5a8ce399ea505113fecf274fc0c17794076e10669a200a07717a0d198ea09c");
}
