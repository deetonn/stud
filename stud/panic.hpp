
#ifndef _STD_PANIC

#include <string_view>
#include <stacktrace>
#include <thread>
#include <iostream>
#include <source_location>
#include <utility>
#include <string>
#include <format>

#include "forward.hpp"
#include "defer.hpp"

_STD_API_BEGIN

template<typename ...Ts>
[[noreturn]] static constexpr __forceinline void __panic(std::source_location location, const std::format_string<Ts...> fmt, Ts&&... args) noexcept {
    const auto stack = std::stacktrace::current();
    const auto thread_id = std::this_thread::get_id();

    for (std::stacktrace::const_reverse_iterator it = std::rbegin(stack);
        it != stack.rend();
        ++it) 
    {
        const auto& frame = *it;
        auto fmt = std::format("{}", frame.description());
        std::cout << fmt.c_str() << '\n';
    }

    std::cout << "----- INFO -----" << '\n';
    auto location_info = std::format("{}:{}:{}",
        location.file_name(), location.line(), location.column());
    std::cout << std::format("({})", location_info) << " in " << location.function_name() << '\n';

    auto msg = std::format(fmt, std::forward<Ts>(args)...);
    std::cout << "panic on thread (" << thread_id << ")" << '\n';
    auto output = std::format("message: {}", msg);

    std::cout << output << '\n';
    std::unreachable();
}

// #define panic(message, ...) stud::__panic(std::source_location::current(), message, ##__VA_ARGS__)

class PanicExpr {
    bool result;
    std::string_view view;
    std::source_location _location;
public:
    _STD_API PanicExpr(bool expr, std::string_view view, std::source_location location = std::source_location::current())
        : result(expr)
        , view(view)
        , _location(location)
    {}

    _STD_API bool get_expression() const noexcept {
        return result;
    }

    _STD_API std::string_view get_view() const noexcept {
        return view;
    }

    _STD_API std::source_location location() const noexcept {
        return _location;
    }
};

template<typename ...Ts>
static constexpr __forceinline void panic(
    PanicExpr condition,
    const std::format_string<Ts...> fmt,
    Ts&&... args
) noexcept {
    if (condition.get_expression()) {
        auto message = std::format<Ts...>(fmt, std::forward<Ts>(args)...);
        __panic(condition.location(), "the expression \"{}\" failed.\nmessage: {}", condition.get_view(), message);
    }
}

#define IF(condition) stud::PanicExpr {condition, #condition}
#define NOT(condition) stud::PanicExpr { !(condition), "!(" #condition ")" }
#define IF_NOT(condition) NOT(condition)
#define ALWAYS(...) stud::PanicExpr { !false, "(Debug-Assertion)" }

#define IF_EQUAL(left, right) stud::PanicExpr {left == right, "(" #left " == " #right ")"}
#define IF_NOT_EQUAL(left, right) stud::PanicExpr {left != right, "!(" #left " == " #right ")"}

_STD_API_END

#define _STD_PANIC
#endif