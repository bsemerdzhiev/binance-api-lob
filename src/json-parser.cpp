#include "json-parser.hpp"
#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include "symbol-repo.hpp"
#include <string_view>

uint64_t parse_string_decimal(std::string_view cur_string,
                              const SymbolIntegerScale scale) {
  uint64_t final_number = 0;

  int8_t dec_scale = 0;
  int8_t remaining_scale = scale;

  for (auto &cur_char : cur_string) {
    if (cur_char == '.') {
      dec_scale = 1;
      continue;
    }

    remaining_scale -= dec_scale;

    final_number *= 10;

    final_number += cur_char - '0';
  }

  for (int32_t i{0}; i < remaining_scale; i++) {
    final_number *= 10;
  }

  return final_number;
}

// following the API description listed here
// https://developers.binance.com/en/docs/catalog/core-trading-spot-trading/api/rest-api/market#depth
void JsonParser::parse_snapshot(const Symbol &symbol,
                                std::string_view message) {
  std::size_t msg_index = 0;

  SymbolId symbol_id = symbol_repo.get_symbol_id(symbol);
  const SymbolInfo &symbol_info = symbol_repo.get_symbol_info(symbol_id);

  Side order_side = Side::BUY;
  ParseState parse_state = ParseState::ParsePrice;
  Order current_order;

  while (true) {
    std::size_t token_start = message.find('"', msg_index);

    if (token_start == std::string_view::npos)
      break;

    std::size_t token_end = message.find('"', token_start + 1);

    std::string_view cur_string =
        message.substr(token_start + 1, token_end - token_start - 1);

    if (cur_string == "asks") {
      order_side = Side::SELL;
    } else if (cur_string == "bids") {
      order_side = Side::BUY;
    } else if (parse_state == ParseState::ParsePrice) {
      // parse cur_string to a price
      current_order.first =
          parse_string_decimal(cur_string, symbol_info.price_scale);

      parse_state = ParseState::ParseVolume;
    } else {
      // parse cur_string to a volume
      current_order.second =
          parse_string_decimal(cur_string, symbol_info.volume_scale);

      // update the LOB corresponding to the symbol
      order_book_handler.update_symbol(symbol_id, order_side, current_order);

      parse_state = ParseState::ParsePrice;
    }

    msg_index = token_end + 1;
  }
}

// following the API description listed here
// https://developers.binance.com/en/docs/catalog/core-trading-spot-trading/api/ws-streams/~#diff-book-depth
void JsonParser::parse_update(std::string_view message) {
  std::size_t msg_index = 0;

  SymbolId symbol_id = 0;
  SymbolInfo symbol_info;

  Side order_side = Side::BUY;
  //                doesnt matter what we initialize it to initially
  ParseState parse_state = ParseState::ParsePrice;
  Order current_order;

  while (true) {
    std::size_t token_start = message.find('"', msg_index);

    if (token_start == std::string_view::npos)
      break;

    std::size_t token_end = message.find('"', token_start + 1);

    std::string_view cur_string =
        message.substr(token_start + 1, token_end - token_start - 1);

    if (cur_string == "e") {
      parse_state = ParseState::ParseType;
    } else if (cur_string == "s") {
      parse_state = ParseState::ParseSymbol;
    } else if (cur_string == "a") {
      order_side = Side::SELL;
      parse_state = ParseState::ParsePrice;
    } else if (cur_string == "b") {
      order_side = Side::BUY;
      parse_state = ParseState::ParsePrice;
    } else if (parse_state == ParseState::ParseSymbol) {
      symbol_id = symbol_repo.get_symbol_id(cur_string);
      symbol_info = symbol_repo.get_symbol_info(symbol_id);

      parse_state = ParseState::ParsePrice;
    } else if (parse_state == ParseState::ParseType) {
      assert(cur_string == "depthUpdate");

      parse_state = ParseState::ParsePrice;
    } else if (parse_state == ParseState::ParsePrice) {
      // parse cur_string to a price
      current_order.first =
          parse_string_decimal(cur_string, symbol_info.price_scale);

      parse_state = ParseState::ParseVolume;
    } else {
      // parse cur_string to a volume
      current_order.second =
          parse_string_decimal(cur_string, symbol_info.volume_scale);

      // update the LOB corresponding to the symbol
      order_book_handler.update_symbol(symbol_id, order_side, current_order);

      parse_state = ParseState::ParsePrice;
    }

    msg_index = token_end + 1;
  }
}
