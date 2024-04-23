#pragma once

#include "forward.hpp"

#include <string>
#include <type_traits>

_STD_API_BEGIN

using string = std::string;

#define __CT_DEF _STD_API static

template <class _CharT>
class char_traits {
public:
	template <std::size_t N>
	__CT_DEF size_t constexpr_length(const _CharT(&s)[N]) noexcept {
		return N;
	}

	__CT_DEF size_t length(const _CharT* s) noexcept {
		static_assert(std::is_same_v<_CharT, char> || std::is_same_v<_CharT, wchar_t>, "This version char_traits::length() only supports `char` & `wchar_t`");
		if constexpr (std::is_same_v<_CharT, char>)
			return std::strlen(s);
		if constexpr (std::is_same_v<_CharT, wchar_t>)
			return std::wcslen(s);
	}

	__CT_DEF size_t size() noexcept {
		return sizeof(_CharT);
	}

	__CT_DEF size_t(max_value)() noexcept {
		return ((std::make_unsigned<_CharT>::type) -1) / 2;
	}

	__CT_DEF size_t(min_value)() noexcept {
		return 0;
	}
};

_STD_API_END