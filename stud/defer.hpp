
#ifndef _STD_DEFER

#include "forward.hpp"

#include <future>
#include <chrono>

using namespace std::chrono_literals;

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
#define defer_stored(name) stud::defer_context name = [&]()

class _Async_context {
private:
    std::future<void> _M_future;
public:
    _Async_context(std::future<void>&& future) noexcept
        : _M_future(std::move(future))
    {}

    std::future<void>* _Get() noexcept { return &_M_future; }
    const std::future<void>* _Get() const noexcept { return &_M_future; }
    std::future<void>& get() noexcept { return *_Get(); }
    const std::future<void>& get() const noexcept { return *_Get(); }
};

class later {
private:
    _Async_context _M_ctx;
public:
    _STD_INLINE later(const std::function<void()>& _Fot) noexcept
        : _M_ctx(std::async(std::launch::async, _Fot))
    {}

    _STD_INLINE bool done(void) const noexcept {
        const auto* future = _M_ctx._Get();
        const auto status = future->wait_for(std::chrono::milliseconds(1));
        return status == std::future_status::ready;
    }

    _STD_INLINE static later run(const std::function<void()>& _Fot) noexcept {
        return later{ _Fot };
    }
};

later make_task(const std::function<void()>& __taskf) _STD_NOEXCEPT(later{ __taskf }) {
    return later{ __taskf };
}

template <class _Rep, class _Period = std::ratio<1>>
_STD_API later execute_after(const std::chrono::duration<_Rep, _Period> duration, const std::function<void()>& callback) {
    return make_task([duration, callback]() {
        std::this_thread::sleep_for(duration);
        callback();
    });
}

_STD_API_END

#define _STD_DEFER
#endif