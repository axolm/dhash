#pragma once

#include <openssl/sha.h>
#include <array>
#include <string_view>

namespace dhash::algorithms {
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

}  // namespace dhash::algorithms
