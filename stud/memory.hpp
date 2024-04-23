
#ifndef _STD_MEMORY

#include <utility>
#include <dbghelp.h>
#include <Windows.h>
#include <functional>

#include "forward.hpp"
#include "panic.hpp"
#include "io.hpp"
#include "utility.hpp"

_STD_DETAIL_API

template<class _Ty>
class _Default_destroy {
public:
    // do nothing, let the constructor happen by default.
    _STD_API static void destruct(_Ty* ptr) noexcept {
        if (!ptr) return;
        delete ptr;
    }
};

template<class _Ty, class _Close = _Default_destroy<_Ty>>
class _SOC_base {
private:
    _Ty* __data{ NULL };
public:
    _STD_API _SOC_base() noexcept = default;

    _STD_MAKE_NONCOPYABLE(_SOC_base);
    _STD_MAKE_NONMOVEABLE(_SOC_base);

    _STD_API _SOC_base(_Ty* __resource) noexcept
        : __data{ __resource }
    {}

    _STD_API ~_SOC_base() noexcept {
        _Close::destruct(__data);
    }

protected:
    _STD_API _Ty* _Get() noexcept {
        return __data;
    }
    _STD_API const _Ty* _Get() const noexcept {
        return __data;
    }

    _STD_API _Ty* _Release() noexcept {
        auto tmp = __data;
        __data = nullptr;
        return tmp;
    }
};

template<class T, class Deleter>
using _Standard_object_control = _SOC_base<T, Deleter>;

_STD_API_END

_STD_API_BEGIN

template<class T>
using DefaultDelete = _DETAIL _Default_destroy<T>;

template<class T, class Deleter = DefaultDelete<T>>
class UniquePtr : public _DETAIL _Standard_object_control<T, Deleter> {
public:
    using Control = _DETAIL _Standard_object_control<T, Deleter>;

    _STD_MAKE_NONCOPYABLE(UniquePtr);

    _STD_API UniquePtr(UniquePtr&& other) noexcept
        : Control(other.release())
    {}
    _STD_API UniquePtr(T* now_owned_by_this_ptr) noexcept
        : Control(now_owned_by_this_ptr)
    {}

    _STD_API UniquePtr(const T& value) noexcept {
        Control(new T(value));
    }
    template<typename ...Ts>
    _STD_API UniquePtr(Ts&&... args) noexcept
        : Control(new T{ std::forward<Ts>(args)... })
    {}

    _STD_API UniquePtr() noexcept
        : Control()
    {}

    _NODISCARD _STD_API T* get() noexcept {
        T* _Ptr = Control::_Get();
        panic(IF_NOT(_Ptr), "attempt to dereference a nullptr. (was the smart pointer released?)");
        return _Ptr;
    }
    _NODISCARD _STD_API const T* get() const noexcept {
        return Control::_Get();
    }

    _NODISCARD _STD_API T* release() noexcept {
        return Control::_Release();
    }

    _NODISCARD _STD_API T& operator*() noexcept {
        auto* _Ptr = get();
        panic(IF_NOT(_Ptr), "cannot dereference a nullptr");
        return *_Ptr;
    }
    _NODISCARD _STD_API const T& operator*() const noexcept {
        auto* _Ptr = get();
        panic(IF_NOT(_Ptr), "cannot dereference nullptr");
        return *_Ptr;
    }

    _NODISCARD _STD_API bool is_null() const noexcept {
        return get() == nullptr;
    }

    _NODISCARD _STD_API T* operator->() noexcept {
        return get();
    }
    _NODISCARD _STD_API const T* operator->() const noexcept {
        return get();
    }

    _NODISCARD static UniquePtr<T, Deleter> null() noexcept {
        return UniquePtr<T, Deleter>();
    }
};

template<class T, typename ...Ts>
_STD_API UniquePtr<T> make_unique(Ts&&... args) noexcept {
    return UniquePtr<T>(std::forward<Ts>(args)...);
}

_STD_INLINE void memcpy(void* dst, const void* src, size_t len) noexcept {
    char* _Dst = reinterpret_cast<char*>(dst);
    const char* _Src = reinterpret_cast<const char*>(src);

    for (size_t _Offset = 0; _Offset < len; ++_Offset) {
        _Dst[_Offset] = _Src[_Offset];
    }
}
_STD_INLINE void memset(void* destination, uint8_t value, size_t count) {
    char* _Dst = reinterpret_cast<char*>(destination);
    while (count--) {
        _Dst[count] = value;
    }
}

_STD_INLINE uintptr_t get_current_process_base() noexcept {
    HMODULE base = GetModuleHandleA(NULL);
    return reinterpret_cast<uintptr_t>(base);
}

inline BOOL is_heap_allocated(void* ptr) noexcept {
    auto base = get_current_process_base();
    auto address = reinterpret_cast<uintptr_t>(ptr);
    PVOID stack_frames[1024] = {};
    auto frame_count = RtlCaptureStackBackTrace(0, 1024, stack_frames, NULL);

    auto stack_begin_address = 
        base + reinterpret_cast<uintptr_t>(*stack_frames);
    auto stack_end_address = 
        base + reinterpret_cast<uintptr_t>(*(stack_frames + (frame_count - 1)));

    eprintln("Range: (0x{:x} -> 0x{:x}) Value: 0x{:x}", stack_begin_address, stack_end_address, (uintptr_t)ptr);

    if (address >= stack_begin_address && address <= stack_end_address)
        return false;

    return true;
}

template <class T>
inline UniquePtr<T> smuggle(T* ptr) noexcept {
    if (ptr == NULL)
        return UniquePtr<T>::null();
    if (is_heap_allocated(ptr)) {
        return make_unique(ptr);
    }
    return make_unique(std::move(*ptr));
}

template <size_t N>
_STD_API size_t strlen_s(const char(&s)[N]) noexcept {
    return N;
}

_STD_INLINE size_t strlen(const char* s) noexcept {
    // We just entirely rely on the standard here.
    // No need to reinvent the wheel.
    return _STD strlen(s);
}

template <class T>
class MutexProtectedAllocation {
private:
    T* _Ptr;
    Mutex _Mtx;
public:
    MutexProtectedAllocation() noexcept
        : _Mtx(Mutex::create()) {
        _Ptr = malloc<T>();
    }
    MutexProtectedAllocation(T&& init) noexcept 
        : _Mtx(Mutex::create()) {
        _Ptr = malloc<T>(std::forward<T>(init));
    }

    ~MutexProtectedAllocation() noexcept {
        free<T>(_Ptr);
    }

    _STD_INLINE const T* const get() const noexcept {
        return _Ptr;
    }
    void edit(const std::function<void(T*)>& edit) noexcept {
        __with_mutex(_Mtx, [edit, ptr = _Ptr]() { edit(ptr); });
    }

    _STD_INLINE T operator *() noexcept {
        return *_Ptr;
    }
    _STD_INLINE T operator *() const noexcept {
        return *_Ptr;
    }
};

template <class T>
MutexProtectedAllocation<T> mpalloc(std::optional<T> initializer = std::nullopt) noexcept {
    if (initializer) {
        return MutexProtectedAllocation<T>(std::move(initializer.value()));
    }
    return MutexProtectedAllocation<T>();
}

_STD_API_END

#define _STD_MEMORY
#endif