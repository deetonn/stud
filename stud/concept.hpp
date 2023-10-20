
#ifndef _STD_CONCEPTS

#include <concepts>
#include <string>

#include "forward.hpp"

_STD_API_BEGIN

template<class T>
concept ToString = requires(T t) {
    { t.to_string() } -> std::same_as<std::string>;
};

_STD_API_END

#define _STD_CONCEPTS
#endif