#pragma once

#include "limit-order-book.hpp"
#include "memory-pool.hpp"
#include "symbol-repo.hpp"

class LimitOrderBookHandler {
public:
  LimitOrderBookHandler();

  LimitOrderBookHandler(const LimitOrderBookHandler &rhs);
  LimitOrderBookHandler &operator=(const LimitOrderBookHandler &rhs);

  LimitOrderBookHandler(LimitOrderBookHandler &&rhs);
  LimitOrderBookHandler &operator=(LimitOrderBookHandler &&rhs);

  ~LimitOrderBookHandler();

  void update_symbol(const SymbolId &symbol_id, const Side side,
                     const Order &order);

private:
  //                                             1M allocations
  static inline constexpr std::size_t ARENA_SIZE = 1000 * 1000;
  std::vector<LimitOrderBook> order_books_;
  MemoryPool *memory_pool_;
};

inline LimitOrderBookHandler order_book_handler;
