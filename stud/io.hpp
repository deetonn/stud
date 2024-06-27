

#ifndef _STD_IO

#include <format>
#include <iostream>
#include <filesystem>

#include "forward.hpp"

_STD_API_BEGIN

template<typename ...Args>
inline void println(
    std::format_string<Args...> format,
    Args&&... args) noexcept 
{
    const auto _Fmtd = std::format(format, std::forward<Args>(args)...);
    std::cout << _Fmtd << '\n';
}

template<typename ...Args>
inline void print(
    std::format_string<Args...> format,
    Args&&... args) noexcept
{
    const auto _Fmtd = std::format(format, std::forward<Args>(args)...);
    std::cout << _Fmtd;
}

template<typename ...Args>
inline void eprintln(
    std::format_string<Args...> format,
    Args&&... args) noexcept
{
    const auto _Fmtd = std::format(format, std::forward<Args>(args)...);
    std::cerr << _Fmtd << '\n';
}

template<typename ...Args>
inline void eprint(
    std::format_string<Args...> format,
    Args&&... args) noexcept
{
    const auto _Fmtd = std::format(format, std::forward<Args>(args)...);
    std::cerr << _Fmtd;
}

template<typename ...Args>
inline _NODISCARD
std::string 
format(const std::format_string<Args...> fmt, Args&&... args) noexcept {
    return std::format<Args...>(fmt, std::forward<Args>(args)...);
}

namespace fs = std::filesystem;

_STD_API_END

#define _STD_IO
#endif