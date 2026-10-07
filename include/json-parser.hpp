#pragma once

#include "symbol-repo.hpp"
#include <cstdint>

/*
 * We assume that the message fields might be in any order
 * while parsing the message.
 *
 * Thats why the FSM structure ParseState is needed
 */
namespace JsonParser {
enum class ParseState : uint8_t {
  ParsePrice = 0,
  ParseVolume = 1,
  ParseSymbol = 2,
  ParseType = 3,
};

uint64_t parse_string_decimal(std::string_view cur_string,
                              const SymbolIntegerScale scale);
void parse_snapshot(const Symbol &symbol, std::string_view message);
void parse_update(std::string_view message);
} // namespace JsonParser
