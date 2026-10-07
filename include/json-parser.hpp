#pragma once

#include "limit-order-book-handler.hpp"
namespace JsonParser {
enum class ParseState {
  ParsePrice = 0,
  ParseVolume = 1,
};

void parse_snapshot(const Symbol &symbol, const std::string &message);
void parse_update(const std::string &message);
} // namespace JsonParser
