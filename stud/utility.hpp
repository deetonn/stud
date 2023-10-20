
#ifndef _STD_UTILITY

#include <utility>
#include <atomic>

#include "forward.hpp"

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
    _STD_API Mutex() noexcept = default;
    _STD_API Mutex(const Mutex&) noexcept = delete;
    _STD_API Mutex& operator=(const Mutex&) noexcept = delete;
    _STD_API Mutex(Mutex&&) noexcept = delete;

    inline void lock() noexcept {
        __lock.wait(false);
        __lock.test_and_set();
    }

    inline void unlock() noexcept {
        panic(IF(!__lock.test_and_set()), "cannot unlock an unlocked mutex.");
        __lock.clear();
    }
};

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

_STD_API_END

#define _STD_UTILITY
#endif