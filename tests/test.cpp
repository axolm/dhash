#include <bits/stdc++.h>
#include <dhash/dhash.hpp>
using namespace std;

// TODO: separate files, catch2

struct MyStruct {
  int x;
  std::string s;

  template<typename Hasher>
  void hash(Hasher& hasher) {
    hasher(x);
    hasher(s);
  }
};

struct MyStruct2 {
  int x;
  std::string s;

  using hash = dhash::default_hash;
};

struct MyStruct3 : public MyStruct {
  MyStruct2 m;

  template<typename Hasher>
  void hash(Hasher& hasher) {
    MyStruct::hash(hasher);
    hasher(m);
  }
};

struct OnlyUints {
  uint64_t x1;
  uint64_t x2;

  using hash = dhash::default_hash;
};

struct MyStruct4 {
  uint64_t x1;
  uint64_t x2;
  std::string s;

  using hash = dhash::default_hash;
  bool operator==(const MyStruct4& rhs) const = default;
};

namespace std {
template<typename T>
requires(!requires { std::hash<T>{}; }) struct hash<T> {
  size_t operator()(const T& data) const {
    dhash::hasher hasher(dhash::boost_hash_algorithm{});
    hasher(data);
    return std::move(hasher).result();
  }
};
}  // namespace std

int main() {
  MyStruct m1{.x = 1, .s = "afsa"};
  MyStruct2 m2{.x = 2, .s = "abacaba"};
  MyStruct3 m3{MyStruct{.x = 1, .s = "2"}, MyStruct2{.x = 2, .s = "abacaba"}};


  OnlyUints uints{.x1 = 228, .x2 = 231};
  auto xor_hash_result = dhash::xor_hash(uints);
  std::cout << "xor hash #1: " << xor_hash_result << '\n';

  {
    OnlyUints more_uints{.x1 = 10, .x2 = 18};
    auto xor_hash_algorithm = dhash::xor_hash_algorithm::from_previous_result(xor_hash_result);
    dhash::hasher hasher(std::move(xor_hash_algorithm));
    hasher(more_uints);
    std::cout << "xor hash #2: " << std::move(hasher).result() << '\n';
  }


  {
    dhash::hasher hasher(dhash::boost_hash_algorithm{});
    hasher(uints);
    std::cout << "boost hash: " << std::move(hasher).result() << '\n';
  }

  {
    std::unordered_map<MyStruct4, std::string> map;
    map[{1, 2, ""}] = "abacaba";
    map[{2, 3, ""}] = "abacaba";
    std::cout << "map size: " << map.size() << '\n';
  }

  // 1. integers to bytes. only constant size
  // 2. stl containers

  {
    dhash::hasher hasher(dhash::algorithms::sha512{},
                         dhash::hasher_options::hash_integers_as_bytes<std::endian::native>,
                         dhash::hasher_options::hash_range_as_items);
    std::string_view sv = "abacaba";
    hasher(sv);
    hasher(uints);
    hasher(true);
    hasher(std::vector<int>{1, 2, 3});
    hasher(std::vector<std::string>{"abacaba"});

    auto sha512_digest = std::move(hasher).result();
    std::cout << "sha512 digest:\n" << std::hex << std::setfill('0');
    for (auto ch : sha512_digest) {
      std::cout << std::setw(2) << int(ch);
    }
    std::cout << std::endl;
    // TODO: stream in test
  }

  return 0;
}
