#include "catch.hpp"

#include <result/result.hpp>

TEST_CASE("Check state") {
    result::Result<int, std::string> r{0};

    SECTION("Value") {
        REQUIRE(r.hasValue());
    }

    SECTION("Error") {
        r = std::string("");
        REQUIRE(r.hasError());
    }
}