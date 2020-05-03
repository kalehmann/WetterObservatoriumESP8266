#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

TEST_CASE("One plus one equals two") {
  REQUIRE(1 + 1 == 2);
}
