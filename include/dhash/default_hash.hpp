#pragma once

namespace dhash {

// make `using hash = dhash::default_hash;` in your class
// for generate default hash calculation method using reflection (boost::pfr for now)
struct default_hash {};

}  // namespace dhash
