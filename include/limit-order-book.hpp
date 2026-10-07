#pragma once

#include "free-list-allocator.hpp"
#include "memory-pool.hpp"
#include <cstdint>
#include <map>
#include <utility>
#include <vector>

using Price = uint64_t;
using Volume = uint64_t;

using AllocType = FreeListAllocator<std::pair<const Price, Volume>>;
using Map = std::map<Price, Volume, std::less<Price>, AllocType>;

using Order = std::pair<Price, Volume>;

enum class Side {
  BUY,
  SELL,
};

struct Snapshot {
  std::vector<Order> bids, asks;
};

class LimitOrderBook {
public:
  LimitOrderBook(MemoryPool *memory_pool);
  template <Side ORDER_SIDE> void modify_level(const Order &order);

private:
  Map bids_, asks_;
};
