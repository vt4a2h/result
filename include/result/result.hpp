#pragma once

#include <variant>
#include <cassert>

namespace result {

template <class V, class E>
class Result {
public: // Types
    using Value = V;
    using Error = E;

public:
    explicit Result(const Value& v): data(v) {}
    explicit Result(Value&& v): data(std::move(v)) {}

    explicit Result(const Error& e): data(e) {}
    explicit Result(Error&& e): data(std::move(e)) {}

    Result(const Result& src): data(src.data) {}
    Result(Result&& src) noexcept: data(std::move(src).data) {}

    Result &operator= (const Value &v) {
        data = v;
        return *this;
    }

    Result &operator= (Value &&v) noexcept {
        data = std::move(v);
        return *this;
    }

    Result &operator= (const Error &e) {
        data = e;
        return *this;
    }

    Result &operator= (Error &&e) noexcept {
        data = std::move(e);
        return *this;
    }

    Result &operator= (const Result& rhs) {
        auto tmp = rhs.data;
        std::swap(tmp, data);
        return *this;
    }

    Result &operator= (Result&& rhs) noexcept {
        data = std::move(rhs).data;
        return *this;
    }

    [[nodiscard]]
    inline constexpr bool hasError() const { return std::holds_alternative<Error>(data); }

    [[nodiscard]]
    inline constexpr bool hasValue() const { return std::holds_alternative<Value>(data); }

    inline constexpr Value &value() & { return std::get<Value>(data); }
    inline constexpr const Value &value() const & { return std::get<Value>(data); }
    inline constexpr Value &&value() && { return std::get<Value>(std::move(data)); }
    inline constexpr const Value &&value() const &&
    { return std::get<Value>(static_cast<const Result&&>(*this).data); }

    inline constexpr Error &error() & { return std::get<Error>(data); }
    inline constexpr const Error &error() const & { return std::get<Error>(data); }
    inline constexpr Error &&error() && { return std::get<Error>(std::move(data)); }
    inline constexpr const Error &&error() const &&
    { return std::get<Error>(static_cast<const Result&&>(*this).data); }

private:
    std::variant<Value, Error> data;
};

} // namespace result
