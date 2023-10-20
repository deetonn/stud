
#ifndef _STD_CLONE

#include <cstring>

#include "forward.hpp"

_STD_API_BEGIN

template<typename T>
class Duplicate {
public:
    _NODISCARD virtual _STD_API T clone() const noexcept {
        T result;
        std::memcpy(&result,
            this, sizeof(T));
        return result;
    }
};

_STD_API_END

#define _STD_CLONE
#endif