#include "limit-order-book.hpp"
#include "memory-pool.hpp"

LimitOrderBook::LimitOrderBook(MemoryPool *memory_pool)
    : bids_{Map{std::less<Price>{}, AllocType{*memory_pool}}},
      asks_{Map{std::less<Price>{}, AllocType{*memory_pool}}} {
  // NOTE: change to the default allocator later to test
  //       the execution improvement
}

std::vector<Order> LimitOrderBook::get_all_levels(const Side side) const {
  if (side == Side::BUY) {
    return std::vector<Order>(bids_.begin(), bids_.end());
  } else {
    return std::vector<Order>(asks_.begin(), asks_.end());
  }
}
