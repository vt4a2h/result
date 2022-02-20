#pragma once

#include <variant>
#include <cassert>
#include <functional>

namespace result {

namespace details {

    class ResultBase {};

    template <class F, class V, class ...Args>
    concept IsInvocable = std::is_invocable_v<F, std::add_rvalue_reference_t<V>, Args...>;

    template <class F, class V, class ...Args>
    concept ProducesCorrectResult =
        std::is_base_of_v<
            ResultBase,
            std::invoke_result_t<F, std::add_rvalue_reference_t<V>, Args...>
        >;

    template <class F, class V, class E, class ...Args>
    concept ErrorIsConvertable =
        std::convertible_to<
            typename std::invoke_result_t<F, std::add_rvalue_reference_t<V>, Args...>::Error,
            E
        >;

    template <class F, class V, class E, class ...Args>
    concept ValueIsConvertable =
        std::convertible_to<
            typename std::invoke_result_t<F, std::add_rvalue_reference_t<E>, Args...>::Value,
            V
        >;
}

template <class V, class E>
class Result : public details::ResultBase {
public: // Types
    using Value = V;
    using Error = E;

public:
    // NOLINTBEGIN
    Result(const Value& v): data(v) {}
    Result(Value&& v): data(std::move(v)) {}

    Result(const Error& e): data(e) {}
    Result(Error&& e): data(std::move(e)) {}
    // NOLINTEND

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

    template<class F, class ...Args>
        requires
            details::IsInvocable<F, Value, Args...> &&
            details::ProducesCorrectResult<F, Value, Args...> &&
            details::ErrorIsConvertable<F, Value, Error, Args...>
    auto then(F f, Args&&... args) -> std::invoke_result_t<F, std::add_rvalue_reference_t<V>, Args...> {
        if (hasValue())
            return std::invoke(f, std::move(*this).value(), std::forward<Args>(args)...);

        return std::move(*this).error();
    }

    template<class F, class ...Args>
        requires
            details::IsInvocable<F, Error, Args...> &&
            details::ProducesCorrectResult<F, Error, Args...> &&
            details::ValueIsConvertable<F, Value, Error, Args...>
    [[nodiscard]] auto onError(F f, Args&&... args) -> std::invoke_result_t<F, std::add_rvalue_reference_t<E>, Args...> {
        if (hasError())
            return std::invoke(f, std::move(*this).error(), std::forward<Args>(args)...);

        return std::move(*this).value();
    }

private:
    std::variant<Value, Error> data;
};

} // namespace result
