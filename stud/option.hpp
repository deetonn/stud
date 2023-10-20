#ifndef _STD_OPTION

#include <variant>
#include <utility>

#include "forward.hpp"

_STD_API_BEGIN

struct __none_t {};
static constexpr inline __none_t none = {};

template<class _Ty>
class Option {
private:
    std::variant<_Ty, __none_t> _Variant;
public:
    _STD_API Option(_Ty&& some) noexcept {
        _Variant = std::move(some);
    }
    _STD_API Option(__none_t) noexcept {
        _Variant = none;
    }

    _NODISCARD _STD_API bool is_none() const noexcept {
        return std::get_if<1>(&_Variant) != nullptr;
    }

    _NODISCARD _STD_API bool is_some() const noexcept {
        return std::get_if<0>(&_Variant) != nullptr;
    }

    _NODISCARD _STD_API _Ty unwrap() const noexcept {
        return *std::get_if<0>(&_Variant);
    }

    _NODISCARD _STD_API _Ty unwrap_or(_Ty other) const noexcept {
        if (is_none()) {
            return other;
        }
        else {
            return unwrap();
        }
    }
};

auto Some(auto&& value) -> Option<decltype(std::move<decltype(std::decay_t<decltype(value)>{value}) > (value)) > {
    return std::move(value);
}

_NODISCARD auto Some(auto value) -> Option<decltype(value)> {
    return Option<decltype(value)>(std::move(value));
}

_STD_API_END

#define _STD_OPTION
#endif