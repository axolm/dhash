#pragma once

namespace dhash {

namespace __impl {
template<typename...>
inline constexpr bool always_false_v = false;
}

}  // namespace dhash
