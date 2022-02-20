#include "catch.hpp"

#include <result/result.hpp>

TEST_CASE("On error handler is invoked") {
    using namespace result;

    SECTION("When error occurred") {
        auto r = [] { return Result<std::string, int>(41); }()
            .onError([](int e) { return Result<std::string, int>(++e); });

        REQUIRE(r.hasError());
        REQUIRE(r.error() == 42);
    }

    SECTION("To recover from the error") {
        auto r = [] { return Result<std::string, int>(42); }()
            .onError([](int e) { return Result<std::string, int>(std::to_string(e)); });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == "42");
    }

    SECTION("Several times") {
        auto r = [] { return Result<std::string, int>(40); }()
            .onError([](int e) { return Result<std::string, int>(++e); })
            .onError([](int e) { return Result<std::string, int>(++e); });

        REQUIRE(r.hasError());
        REQUIRE(r.error() == 42);
    }

    SECTION("Recover several times") {
        auto r = [] { return Result<std::string, int>(40); }()
            .onError([](int e) { return Result<std::string, int>(std::to_string(e)); })
            .then([](auto &&v) { return Result<std::string, int>(std::stoi(v) + 1); })
            .onError([](int e) { return Result<std::string, int>(std::to_string(e)); })
            .then([](auto &&v) { return Result<std::string, int>(std::stoi(v) + 1); })
            .onError([](int e) { return Result<std::string, int>(std::to_string(e)); });

        REQUIRE(r.hasValue());
        REQUIRE(r.value() == "42");
    }

    SECTION("Different return types") {
        auto r = [] { return Result<int, std::string>(""); }()
            .onError([](auto &&) { return Result<double, std::string>(1 / 3.);  });

        REQUIRE(r.hasValue());
        REQUIRE(std::is_same_v<decltype(r)::Value, double>);
        REQUIRE(fabs(r.value() - 1 / 3.) < std::numeric_limits<double>::epsilon());
    }
}

TEST_CASE("On error handler is not invoked"){
    using namespace result;

    SECTION("When no error occurred") {
        int counter{};
        auto r = [] { return Result<std::string, int>("42"); }()
            .onError([&counter](auto&&) { return Result<std::string, int>(++counter); });

        REQUIRE(r.hasValue());
        REQUIRE(counter == 0);
        REQUIRE(r.value() == "42");
    }
}