#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

using Symbol = std::string;
using SymbolView = std::string_view;
using SymbolId = uint16_t;
using SymbolIntegerScale = uint8_t;

struct SymbolInfo {
  // from our assumptions - tickSize and stepSize are in the form 10^-K1/K2
  // here, we store just K1 and K2
  SymbolIntegerScale price_scale;
  SymbolIntegerScale volume_scale;
};

inline constexpr std::array MOCK_ENTRIES = {
    std::tuple{SymbolView{"AB"}, SymbolIntegerScale{4}, SymbolIntegerScale{0}},
    std::tuple{SymbolView{"AC"}, SymbolIntegerScale{2}, SymbolIntegerScale{1}},
    std::tuple{SymbolView{"BNBBTC"}, SymbolIntegerScale{4},
               SymbolIntegerScale{1}},
};

class SymbolRepo {
public:
  SymbolRepo();

  SymbolId get_symbol_id(SymbolView symbol) const;

  const SymbolInfo &get_symbol_info(SymbolId symbol_id) const;

  std::size_t size() const { return symbol_ids_.size(); }

private:
  std::unordered_map<Symbol, SymbolId> symbol_ids_;
  std::vector<SymbolInfo> symbol_infos_;
};

inline SymbolRepo symbol_repo;
