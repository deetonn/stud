#pragma once

#include "forward.hpp"

#include <string>
#include <type_traits>
#include <new>

_STD_API_BEGIN

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

/*
NOTE: This function always tried to go larger. This is to fit stud::strings specification
      of never having memory issues. We always go larger if needed.
*/

_STD_API size_t aligned_size(size_t actual_size) {
	while ((actual_size % std::hardware_destructive_interference_size) != 0) {
		++actual_size;
	}

	return actual_size;
}

_STD_API_END

_STD_DETAIL_API

#define RAW_STR_BUFF_DEREF(p) (*(p))

template <class _CharT, class _Traits>
class _String_guts {
private:
	_CharT* _Ptr{ nullptr };
	size_t  _Length{ 0 };
	size_t _Capacity{ 0 };
public:
	_STD_API bool _Is_uninitialized() const noexcept {
		return _Ptr == nullptr;
	}
	_STD_API void _Overwrite_init(const _CharT* ptr) noexcept {
		/* 
			NOTES: This function aims to keep string memory aligned, rather than reserve memory space. 
				   Keeping memory aligned for cache hits is much more important than saving 12 bytes of memory.
		*/
		size_t _New_str_length = _Traits::length(ptr);
		const size_t _Aligned_size = aligned_size(_New_str_length);
		if (_Ptr) {
			if (_New_str_length == _Length) {
				// Perfect.
				std::memcpy(_Ptr, ptr, _Length);
				return;
			}

			if (_New_str_length > _Capacity) {
				// Realloc to avoid a fragmented heap. This class aims to keep the same piece
				// of heap memory the whole time.
				_Ptr = static_cast<_CharT*>(realloc(_Ptr, aligned_size(_New_str_length)));
				std::memcpy(_Ptr, ptr, _New_str_length);
				std::memcpy(_Ptr + _New_str_length, "\0", 1);
			}
			else {
				std::memcpy(_Ptr, ptr, _New_str_length);
				std::memcpy(_Ptr + _New_str_length, "\0", 1);
			}

			_Length = _New_str_length;
			_Capacity = _Aligned_size;

			return;
		}

		_Ptr = static_cast<_CharT*>(::malloc(aligned_size(_New_str_length)));
		std::memcpy(_Ptr, ptr, _New_str_length);
		std::memcpy(_Ptr + _New_str_length, "\0", 1);

		_Length = _New_str_length;
		_Capacity = _Aligned_size;
	}
	_STD_API void _Overwrite_init(const _CharT* ptr, size_t count) noexcept {

	}

	_STD_API void _Do_copy(_String_guts& other) noexcept {
		_Overwrite_init(other._Ptr);
	}
	_STD_API void _Do_move(_String_guts& other) noexcept {
		other._Ptr = _Ptr;
		other._Length = _Length;
		other._Capacity;

		other._Ptr = NULL;
		other._Length = 0;
		other._Capacity = 0;
	}

	_STD_API void _Do_append(const _CharT* ptr) noexcept {
		const auto _New_str_length = _Traits::length(ptr);
		const auto _Combined_length = _Length + _New_str_length;

		if (_Combined_length > _Capacity) {
			_Capacity = aligned_size(_Combined_length);
			_Ptr = static_cast<_CharT*>(::realloc(_Ptr, _Capacity));
			std::memcpy(_Ptr + _Length, ptr, _New_str_length);
			std::memcpy(_Ptr + _Length, "\0", 1);
		}
		else {
			std::memcpy(_Ptr + _Length, ptr, _New_str_length);
			std::memcpy(_Ptr + _Combined_length, "\0", 1);
		}

		_Length = _Combined_length;
	}
	_STD_API void _Do_append_charT(const _CharT ch) noexcept {
		const auto _New_length = _Length + 1;

		if (_New_length >= _Capacity) {
			// Account for the fact we could be bang on 64, so we may have to manually increment by
			// one to force aligned_size to double.
			_Capacity = aligned_size(_New_length % std::hardware_destructive_interference_size == 0 ? _New_length + 1 : _New_length);
			_Ptr = static_cast<_CharT*>(::realloc(_Ptr, _Capacity));
			RAW_STR_BUFF_DEREF(_Ptr + _Length) = ch;
			_Length = _Length + 1;
			RAW_STR_BUFF_DEREF(_Ptr + _Length) = '\0';
		}
		else {
			RAW_STR_BUFF_DEREF(_Ptr + _Length) = ch;
			_Length++;
			RAW_STR_BUFF_DEREF(_Ptr + _Length) = '\0';
		}
	}

	_STD_API void _Die() noexcept {
		if (_Is_uninitialized())
			return;

		free<_CharT>(_Ptr);
	}
	// Strange const modifier, but this is private API so no point.
	_STD_API _CharT* _Data() const noexcept {
		return _Ptr;
	}

	_STD_API size_t _Get_Length() const noexcept { return _Length; }
	_STD_API size_t _Get_Capacity() const noexcept { return _Capacity; }
};

/* 
All string constructors and assignment operators are required to deal with the null terminator.
*/

template <class _CharT, class _Traits = _STUD char_traits<_CharT>>
class _Basic_string {
private:
	_String_guts<_CharT, _Traits> _Base{};
public:
	_STD_API _Basic_string() noexcept = default;
	_STD_API _Basic_string(const _CharT* str) noexcept {
		_Base._Overwrite_init(str);
	}
	_STD_API _Basic_string(const _CharT* str, size_t count) noexcept {
		_Base._Overwrite_init(str, count);
	}
	_STD_API _Basic_string(const _Basic_string& other) noexcept {
		_Base._Do_copy(other._Base);
	}
	_STD_API _Basic_string(_Basic_string&& other) noexcept {
		_Base._Do_move(other._Base);
	}

	_STD_API ~_Basic_string() noexcept {
		_Base._Die();
	}

	_STD_API _Basic_string& operator= (const _Basic_string& other) noexcept {
		_Base._Do_copy(other._Base);
		return *this;
	}

	void append(const _CharT* str) noexcept {
		_Base._Do_append(str);
	}
	void append(const _CharT ch) noexcept {
		_Base._Do_append_charT(ch);
	}

	_Basic_string& operator+= (const _CharT* str) noexcept {
		this->append(str);
		return *this;
	}
	_Basic_string& operator+= (const _CharT ch) noexcept {
		this->append(ch);
		return *this;
	}

	_STD_API size_t size() const noexcept { return _Base._Get_Length(); }
	_STD_API size_t capacity() const noexcept { return _Base._Get_Capacity(); }

	_STD_API _Basic_string substr(size_t offset, size_t count = ULLONG_MAX) {
		panic(IF(offset > size()), "Substring operand is out of range. (offset={}, size={})", offset, size());
		return _Basic_string(data() + offset, count);
	}

	_STD_API bool operator== (const _Basic_string& other) noexcept {
		return strcmp(data(), other.data()) == 0;
	}
	_STD_API bool operator!= (const _Basic_string& other) noexcept {
		return !(*this == other);
	}

	const _CharT* data() const noexcept { return _Base._Data(); }

	_String_guts<_CharT, _Traits>* _Scary_get_guts() noexcept { return &_Base; }
};

_STD_API_END

_STD_API_BEGIN

using string = _DETAIL _Basic_string<char>;
using wstring = _DETAIL _Basic_string<wchar_t>;

template <class _CharT>
inline _DETAIL _Basic_string<_CharT> make_string(const _CharT* ptr) noexcept {
	return { ptr };
}

_STD_API_END