#include "catch.hpp"

#include <result/result.hpp>

using IntStrResult = result::Result<int, std::string>;
using StrIntResult = result::Result<std::string, int>;

static auto addOne(int i) { return IntStrResult{++i}; }

TEST_CASE("Invoke next function when result contains a value") {
    using namespace result;

    SECTION("Invoke single function") {
        auto f = [] { return Result<int, std::string>{41}; };

        auto r = f().then([](auto &&v) { return IntStrResult{++v} ; });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == 42);
    }

    SECTION("Invoke several functions") {
        auto f = [] { return Result<int, std::string>{39}; };

        auto r = f()
            .then([](auto &&v) { return IntStrResult{++v} ; })
            .then([](auto &&v) { return IntStrResult{++v} ; })
            .then([](auto &&v) { return IntStrResult{++v} ; });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == 42);
    }

    SECTION("Invoke a free function") {
        auto f = [] { return Result<int, std::string>{41}; };

        auto r = f().then(&addOne);

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == 42);
    }

    SECTION("Invoke a function with optional args") {
        auto f = [] { return Result<int, std::string>{41}; };

        auto r = f().then([](auto &&v, int add) { return IntStrResult{v + add}; }, 1);

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == 42);
    }

    SECTION("Functions with different types") {
        auto f = [] { return StrIntResult{"foo"}; };

        auto r = f()
            .then([](std::string s) -> StrIntResult { return s; })
            .then([](auto &&s) -> StrIntResult { return s; })
            .then([](const auto &&s) -> StrIntResult { return s; })
            .then([](std::string &&s) -> StrIntResult { return s; })
            .then([](const std::string &s) -> StrIntResult { return s; })
            .then([](const auto &s) -> StrIntResult { return s; });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == "foo");
    }

    SECTION("Different return types") {
        auto r = [] { return IntStrResult{42}; }()
            .then([](auto v) { return Result<double, std::string>{v / 2.}; });

        REQUIRE(r.hasValue());
        REQUIRE(std::is_same_v<decltype(r)::Value, double>);
        REQUIRE(fabs(r.value() - 42 / 2.) < std::numeric_limits<double>::epsilon());
    }
}

TEST_CASE("Invoke next function when result contains an error") {
    using namespace result;

    SECTION("Function is not invoked if result contains an error") {
        int counter = 0;

        auto f = [] { return Result<int, std::string>{"foo"}; };

        auto r = f().then([&counter](auto &&v) -> IntStrResult { return ++counter, ++v ; });

        REQUIRE(r.hasError());
        REQUIRE(r.error() == "foo");
        REQUIRE(counter == 0);
    }

    SECTION("Error breaks evaluation chain") {
        int counter = 0;

        auto f = [] { return Result<int, std::string>{40}; };

        auto r = f()
            .then([&counter](auto &&v) -> IntStrResult { return ++counter, ++v ; })
            .then([&counter](auto &&v) -> IntStrResult { return ++counter, ++v ; })
            .then([&counter](auto &&) -> IntStrResult { return ++counter, std::string("error"); })
            .then([&counter](auto &&v) -> IntStrResult { return ++counter, ++v ; });

        REQUIRE(r.hasError());
        REQUIRE(r.error() == "error");
        REQUIRE(counter == 3);
    }
}
