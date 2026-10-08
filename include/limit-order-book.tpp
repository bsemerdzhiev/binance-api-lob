#include "limit-order-book.hpp"

template <typename Alloc>
template <Side ORDER_SIDE>
void LimitOrderBook<Alloc>::modify_level(const Order &order) {
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

template <typename Alloc>
LimitOrderBook<Alloc>::LimitOrderBook(Alloc &alloc_obj)
    : bids_{Map{std::less<Price>{}, alloc_obj}},
      asks_{Map{std::less<Price>{}, alloc_obj}} {}

template <typename T>
std::vector<Order> LimitOrderBook<T>::get_all_levels(const Side side) const {
  if (side == Side::BUY) {
    return std::vector<Order>(bids_.begin(), bids_.end());
  } else {
    return std::vector<Order>(asks_.begin(), asks_.end());
  }
}
