#include "catch.hpp"

#include <result/result.hpp>

TEST_CASE("Create result") {
    using namespace result;
    using TestResult = Result<int, std::string>;

    // NOLINTBEGIN
    SECTION("From value by copy") {
        int v = 42;
        std::string err;

        TestResult r{v};
        TestResult r1{err};
        TestResult r2{r};
    }

    SECTION("From value by move") {
        TestResult r{42};
        TestResult r1{""};
        TestResult r2{std::move(r)};
    }

    SECTION("Assign operator") {
        TestResult r{42};
        TestResult r1{""};
        r = r1;

        int v = 42;
        std::string err;

        r = v;
        r = err;
    }

    SECTION("Move assign operator") {
        TestResult r{42};
        TestResult r1{""};

        r = std::move(r1);

        int v = 42;
        std::string err;

        r = std::move(v);
        r = std::move(err);
    }
    // NOLINTEND
}
