
#ifndef _STD_FORWARD_HPP

#define _STD_API_BEGIN namespace stud {
#define _STD_UNSTABLE_API_BEGIN namespace stud::unsafe {
#define _STD_DETAIL_API namespace stud::__detail {
#define _STD_API_END }

#define _STD_INLINE inline
#define _STD_CONSTEXPR constexpr

#define _STD_API inline constexpr

#define _STD_DISABLED_WARNINGS_LIST 4244

#define _STD_DISABLE_WARNINGS __pragma(warning (disable : _STD_DISABLED_WARNINGS_LIST)); __pragma(warning (push));
#define _STD_ENABLE_WARNINGS __pragma(warning (pop));

#define _STD_NODISCARD [[nodiscard]]
#define _STD_NORETURN [[noreturn]]

#define _STD_NOEXCEPT(expr) noexcept(noexcept(expr))

#define DISCARD(expr) ((void)expr)
#define NOOP() DISCARD(0)

#define _STUD ::stud::
#define _DETAIL ::stud::__detail::

#define _NODISCARD [[nodiscard]]

_STD_DETAIL_API

#ifdef _WIN32
_STD_API bool __is_windows = true;
#else
_STD_API bool __is_windows = false;
#endif

#define WIN_PAYLOAD if constexpr(_DETAIL __is_windows)

#if defined(__linux__) 
_STD_API bool __is_linux = true;
#else
_STD_API bool __is_linux = false;
#endif

#define LINUX_PAYLOAD if constexpr (_DETAIL __is_linux)

#if defined (__linux__) || defined (__unix__)
_STD_API bool __is_unix = true;
#else
_STD_API bool __is_unix = false;
#endif

#define UNIX_PAYLOAD if constexpr (_DETAIL __is_unix)

#if defined (__APPLE__) || defined(__MACH__)
_STD_API bool __is_macos = true;
#else
_STD_API bool __is_macos = false;
#endif

#define MAC_PAYLOAD if constexpr (_DETAIL __is_macos)

#if defined (__FreeBSD__)
_STD_API bool __is_freebsd = true;
#else
_STD_API bool __is_freebsd = false;
#endif

#if defined (__ANDROID__)
_STD_API bool __is_android = true;
#else
_STD_API bool __is_android = false;
#endif

_STD_API_END

_STD_API_BEGIN

#define SYSTEM_DISCOVERY_FUNCTION(name, builtin) \
_STD_API bool name() noexcept { \
  return _DETAIL builtin; \
}

SYSTEM_DISCOVERY_FUNCTION(is_windows, __is_windows);
SYSTEM_DISCOVERY_FUNCTION(is_linux, __is_linux);
SYSTEM_DISCOVERY_FUNCTION(is_unix, __is_unix);
SYSTEM_DISCOVERY_FUNCTION(is_macos, __is_macos);
SYSTEM_DISCOVERY_FUNCTION(is_free_bsd, __is_freebsd);
SYSTEM_DISCOVERY_FUNCTION(is_android, __is_android);

enum class System {
    windows, linux, unix, macos, free_bsd, android
};

_STD_API bool is_system(System system) noexcept {
    if (system == System::windows) {
        return is_windows();
    }
    else if (system == System::linux) {
        return is_linux();
    }
    else if (system == System::unix) {
        return is_unix();
    }
    else if (system == System::macos) {
        return is_macos();
    }
    else if (system == System::free_bsd) {
        return is_free_bsd();
    }
    else if (system == System::android) {
        return is_android();
    }
    else {
        return false;
    }
}

#define _STD_MAKE_NONCOPYABLE(type) _STD_API type(const type&) noexcept = delete; \
_STD_API type& operator=(const type&) noexcept = delete
#define _STD_MAKE_NONMOVEABLE(type) _STD_API type(type&&) noexcept = delete;

// The case of a string
enum class Case {
    // All lowercase - all lowercase
    Lower,
    // Words are correctly capitalized
    Capitalized,
    // All uppercase - ALL UPPERCASE
    Upper,
};

_STD_API_END

#define _STD_FORWARD_HPP
#endif