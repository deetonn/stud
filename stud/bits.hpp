#pragma once

#include "forward.hpp"
#include "string.hpp"
#include "array.hpp"

_STD_API_BEGIN

template <size_t N>
class BitSet;

template <size_t _NumBytes>
struct _ByteStorage {
	static constexpr size_t _Count = _NumBytes;
	std::uint8_t storage[_NumBytes];
};

template <class T, size_t _Coff = T::StorageType::_Count>
static _STD_API size_t _Get_storage_index_from_bit_position(const std::size_t _Bitoff) noexcept {
	if (_Bitoff > _Coff) {
		return _Coff / std::hardware_destructive_interference_size;
	}

	return _Get_storage_index_from_bit_position<T, _Coff - 8>(_Bitoff);
}

template <size_t N>
class BitSet {
public:
	static_assert(N % 8 == 0, "The amount of bits must be a multiple of 8.");

	static _STD_API bool _IsX64 = std::hardware_destructive_interference_size == 64 ? true : false;
	static _STD_API size_t _NumberOfBytesNeeded = N / (_IsX64 ? 8 : 4);

	using StorageType = _ByteStorage<_NumberOfBytesNeeded>;
private:
	StorageType _Storage;
public:
	_STD_API std::uint8_t* storage() noexcept {
		return _Storage.storage;
	}
};

_STD_API_END