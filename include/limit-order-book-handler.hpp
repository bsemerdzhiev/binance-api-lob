#pragma once

#include "limit-order-book.hpp"
#include "memory-pool.hpp"
#include "symbol-repo.hpp"
#include <vector>

class LimitOrderBookHandler {
public:
  LimitOrderBookHandler();

  void update_symbol(const SymbolId symbol_id, const Side side,
                     const Order &order);

  std::vector<Order> get_all_levels(const SymbolId symbol_id,
                                    const Side side) const;

  void reset();

private:
  //                                             1M allocations
  static inline constexpr std::size_t ARENA_SIZE = 1000 * 1000;
  MemoryPool memory_pool_;
  std::vector<LimitOrderBook> order_books_;
};

inline LimitOrderBookHandler order_book_handler;
