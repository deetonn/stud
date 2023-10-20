
#ifndef _STD_DEFER

#include "forward.hpp"

_STD_API_BEGIN

#define STRING_CONCAT_INNER(x, y) x##y
#define STRING_CONCAT(x, y) STRING_CONCAT_INNER(x, y)

template<typename F>
struct _NODISCARD defer_context final {
    F callback;
    _STD_API defer_context(F f) noexcept
        : callback(f)
    {}
    _STD_API ~defer_context() _STD_NOEXCEPT(callback()) {
        callback();
    }
};

#define defer stud::defer_context STRING_CONCAT(__defer_, __COUNTER__) = [&]() 

_STD_API_END

#define _STD_DEFER
#endif