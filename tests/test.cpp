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

int main() {
  MyStruct m1{.x = 1, .s = "afsa"};
  MyStruct2 m2{.x = 2, .s = "abacaba"};
  MyStruct3 m3{MyStruct{.x = 1, .s = "2"}, MyStruct2{.x = 2, .s = "abacaba"}};

  std::cout << twab::hash::md5(m1) << '\n';
  std::cout << "---\n";
  std::cout << twab::hash::md5(m2) << '\n';
  std::cout << "---\n";
  std::cout << twab::hash::md5(m3) << '\n';
  std::cout << "---\n";

  return 0;
}
