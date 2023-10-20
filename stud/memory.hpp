
#ifndef _STD_MEMORY

#include <utility>

#include "forward.hpp"
#include "panic.hpp"

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
    _Ty* __data;
public:
    _STD_API _SOC_base() noexcept = delete;

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

    _STD_API UniquePtr(const T& value) noexcept {
        Control(new T(value));
    }
    template<typename ...Ts>
    _STD_API UniquePtr(Ts&&... args) noexcept
        : Control(new T{ std::forward<Ts>(args)... })
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
};

template<class T, typename ...Ts>
_STD_API UniquePtr<T> make_unique(Ts&&... args) noexcept {
    return UniquePtr<T>(std::forward<Ts>(args)...);
}

inline void memcpy(void* dst, const void* src, size_t len) noexcept {
    char* _Dst = reinterpret_cast<char*>(dst);
    const char* _Src = reinterpret_cast<const char*>(src);

    for (size_t _Offset = 0; _Offset < len; ++_Offset) {
        _Dst[_Offset] = _Src[_Offset];
    }
}

inline void memset(void* destination, uint8_t value, size_t count) {
    char* _Dst = reinterpret_cast<char*>(destination);
    while (count--) {
        _Dst[count] = value;
    }
}

_STD_API_END

#define _STD_MEMORY
#endif