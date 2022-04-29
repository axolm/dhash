#include <bits/stdc++.h>
#include <twab/twab.hpp>
using namespace std;

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

  using hash = twab::hash::use_pfr;
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

  using hash = twab::hash::use_pfr;
};

int main() {
  MyStruct m1{.x = 1, .s = "afsa"};
  MyStruct2 m2{.x = 2, .s = "abacaba"};
  MyStruct3 m3{MyStruct{.x = 1, .s = "2"}, MyStruct2{.x = 2, .s = "abacaba"}};


  OnlyUints uints{.x1 = 228, .x2 = 231};
  auto xor_hash_result = twab::hash::xor_hash(uints);
  std::cout << "xor hash #1: " << xor_hash_result << '\n';
  auto xor_hash_algorithm = twab::hash::xor_hash_algorithm::from_previous_result(xor_hash_result);
  twab::hash::hasher hasher(std::move(xor_hash_algorithm));
  OnlyUints more_uints{.x1 = 10, .x2 = 18};
  hasher(more_uints);
  std::cout << "xor hash #2: " << std::move(hasher).result() << '\n';


  // std::cout << twab::hash::md5(m1) << '\n';
  // std::cout << "---\n";
  // std::cout << twab::hash::md5(m2) << '\n';
  // std::cout << "---\n";
  // std::cout << twab::hash::md5(m3) << '\n';
  // std::cout << "---\n";

  return 0;
}
