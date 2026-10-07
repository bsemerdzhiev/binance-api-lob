#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include "memory-pool.hpp"
#include "symbol-repo.hpp"

LimitOrderBookHandler::LimitOrderBookHandler()
    : memory_pool_(MemoryPool{ARENA_SIZE}) {
  order_books_.resize(symbol_repo.size(), LimitOrderBook{&memory_pool_});
}

void LimitOrderBookHandler::update_symbol(const SymbolId symbol_id,
                                          const Side side, const Order &order) {
  assert(symbol_id < order_books_.size());

  LimitOrderBook &order_book = order_books_[symbol_id];

  if (side == Side::BUY) {
    order_book.modify_level<Side::BUY>(order);
  } else {
    order_book.modify_level<Side::SELL>(order);
  }
}

std::vector<Order>
LimitOrderBookHandler::get_all_levels(const SymbolId symbol_id,
                                      const Side side) const {
  assert(symbol_id < order_books_.size());

  const LimitOrderBook &order_book = order_books_[symbol_id];

  return order_book.get_all_levels(side);
}

void LimitOrderBookHandler::reset() {
  order_books_.clear();
  order_books_.resize(symbol_repo.size(), LimitOrderBook{&memory_pool_});
}
