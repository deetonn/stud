
#ifndef _STD_OS

#include <string>
#include <algorithm>

#include "forward.hpp"

#include "result.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

#include "_os_file_info.hpp"
#include "_os_environment.hpp"

_STD_API_BEGIN

enum class ToNativePathErr {
    CannotCalculateDrive,
};

_STD_API
Result<placeholder, ToNativePathErr>
to_native_path(std::string& path) noexcept {
    constexpr bool is_windows = _DETAIL __is_windows;
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

/// <summary>
/// Determine whether a file extension (including the dot) is a windows only file.
/// (This is never guaranteed as extensions aren't always entirely transparent with what their file
/// type actually is)
/// </summary>
_STD_API 
bool 
likely_windows_file(std::string_view fileNameOrPath)
{
    constexpr auto _Array_items = stud::make_array<std::string_view>(".exe", ".sys", ".dll");

    auto& path = fileNameOrPath;
    auto dot_position = path.find('.');

    if (dot_position == std::string::npos)
        return false;

    auto extension = path.substr(dot_position, path.size() - dot_position + 1);
    return _Array_items.contains(extension);
}

_STD_API
__forceinline
bool
handle_is_valid(HANDLE handle) noexcept {
    return handle != INVALID_HANDLE_VALUE;
}

enum FileAccess : DWORD {
    FileAccess_All = 0x10000000,
    FileAccess_Execute = 0x20000000,
    FileAccess_Write = 0x40000000,
    FileAccess_Read = 0x80000000,

    FileAccess_ReadWrite = (FileAccess_Read | FileAccess_Write),
};
enum FileShareAccess {
    FileShare_NoAccess = 0x0,
    FileShare_Delete = 0x00000004,
    FileShare_Read = 0x00000001,
    FileShare_Write = 0x00000002,

    FileShare_ReadWrite = (FileShare_Read | FileShare_Write),
    FileShare_FullShare = (0x00000001 | 0x00000002 | 0x00000004)
};
enum FileDisposition {
    FileDisposition_CreateAlways = 0x2,
    FileDisposition_CreateNew = 0x1,
    FileDisposition_OpenAlways = 0x4,
    FileDisposition_OpenExisting = 0x3,
    FileDisposition_TruncateExisting = 0x5,
};
enum FileAttribute {
    FileAttribute_Archive = 0x20,
    FileAttribute_Encrypted = 0x4000,
    FileAttribute_Hidden = 0x2,
    FileAttribute_Normal = 0x80,
    FileAttribute_Offline = 0x1000,
    FileAttribute_Readonly = 0x1,
    FileAttribute_System = 0x4,
    FileAttribute_Temporary = 0x100
};

class FileOpenOptions {
private:
    std::string_view _Path;
    DWORD _DesiredAccess{ FileAccess_Read };
    // Default this so files arent locked per process
    DWORD _ShareMode{ FILE_SHARE_WRITE | FILE_SHARE_READ };
    DWORD _CreationDispostion{ NULL };
    DWORD _FlagsNAttrs{ NULL };
public:
    // CreateFileA - lpFilePath
    FileOpenOptions& path(const std::string_view path) noexcept {
        _Path = path;
        return *this;
    }
    // CreateFileA - dwDesiredAccess
    FileOpenOptions& access(FileAccess flags) noexcept {
        _DesiredAccess = static_cast<DWORD>(flags);
        return *this;
    }
    // CreateFileA - dwShareMode
    FileOpenOptions& share(FileShareAccess share) noexcept {
        _ShareMode = static_cast<DWORD>(share);
        return *this;
    }
    // CreateFileA - dwCreationDisposition
    FileOpenOptions& disposition(FileDisposition disp) noexcept {
        _CreationDispostion = static_cast<DWORD>(disp);
        return *this;
    }
    // CreateFileA - dwFlagsAndAttributes
    FileOpenOptions& attributes(FileAttribute attrs) noexcept {
        _FlagsNAttrs = static_cast<DWORD>(attrs);
        return *this;
    }

    // Create or open the file
    Result<HANDLE, std::exception> open() {
        panic(IF(_Path.empty()), "cannot open a file without a filename set.");
        auto handle = CreateFileA(_Path.data(),
            _DesiredAccess,
            _ShareMode,
            NULL,
            _CreationDispostion,
            _FlagsNAttrs,
            NULL);
        if (!handle_is_valid(handle))
            return std::exception("CreateFileA returned an invalid handle");
        return handle;
    }
};

_STD_INLINE
Result<HANDLE, std::exception>
open_file(const std::string_view path) noexcept {
    return FileOpenOptions()
        .path(path)
        .access(FileAccess_Read)
        .share(FileShare_FullShare)
        .disposition(FileDisposition_OpenExisting)
        .attributes(FileAttribute_Normal)
        .open();
}

_STD_INLINE
Result<HANDLE, std::exception>
create_file(const std::string_view path) noexcept {
    return FileOpenOptions()
        .path(path)
        .access(FileAccess_ReadWrite)
        .share(FileShare_ReadWrite)
        .disposition(FileDisposition_CreateNew)
        .attributes(FileAttribute_Normal)
        .open();
}

template <class ...Ts>
_STD_INLINE
int 
system(const std::format_string<Ts...> command_format, Ts&&... args) noexcept
{
    return std::system(
        stud::format(
            command_format, 
            std::forward<Ts>(args)...
        ).c_str()
    );
}

#ifndef _STD_DISABLE_ABORTS
// If set to 1, stud::exit() & stud::exit(code) will not exit.
#define _STD_DISABLE_ABORTS 0
#endif

_STD_NORETURN
_STD_INLINE
void 
exit() _STD_NOEXCEPT(std::abort()) {
#if !_STD_DISABLE_ABORTS
    std::abort();
#endif
}

_STD_NORETURN
_STD_INLINE
void 
exit(int code) _STD_NOEXCEPT(std::exit(0)) {
#if !_STD_DISABLE_ABORTS
    std::exit(code);
#endif
}

void* unaligned_malloc(const size_t byte_count) noexcept {
    return _STD malloc(byte_count);
}

void* mxpand(void* malloced_mem, size_t expansion_in_bytes) noexcept {
    return realloc(malloced_mem, expansion_in_bytes);
}

template <class T>
T* malloc(std::optional<T> initializer = std::nullopt) noexcept {
    auto* space = unaligned_malloc(sizeof(T));
    if (initializer.has_value()) {
        auto&& our_init = std::move(initializer.value());
        std::memcpy(space, &our_init, sizeof(T));
    }
    return reinterpret_cast<T*>(space);
}

template <class T>
void free(T* ptr) noexcept {
    _STD free((void*)ptr);
}

_STD_API_END

#if defined(_WIN32) || defined(_WIN64)
#include "stud_windefs.h"
#endif

#define _STD_OS
#endif