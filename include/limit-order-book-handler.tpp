#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include "memory-pool.hpp"
#include "symbol-repo.hpp"

template <typename Alloc>
LimitOrderBookHandler<Alloc>::LimitOrderBookHandler()
    : memory_pool_(MemoryPool{ARENA_SIZE}),
      alloc_obj_(make_allocator(memory_pool_)) {

  order_books_.resize(symbol_repo.size(), LimitOrderBook{alloc_obj_});
}

template <typename Alloc>
void LimitOrderBookHandler<Alloc>::update_symbol(const SymbolId symbol_id,
                                                 const Side side,
                                                 const Order &order) {
  assert(symbol_id < order_books_.size());

  LimitOrderBook<Alloc> &order_book = order_books_[symbol_id];

  if (side == Side::BUY) {
    order_book.template modify_level<Side::BUY>(order);
  } else {
    order_book.template modify_level<Side::SELL>(order);
  }
}

template <typename Alloc> void LimitOrderBookHandler<Alloc>::warm_up_insert() {
  for (LimitOrderBook<Alloc> &order_book : order_books_) {
    order_book.warm_up();
  }
}

template <typename Alloc>
std::vector<Order>
LimitOrderBookHandler<Alloc>::get_all_levels(const SymbolId symbol_id,
                                             const Side side) const {
  assert(symbol_id < order_books_.size());

  const LimitOrderBook<Alloc> &order_book = order_books_[symbol_id];

  return order_book.get_all_levels(side);
}

template <typename Alloc> void LimitOrderBookHandler<Alloc>::reset() {
  order_books_.clear();
  order_books_.resize(symbol_repo.size(), LimitOrderBook{alloc_obj_});
}

template <typename Alloc>
std::optional<Price>
LimitOrderBookHandler<Alloc>::get_best_bid(const SymbolId symbol_id) const {
  assert(symbol_id < order_books_.size());

  const LimitOrderBook<Alloc> &order_book = order_books_[symbol_id];
  return order_book.get_best_bid();
}

template <typename Alloc>
std::optional<Price>
LimitOrderBookHandler<Alloc>::get_best_ask(const SymbolId symbol_id) const {
  assert(symbol_id < order_books_.size());

  const LimitOrderBook<Alloc> &order_book = order_books_[symbol_id];
  return order_book.get_best_ask();
}
