
#ifndef _STD_DEF

#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "forward.hpp"

_STD_API_BEGIN

using usize = std::size_t;

using int32 = std::int32_t;
using uint32 = std::uint32_t;

using int64 = std::int64_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

using uintptr = std::uintptr_t;
using ptrdiff = std::ptrdiff_t;

template<class T>
using array_of = T*;

template<class Into, class From>
_NODISCARD _STD_API bool memcpy(Into* dest, const From* from) noexcept {
    if (sizeof(Into) < sizeof(From)) {
        return false;
    }
    for (ptrdiff offset = 0; offset < sizeof(From); ++offset) {
// disable warnings for narrowing conversions here, 
// we already check against above, aswell as in memcpy_s
// so there is no worry.
#pragma warning(disable : 4244)
#pragma warning(push)
        dest[offset] = from[offset];
#pragma warning(pop)
    }
    return true;
}

template<class Into, class From>
_STD_API void memcpy_s(Into* dest, const From* src) noexcept {
    static_assert(sizeof(Into) >= sizeof(From), "Into type must be larger or equal to the From type.");
    DISCARD(memcpy(dest, src));
}

template<size_t N, class T, class It>
_STD_API void copy_to(T dest[N], It container) noexcept {
    size_t offset = 0;
    for (const auto& element : container) {
        dest[offset++] = element;
    }
}

_STD_API_END

#define _STD_DEF
#endif