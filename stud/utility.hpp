
#ifndef _STD_UTILITY

#include <utility>
#include <atomic>
#include <functional>

#include "forward.hpp"
#include "io.hpp"

_STD_API_BEGIN

template<class T>
typename std::remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename std::remove_reference<T>::type&&>(t);
}

// non-allocating indirect construction.
template<class T, typename... Args>
_STD_API void construct_at(void* location, Args&&... args) noexcept {
    T* _Loc = reinterpret_cast<T*>(location);
    T  _Obj = T(std::forward<Args>(args)...);
    *_Loc = move(_Obj);
}

template<class _Ty>
concept MutexLike = requires(_Ty inst) {
    { inst.lock() } -> std::same_as<void>;
    { inst.unlock() } -> std::same_as<void>;
};

class Mutex {
private:
    std::atomic_flag __lock;
public:
    _STD_INLINE Mutex() noexcept {
        __lock.clear();
    }
    _STD_API Mutex(const Mutex&) noexcept = delete;
    _STD_API Mutex& operator=(const Mutex&) noexcept = delete;
    _STD_API Mutex(Mutex&&) noexcept = delete;

    inline void lock() noexcept {
        if (__lock.test())
            __lock.wait(false);
        __lock.test_and_set();
    }

    inline void unlock() noexcept {
        panic(IF(!__lock.test_and_set()), "cannot unlock an unlocked mutex.");
        __lock.clear();
    }

    _STD_INLINE static Mutex create() noexcept {
        return Mutex();
    }
};

static void __with_mutex(Mutex& mtx, const std::function<void()>& fn) noexcept {
    mtx.lock();
    fn();
    mtx.unlock();
}

#define INTO_VOID(body) [&]() body
#define GUARD(mtx, body) _STUD __with_mutex(mtx, INTO_VOID(body))

template<MutexLike T>
struct GenericMutexLock final {
private:
    T* __ptr;
public:
    _STD_API GenericMutexLock() noexcept = delete;
    _STD_API GenericMutexLock(const GenericMutexLock&) noexcept = delete;
    _STD_API GenericMutexLock& operator=(const GenericMutexLock&) noexcept = delete;
    _STD_API GenericMutexLock(GenericMutexLock&&) noexcept = delete;

    _STD_API GenericMutexLock(T* ptr) noexcept
        : __ptr{ ptr }
    {
        if (__ptr) {
            ptr->lock();
        }
    }
    _STD_API ~GenericMutexLock() noexcept {
        DISCARD(this->release());
    }

    _STD_API T* release() noexcept {
        if (__ptr) {
            auto* copy = __ptr;
            __ptr->unlock();
            __ptr = nullptr;
            return copy;
        }
        return nullptr;
    }
};

template<class To, class From>
To&& bit_cast(From&& from) noexcept {
    static_assert(sizeof(To) >= sizeof(From), "To's size must be larger than or equal to the size of From.");
    union {
        std::decay_t<From> __from;
        To __to;
    } bits;
    bits.__from = move(from);
    return move(bits.__to);
}

#define __STUD___INTERNAL_DEFAULT_BRANCH "main"
 
class Version {
    size_t _Major, _Minor, _Others;
    std::string_view _Branch;
public:
    _STD_API Version(size_t major, size_t minor, size_t others = 0, std::string_view branch = __STUD___INTERNAL_DEFAULT_BRANCH) noexcept {
        _Major = major;
        _Minor = minor;
        _Others = others;
        _Branch = branch;
    }

    _STD_API static Version normal(size_t major, size_t minor, std::string_view branch = __STUD___INTERNAL_DEFAULT_BRANCH) noexcept {
        return Version(major, minor, 0, branch);
    }

    _STD_API size_t major() const noexcept { return _Major; }
    _STD_API size_t minor() const noexcept { return _Minor; }
    _STD_API size_t others() const noexcept { return _Others; }
    _STD_API std::string_view branch() const noexcept { return _Branch; }

    _STD_API std::string to_string() const noexcept {
        if (_Others) {
            return stud::format("{}.{}.{}-{}", major(), minor(), others(), branch());;
        }
        return stud::format("{}.{}-{}", major(), minor(), branch());
    }
};

auto make_version(size_t major, size_t minor, std::string_view branch) noexcept {
    return Version::normal(major, minor, branch);
}
auto make_complex_version(size_t major, size_t minor, size_t others, std::string_view branch) noexcept {
    return Version(major, minor, others, branch);
}

template <class T>
_STD_API void swap(T& left, T& right) noexcept {
    // Explicit type to force a copy.
    T _Tmp = left;
    left = _Tmp;
    right = _Tmp;
}

template <class T, class U = T>
_STD_API T exchange(T& original, U&& new_value) noexcept {
    // Explicit type to force a copy.
    T _Old = original;
    original = move(new_value);
    return _Old;
}

template <class T>
_STD_API auto forward(std::remove_reference_t<T>& type) noexcept {
    return static_cast<T&&>(type);
}

template <class T>
_STD_API const T& as_const(T& r) noexcept { return r; }

template <class T>
_STD_API const T* as_const(T* r) noexcept { return r; }

_STD_API_END

#define _STD_UTILITY
#endif