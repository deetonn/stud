
#include "forward.hpp"
#include "array.hpp"
#include "io.hpp"
#include "time.hpp"

#include <io.h>

_STD_API_BEGIN

using FileInfoErrorMsg = struct _FileInfo_Err {
    std::string data;
};

class FileAttributes {
private:
    DWORD _Attrs;
public:
    inline explicit FileAttributes(DWORD d) noexcept
        : _Attrs(d) {}
    // FILE_ATTRIBUTE_READONLY
    // 1 (0x00000001)
    _NODISCARD bool is_readonly() const noexcept { return _Attrs & FILE_ATTRIBUTE_READONLY; }
    // FILE_ATTRIBUTE_HIDDEN
    // 2 (0x00000002)
    _NODISCARD bool is_hidden() const noexcept { return _Attrs & FILE_ATTRIBUTE_HIDDEN; }

    // FILE_ATTRIBUTE_SYSTEM
    // 4 (0x00000004)
    _NODISCARD bool is_system_file() const noexcept { return _Attrs & FILE_ATTRIBUTE_SYSTEM; }
    // FILE_ATTRIBUTE_DIRECTORY
    // 16 (0x00000010)
    _NODISCARD bool is_directory() const noexcept { return _Attrs & FILE_ATTRIBUTE_DIRECTORY; }

    // FILE_ATTRIBUTE_ARCHIVE
    // 32 (0x00000020)
    _NODISCARD bool is_archive() const noexcept { return _Attrs & FILE_ATTRIBUTE_ARCHIVE; }
    
    // FILE_ATTRIBUTE_NORMAL
    // 128 (0x00000080)
    _NODISCARD bool is_normal() const noexcept { return _Attrs & FILE_ATTRIBUTE_NORMAL; }

    // FILE_ATTRIBUTE_TEMPORARY
    // 256 (0x00000100)
    _NODISCARD bool is_temporary() const noexcept { return _Attrs & FILE_ATTRIBUTE_TEMPORARY; }
    // FILE_ATTRIBUTE_SPARSE_FILE
    // 512 (0x00000200)
    _NODISCARD bool is_sparse_file() const noexcept { return _Attrs & FILE_ATTRIBUTE_SPARSE_FILE; }
    // FILE_ATTRIBUTE_REPARSE_POINT
    // 1024 (0x00000400)
    _NODISCARD bool is_reparse_point() const noexcept { return _Attrs & FILE_ATTRIBUTE_REPARSE_POINT; }

    // FILE_ATTRIBUTE_COMPRESSED
    // 2048 (0x00000800)
    _NODISCARD bool is_compressed() const noexcept { return _Attrs & FILE_ATTRIBUTE_COMPRESSED; }
    // FILE_ATTRIBUTE_OFFLINE
    // 4096 (0x00001000)
    _NODISCARD bool is_offline() const noexcept { return _Attrs & FILE_ATTRIBUTE_OFFLINE; }
    // FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
    // 8192 (0x00002000)
    _NODISCARD bool is_not_content_indexed() const noexcept { return _Attrs & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED; }
    // FILE_ATTRIBUTE_ENCRYPTED
    // 16384 (0x00004000)
    _NODISCARD bool is_encrypted() const noexcept { return _Attrs & FILE_ATTRIBUTE_ENCRYPTED; }
    // FILE_ATTRIBUTE_INTEGRITY_STREAM
    // 32768 (0x00008000)
    _NODISCARD bool is_content_stream() const noexcept { return _Attrs & FILE_ATTRIBUTE_INTEGRITY_STREAM; }
    // FILE_ATTRIBUTE_NO_SCRUB_DATA (this check is inverted)
    // 131072 (0x00020000)
    _NODISCARD bool should_scrub_data() const noexcept { return !(_Attrs & FILE_ATTRIBUTE_NO_SCRUB_DATA); }
    // FILE_ATTRIBUTE_PINNED
    // 524288 (0x00080000)
    _NODISCARD bool is_pinned() const noexcept { return _Attrs & FILE_ATTRIBUTE_PINNED; }
    // FILE_ATTRIBUTE_UNPINNED
    // 1048576 (0x00100000)
    _NODISCARD bool is_unpinned() const noexcept { return _Attrs & FILE_ATTRIBUTE_UNPINNED; }
};

class FileInfo {
public:
    inline FileInfo(const std::string& path) {
        _ProcessFile(path);
    }
    inline FileInfo() noexcept {
        this->_Attributes = NULL;
        this->_FileCreationTime = DateTime::none();
        this->_FileLastAccessTime = DateTime::none();
        this->_FileLastModifiedTime = DateTime::none();
        this->_FileSize = 0;
        this->_LinkCount = 0;
    }

    const DateTime& creation_time() const noexcept { return _FileCreationTime; }
    const DateTime& last_modified_time() const noexcept { return _FileLastModifiedTime; }
    const DateTime& last_access_time() const noexcept { return _FileLastAccessTime; }

    FileAttributes attributes() const noexcept { return FileAttributes(_Attributes); }
    DWORD symlink_count() const noexcept { return _LinkCount; }

    long long size() const noexcept { return _FileSize; }

    inline static Result<FileInfo, FileInfoErrorMsg> open(const std::string& path) noexcept {
        try {
            return FileInfo(path);
        }
        catch (std::string err) {
            return FileInfoErrorMsg{
                .data = std::move(err)
            };
        }
    }

private:
    DateTime _FileCreationTime;
    DateTime _FileLastModifiedTime;
    DateTime _FileLastAccessTime;

    DWORD _Attributes;
    DWORD _LinkCount;
    long long _FileSize;

    void _ProcessFile(std::string_view path)
    {
        // NOTE: \\?\ makes allows a path to exceed the MAX_PATH limit.
        if (path.length() > MAX_PATH && !path.starts_with("\\\\?\\")) {
            throw format("The file exceeds the windows path length");
        }

        HANDLE handle = CreateFileA(
            path.data(), 
            GENERIC_READ,
            (FILE_SHARE_READ | FILE_SHARE_WRITE),
            NULL, 
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (handle == INVALID_HANDLE_VALUE) {
            throw stud::format("Failed to open the file (\"{}\") (invalid handle returned) (LastErr={})", path, GetLastError());
        }

        BY_HANDLE_FILE_INFORMATION file_info = {};

        if (!GetFileInformationByHandle(handle, &file_info)) {
            throw format("Failed to get file information! (LastErr={})", GetLastError());
        }

        _FileCreationTime = DateTime(&file_info.ftCreationTime);
        _FileLastModifiedTime = DateTime(&file_info.ftLastWriteTime);
        _FileLastAccessTime = DateTime(&file_info.ftLastAccessTime);
        _Attributes = file_info.dwFileAttributes;
        _LinkCount = file_info.nNumberOfLinks;

        struct _stat64 stat;
        if (_stat64(path.data(), &stat) == 0) {
            _FileSize = stat.st_size;
        }
        else {
            eprintln("Failed to get file size ({}) (see {}:{})", GetLastError(), __FILE__, __LINE__);
            _FileSize = 0;
        }

        CloseHandle(handle);
    }
};

_STD_API_END