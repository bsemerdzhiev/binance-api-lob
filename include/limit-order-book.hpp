#pragma once

#include "free-list-allocator.hpp"
#include <cstdint>
#include <map>

using Price = uint64_t;
using Volume = uint64_t;

class LimitOrderBook {
public:
  void modify_level() {}

private:
  std::map<Price, Volume, std::less<Price>,
           FreeListAllocator<std::pair<const Price, Volume>>>
      bids_, asks_;
};
