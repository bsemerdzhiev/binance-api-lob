#include "json-parser.hpp"
#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include <string_view>

// following the API description listed here
// https://developers.binance.com/en/docs/catalog/core-trading-spot-trading/api/rest-api/market#depth
void JsonParser::parse_snapshot(const Symbol &symbol,
                                const std::string &message) {
  std::size_t msg_index = 0;

  Side order_side = Side::BUY;
  ParseState parse_state = ParseState::ParsePrice;
  Order current_order;

  while (true) {
    int32_t token_start = message.find('"', msg_index);

    if (token_start == std::string::npos)
      break;

    int32_t token_end = message.find('"', token_start);

    std::string_view cur_string =
        message.substr(token_start + 1, token_end - token_start - 1);

    if (cur_string == "asks") {
      order_side = Side::SELL;
    } else if (cur_string == "bids") {
      order_side = Side::BUY;
    } else if (parse_state == ParseState::ParsePrice) {

      parse_state = ParseState::ParseVolume;
    } else {

      parse_state = ParseState::ParsePrice;
    }

    msg_index = token_end + 1;
  }
}

// following the API description listed here
// https://developers.binance.com/en/docs/catalog/core-trading-spot-trading/api/ws-streams/~#diff-book-depth
void JsonParser::parse_update(const std::string &message) {}
