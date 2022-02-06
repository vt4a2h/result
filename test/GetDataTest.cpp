#include "catch.hpp"

#include <result/result.hpp>

TEST_CASE("Get data") {
    using namespace result;
    using TestResult = Result<int, std::string>;

    SECTION("Get value") {
        TestResult r{42};

        REQUIRE(r.value() == 42);
        REQUIRE(const_cast<const TestResult&>(r).value() == 42);
        REQUIRE(static_cast<TestResult&&>(r).value() == 42);
        REQUIRE(static_cast<const TestResult&&>(r).value() == 42);
    }

    SECTION("Get error") {
        TestResult r{std::string("42")};

        REQUIRE(r.error() == "42");
        REQUIRE(const_cast<const TestResult&>(r).error() == "42");
        REQUIRE(static_cast<TestResult&&>(r).error() == "42");
        REQUIRE(static_cast<const TestResult&&>(r).error() == "42");
    }
}