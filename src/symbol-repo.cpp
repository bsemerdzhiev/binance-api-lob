#include "symbol-repo.hpp"
#include <stdexcept>

SymbolRepo::SymbolRepo() {
  SymbolId ids{0};

  for (const auto &[symbol, price_scale, volume_scale] : MOCK_ENTRIES) {
    symbol_ids_.emplace(symbol, ids);
    symbol_infos_.push_back(SymbolInfo{price_scale, volume_scale});

    ids++;
  }
}

SymbolId SymbolRepo::get_symbol_id(SymbolView symbol) const {
  // TODO: maybe change to heterogenous look-ups, as now we are creating a new
  // string when searching
  auto it = symbol_ids_.find(Symbol{symbol});

  if (it == symbol_ids_.end()) {
    throw std::runtime_error("Symbol not found");
  }

  return it->second;
}

const SymbolInfo &SymbolRepo::get_symbol_info(SymbolId symbol_id) const {
  assert(symbol_id < symbol_infos_.size());

  return symbol_infos_[symbol_id];
}
