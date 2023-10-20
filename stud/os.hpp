
#ifndef _STD_OS

#include <string>
#include <algorithm>

#include "forward.hpp"

#include "result.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

_STD_API_BEGIN

enum class ToNativePathErr {
    CannotCalculateDrive,
};

_STD_API
Result<placeholder, ToNativePathErr>
to_native_path(std::string& path) noexcept {
    const bool is_windows = _DETAIL __is_windows;
    if (path.substr(1, 2) == ":\\" && !is_windows) {
        path = path.substr(3, path.size());
    }
    
    if (!is_windows) {
        const char _Rep = is_windows ? '/' : '\\';
        const char _New = is_windows ? '\\' : '/';
        std::replace(
            std::begin(path),
            std::end(path),
            _Rep, _New
        );
    }

    if (is_windows && path.substr(1, 2) != ":\\") {
        // we cannot replace that.
        return ToNativePathErr::CannotCalculateDrive;
    }

    return stud::placeholder{};
}

_STD_API_END

#define _STD_OS
#endif