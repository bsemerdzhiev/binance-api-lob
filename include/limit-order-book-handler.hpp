#pragma once

#include "limit-order-book.hpp"
#include "memory-pool.hpp"
#include "symbol-repo.hpp"

class LimitOrderBookHandler {
public:
  LimitOrderBookHandler();
  ~LimitOrderBookHandler();

  void update_symbol(const Symbol &symbol, const Order &order);

private:
  std::vector<LimitOrderBook> order_books_;
  MemoryPool *memory_pool_;
};

inline LimitOrderBookHandler order_boook_handler;
