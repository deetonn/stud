#pragma once

#include "forward.hpp"
#include "stddef.hpp"

#include <math.h>

#include <concepts>

_STD_MATH_API_BEGIN

static _STD_API float64 pi = 3.14159265358979323846;

static _STD_API bool even(std::integral auto item) noexcept {
	return item % 2 == 0;
}

static _STD_API bool odd(std::integral auto item) noexcept {
	return item % 2 != 0;
}

_STD_API_END
