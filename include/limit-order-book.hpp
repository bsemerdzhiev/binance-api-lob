#pragma once

#include <cstdint>
#include <map>

using Price = uint64_t;
using Volume = uint64_t;

class LimitOrderBook {
public:
private:
  std::map<Price, Volume> bids_, asks_;
};
