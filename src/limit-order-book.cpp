#include "limit-order-book.hpp"
#include "memory-pool.hpp"

LimitOrderBook::LimitOrderBook(MemoryPool *memory_pool)
    : bids_{Map{std::less<Price>{}, AllocType{*memory_pool}}},
      asks_{Map{std::less<Price>{}, AllocType{*memory_pool}}} {
  // NOTE: change to the default allocator later to test
  //       the execution improvement
}

template <Side ORDER_SIDE>
void LimitOrderBook::modify_level(const Order &order) {
  Map &cur_map = (ORDER_SIDE == Side::BUY) ? bids_ : asks_;

  const auto &[price_level, new_volume] = order;

  if (new_volume == 0) {
    // delete the price level
    auto it = cur_map.find(price_level);
    if (it != cur_map.end()) {
      cur_map.erase(it);
    }
  } else {
    // inserts the price level if not present
    auto cur_it = cur_map.try_emplace(price_level).first;

    // overrides the volume, if the price level already existed
    cur_it->second = new_volume;
  }
}
