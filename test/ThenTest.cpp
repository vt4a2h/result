#include "catch.hpp"

#include <result/result.hpp>

static int addOne(int i) { return ++i; }

TEST_CASE("Invoke next function when result contains a value") {
    using namespace result;

    SECTION("Invoke single function") {
        auto f = [] { return Result<int, std::string>{41}; };

        auto r = f().then([](auto &&v) { return ++v ; });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == 42);
    }

    SECTION("Invoke several functions") {
        auto f = [] { return Result<int, std::string>{39}; };

        auto r = f()
            .then([](auto &&v) { return ++v ; })
            .then([](auto &&v) { return ++v ; })
            .then([](auto &&v) { return ++v ; });

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

        auto r = f().then([](auto &&v, int add) { return v + add; }, 1);

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == 42);
    }

    SECTION("Functions with different types") {
        auto f = [] { return Result<std::string, int>{"foo"}; };

        auto r = f()
            .then([](std::string s) { return s; })
            .then([](auto &&s) { return s; })
            .then([](const auto &&s) { return s; })
            .then([](std::string &&s) { return s; })
            .then([](const std::string &s) { return s; })
            .then([](const auto &s) { return s; });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == "foo");
    }
}

TEST_CASE("Invoke next function when result contains an error") {
    using namespace result;

    SECTION("Function is not invoked if result contains an error") {
        int counter = 0;

        auto f = [] { return Result<int, std::string>{"foo"}; };

        auto r = f().then([&counter](auto &&v) { return ++counter, ++v ; });

        REQUIRE(r.hasError());
        REQUIRE(r.error() == "foo");
        REQUIRE(counter == 0);
    }

    SECTION("Error breaks evaluation chain") {
        int counter = 0;

        auto f = [] { return Result<int, std::string>{40}; };

        auto r = f()
            .then([&counter](auto &&v) { return ++counter, ++v ; })
            .then([&counter](auto &&v) { return ++counter, ++v ; })
            .then([&counter](auto &&) { return ++counter, std::string("error"); })
            .then([&counter](auto &&v) { return ++counter, ++v ; })
            ;

        REQUIRE(r.hasError());
        REQUIRE(r.error() == "error");
        REQUIRE(counter == 3);
    }
}
