#include "catch2/matchers/catch_matchers.hpp"
#include "json-parser.hpp"
#include "limit-order-book-handler.hpp"
#include "limit-order-book.hpp"
#include "symbol-repo.hpp"
#include <catch2/catch_test_macros.hpp>

#include <catch2/matchers/catch_matchers_vector.hpp>

using Catch::Matchers::UnorderedEquals;

TEST_CASE("Document Example Test") {
  order_book_handler.reset();

  std::string snapshot_msg = R"(
  {
    "bids" : [
    ["0.0024","14.70000000"],
    ["0.0022","6.40000000"],
    ["0.0020","9.70000000"]
  ]
  "asks" : [
    ["0.0024","14.90000000"],
    ["0.0026","3.60000000"],
    ["0.0028","1.00000000"]
  ]
  }
  )";

  JsonParser::parse_snapshot("BNBBTC", snapshot_msg);

  const SymbolId symbol_id = symbol_repo.get_symbol_id("BNBBTC");

  std::vector<std::pair<Price, Volume>> snapshot_bids, snapshot_asks;
  snapshot_bids = order_book_handler.get_all_levels(symbol_id, Side::BUY);
  snapshot_asks = order_book_handler.get_all_levels(symbol_id, Side::SELL);

  std::vector<std::pair<Price, Volume>> bids_expected = {
      {24, 147}, {22, 64}, {20, 97}};

  std::vector<std::pair<Price, Volume>> asks_expected = {
      {24, 149}, {26, 36}, {28, 10}};

  REQUIRE_THAT(snapshot_bids, UnorderedEquals(bids_expected));
  REQUIRE_THAT(snapshot_asks, UnorderedEquals(asks_expected));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 24);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 24);

  std::string updates[] = {
      R"({ "e": "depthUpdate", "s": "BNBBTC", "b": [ ["0.0024","10"] ], "a":
      [ ["0.0026","100"] ] })",
      R"({ "e": "depthUpdate", "s": "BNBBTC", "b": [ ["0.0024","8"] ], "a": [
      ["0.0028","0"] ] })",
      R"({ "e": "depthUpdate", "s": "BNBBTC", "b": [
      ["0.0024","0"] ], "a": [ ["0.0026","15"],["0.0027","5"] ] })",
      R"( {
      "e": "depthUpdate", "s": "BNBBTC", "b": [ ["0.0025","100"] ], "a": [
      ["0.0026","0"],["0.0027","5"] ] })",
      R"({ "e": "depthUpdate", "s":
      "BNBBTC", "b": [ ["0.0025","0"] ], "a": [
      ["0.0026","15"],["0.0024","0"] ] })",
  };

  for (const auto &update : updates) {
    JsonParser::parse_update(update);
  }

  std::vector<Order> current_bids =
      order_book_handler.get_all_levels(symbol_id, Side::BUY);

  std::vector<Order> current_asks =
      order_book_handler.get_all_levels(symbol_id, Side::SELL);

  bids_expected = {
      {22, 64},
      {20, 97},
  };

  asks_expected = {
      {26, 150},
      {27, 50},
  };

  REQUIRE_THAT(current_bids, UnorderedEquals(bids_expected));
  REQUIRE_THAT(current_asks, UnorderedEquals(asks_expected));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 22);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 26);
}

/*
 *
 * ChatGPT generated tests below
 *
 */

TEST_CASE("LOB Correctness") {
  order_book_handler.reset();
  const SymbolId symbol_id = symbol_repo.get_symbol_id("BNBBTC");

  // ---------------------------------------------------------------------------
  // Initial snapshot
  // ---------------------------------------------------------------------------

  std::string snapshot_msg = R"(
  {
    "bids" : [
      ["0.0024","14.70000000"],
      ["0.0022","6.40000000"],
      ["0.0020","9.70000000"]
    ],
    "asks" : [
      ["0.0024","14.90000000"],
      ["0.0026","3.60000000"],
      ["0.0028","1.00000000"]
    ]
  }
  )";

  JsonParser::parse_snapshot("BNBBTC", snapshot_msg);

  std::vector<Order> expected_bids = {
      {24, 147},
      {22, 64},
      {20, 97},
  };

  std::vector<Order> expected_asks = {
      {24, 149},
      {26, 36},
      {28, 10},
  };

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::BUY),
               UnorderedEquals(expected_bids));

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::SELL),
               UnorderedEquals(expected_asks));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 24);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 24);

  // ---------------------------------------------------------------------------
  // Modify existing levels
  // ---------------------------------------------------------------------------

  JsonParser::parse_update(R"(
  {
    "e": "depthUpdate",
    "s": "BNBBTC",
    "b": [["0.0024","10"]],
    "a": [["0.0026","20"]]
  }
  )");

  expected_bids = {
      {24, 100},
      {22, 64},
      {20, 97},
  };

  expected_asks = {
      {24, 149},
      {26, 200},
      {28, 10},
  };

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::BUY),
               UnorderedEquals(expected_bids));

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::SELL),
               UnorderedEquals(expected_asks));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 24);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 24);

  // ---------------------------------------------------------------------------
  // Insert new levels
  // ---------------------------------------------------------------------------

  JsonParser::parse_update(R"(
  {
    "e": "depthUpdate",
    "s": "BNBBTC",
    "b": [["0.0025","7"]],
    "a": [["0.0027","5"]]
  }
  )");

  expected_bids = {
      {25, 70},
      {24, 100},
      {22, 64},
      {20, 97},
  };

  expected_asks = {
      {24, 149},
      {26, 200},
      {27, 50},
      {28, 10},
  };

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::BUY),
               UnorderedEquals(expected_bids));

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::SELL),
               UnorderedEquals(expected_asks));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 25);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 24);

  // ---------------------------------------------------------------------------
  // Delete existing levels using volume 0
  // ---------------------------------------------------------------------------

  JsonParser::parse_update(R"(
  {
    "e": "depthUpdate",
    "s": "BNBBTC",
    "b": [["0.0024","0"]],
    "a": [["0.0028","0"]]
  }
  )");

  expected_bids = {
      {25, 70},
      {22, 64},
      {20, 97},
  };

  expected_asks = {
      {24, 149},
      {26, 200},
      {27, 50},
  };

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::BUY),
               UnorderedEquals(expected_bids));

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::SELL),
               UnorderedEquals(expected_asks));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 25);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 24);

  // ---------------------------------------------------------------------------
  // Deleting a nonexistent level should be harmless
  // ---------------------------------------------------------------------------

  JsonParser::parse_update(R"(
  {
    "e": "depthUpdate",
    "s": "BNBBTC",
    "b": [["0.0099","0"]],
    "a": [["0.0098","0"]]
  }
  )");

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::BUY),
               UnorderedEquals(expected_bids));

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::SELL),
               UnorderedEquals(expected_asks));

  // ---------------------------------------------------------------------------
  // Multiple levels on each side in a single update
  // ---------------------------------------------------------------------------

  JsonParser::parse_update(R"(
  {
    "e": "depthUpdate",
    "s": "BNBBTC",
    "b": [
      ["0.0025","15"],
      ["0.0023","8"],
      ["0.0020","0"]
    ],
    "a": [
      ["0.0024","0"],
      ["0.0026","3"],
      ["0.0029","11"]
    ]
  }
  )");

  expected_bids = {
      {25, 150},
      {23, 80},
      {22, 64},
  };

  expected_asks = {
      {26, 30},
      {27, 50},
      {29, 110},
  };

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::BUY),
               UnorderedEquals(expected_bids));

  REQUIRE_THAT(order_book_handler.get_all_levels(symbol_id, Side::SELL),
               UnorderedEquals(expected_asks));

  REQUIRE(order_book_handler.get_best_bid(symbol_id) == 25);
  REQUIRE(order_book_handler.get_best_ask(symbol_id) == 26);
}

TEST_CASE("Decimal Parsing") {
  REQUIRE(JsonParser::parse_string_decimal("0.0024", 4) == 24);
  REQUIRE(JsonParser::parse_string_decimal("0.00240000", 4) == 24);

  REQUIRE(JsonParser::parse_string_decimal("14.7", 1) == 147);
  REQUIRE(JsonParser::parse_string_decimal("14.70000000", 1) == 147);

  REQUIRE(JsonParser::parse_string_decimal("10", 1) == 100);
  REQUIRE(JsonParser::parse_string_decimal("0", 1) == 0);

  REQUIRE(JsonParser::parse_string_decimal("1.2345", 4) == 12345);

  REQUIRE(JsonParser::parse_string_decimal("123.2345", 0) == 123);
}
