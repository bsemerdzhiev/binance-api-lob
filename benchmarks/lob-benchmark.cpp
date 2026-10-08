#include "free-list-allocator.hpp"
#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include "symbol-repo.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <random>
#include <string>
#include <vector>

constexpr std::string SYMBOL_TO_USE = "AB";

using GeneratorType = std::tuple<SymbolId, Side, Order>;

std::vector<GeneratorType> generate_updates(const std::size_t N) {
  std::vector<GeneratorType> updates;

  // pick a random seed
  std::mt19937_64 rng{12};

  const std::size_t SNAPSHOT_ORDERS = N;

  SymbolId symbol_id = symbol_repo.get_symbol_id(SYMBOL_TO_USE);

  for (int32_t i{0}; i < SNAPSHOT_ORDERS; i++) {
    Side current_side = static_cast<Side>(rng() % 2);

    Price price = rng();
    Volume volume = rng();

    updates.push_back({symbol_id, current_side, {price, volume}});
  }

  return updates;
}

static void BM_Custom(benchmark::State &state) {
  LimitOrderBookHandler<FreeListAllocator<std::pair<const Price, Volume>>>
      lob_handler;

  const auto N = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    order_book_handler.reset();
    std::vector<GeneratorType> updates = generate_updates(N);

    state.ResumeTiming();

    for (const auto &[symbol_id, side, order] : updates) {
      lob_handler.update_symbol(symbol_id, side, order);
    }
  }
}

static void BM_Default(benchmark::State &state) {
  LimitOrderBookHandler<std::allocator<std::pair<const Price, Volume>>>
      lob_handler;

  const auto N = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    order_book_handler.reset();
    std::vector<GeneratorType> updates = generate_updates(N);

    state.ResumeTiming();

    for (const auto &[symbol_id, side, order] : updates) {
      lob_handler.update_symbol(symbol_id, side, order);
    }
  }
}

BENCHMARK(BM_Custom)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(1'000'000)
    ->Arg(10'000'000);
BENCHMARK(BM_Default)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(1'000'000)
    ->Arg(10'000'000);

BENCHMARK_MAIN();
