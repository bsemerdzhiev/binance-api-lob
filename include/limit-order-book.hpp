#pragma once

#include <cstdint>
#include <map>
#include <utility>
#include <vector>

using Price = uint64_t;
using Volume = uint64_t;

using Order = std::pair<Price, Volume>;

enum class Side {
  BUY,
  SELL,
};

struct Snapshot {
  std::vector<Order> bids, asks;
};

template <typename Alloc> class LimitOrderBook {
public:
  using Map = std::map<Price, Volume, std::less<Price>, Alloc>;

  LimitOrderBook(Alloc &alloc_obj);
  template <Side ORDER_SIDE> void modify_level(const Order &order);

  std::vector<Order> get_all_levels(const Side side) const;

private:
  Map bids_, asks_;
};

#include "limit-order-book.tpp"
