#pragma once

#include "free-list-allocator.hpp"
#include "limit-order-book.hpp"
#include "memory-pool.hpp"
#include "symbol-repo.hpp"
#include <vector>

template <typename Alloc> class LimitOrderBookHandler {
public:
  LimitOrderBookHandler();

  void update_symbol(const SymbolId symbol_id, const Side side,
                     const Order &order);

  std::vector<Order> get_all_levels(const SymbolId symbol_id,
                                    const Side side) const;

  void reset();

  // needed in order to create the free list nodes inside the custom allocator
  void warm_up_insert();

  std::optional<Price> get_best_bid(const SymbolId symbol_id) const;
  std::optional<Price> get_best_ask(const SymbolId symbol_id) const;

private:
  //                                              10M allocations
  static inline constexpr std::size_t ARENA_SIZE = 10 * 1000 * 1000;

  static Alloc make_allocator(MemoryPool &pool) {
    if constexpr (std::constructible_from<Alloc, MemoryPool &>) {
      return Alloc{pool};
    } else {
      return Alloc{};
    }
  }

  MemoryPool memory_pool_;
  Alloc alloc_obj_;
  std::vector<LimitOrderBook<Alloc>> order_books_;
};

// initialized with the custom allocator
inline LimitOrderBookHandler<FreeListAllocator<std::pair<const Price, Volume>>>
    order_book_handler;

#include "limit-order-book-handler.tpp"
