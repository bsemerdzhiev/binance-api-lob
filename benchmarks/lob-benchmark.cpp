#include "free-list-allocator.hpp"
#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include "symbol-repo.hpp"
#include <benchmark/benchmark.h>
#include <memory>
#include <numeric>
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

  std::uniform_int_distribution<int> op_dist(0, 99);

  std::vector<std::vector<int32_t>> free_levels(2);

  for (auto &x : free_levels) {
    x.resize(2'000);
    std::iota(x.begin(), x.end(), 1);
  }

  std::vector<std::vector<int32_t>> used_levels(2);

  for (int32_t i{0}; i < SNAPSHOT_ORDERS; i++) {
    int32_t side = rng() % 2;
    Side current_side = static_cast<Side>(side);

    Price price;

    // decide whether to insert/update/delete

    Volume volume = rng();

    while (true) {
      int32_t op = op_dist(rng);
      if (op < 10) {
        // 10% that it is a delete
        if (used_levels[side].empty()) {
          continue;
        }
        std::size_t idx = rng() % used_levels[side].size();

        price = used_levels[side][idx];

        used_levels[side][idx] = used_levels[side].back();
        used_levels[side].pop_back();
        free_levels[side].push_back(price);

        volume = 0;
      } else if (op < 80) {
        // 80% that it is an update
        if (used_levels[side].empty()) {
          continue;
        }
        std::size_t idx = rng() % used_levels[side].size();

        price = used_levels[side][idx];
        //     chances that rng() will give the same volume as the current level
        //     are astronomically small
        volume = rng();
      } else {
        // 10% that it is an insert
        if (free_levels[side].empty()) {
          continue;
        }

        std::size_t idx = rng() % free_levels[side].size();

        price = free_levels[side][idx];

        free_levels[side][idx] = free_levels[side].back();
        free_levels[side].pop_back();
        used_levels[side].push_back(price);

        volume = rng();
      }
      break;
    }

    updates.push_back({symbol_id, current_side, {price, volume}});
  }

  return updates;
}

static void BM_Custom(benchmark::State &state) {
  LimitOrderBookHandler<FreeListAllocator<std::pair<const Price, Volume>>>
      lob_handler;

  lob_handler.warm_up_insert();

  const auto N = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    lob_handler.reset();
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

  lob_handler.warm_up_insert();

  const auto N = state.range(0);

  for (auto _ : state) {
    state.PauseTiming();

    lob_handler.reset();
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
