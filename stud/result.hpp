
#ifndef _STD_RESULT

#include <type_traits>
#include <variant>

#include "forward.hpp"

_STD_UNSTABLE_API_BEGIN

template<class T, class E>
union result_storage {
    static_assert(std::is_default_constructible_v<E>, "result<T, E>: E must be default constructable");

    _STD_API result_storage() noexcept {
        err = {};
    }
    _STD_API ~result_storage() noexcept {
        
    }

    T ok;
    E err;
};

template<typename T, typename E>
class _Move_only_result {
private:
    result_storage<T, E> storage;
    bool _has_value;
public:
    static_assert(sizeof(T) != sizeof(E), "result<T, E>: same size types causes UB.");
    static_assert(std::is_destructible_v<T>, "result<T, E>: result type must be trivially destructable.");
    static_assert(std::is_destructible_v<E>, "result<T, E>: result error must be trivially destructable.");

    using Self = _Move_only_result<T, E>;

    _Move_only_result() = delete;
    _Move_only_result(const _Move_only_result&) = delete;
    _Move_only_result operator=(const _Move_only_result&) = delete;

    ~_Move_only_result() {
        NOOP();
    }

    _Move_only_result(_Move_only_result&& other) noexcept {
        if (other._has_value) {
            this->storage = std::move(other.storage.value);
        }
        else {
            this->storage = std::move(other.storage.error);
        }
        this->_has_value = other._has_value;
    }

    _Move_only_result(T&& success) _STD_NOEXCEPT(std::move(success))
        : _has_value{ true }
    {
        storage.ok = std::move(success);
    }

    _Move_only_result(E&& error) _STD_NOEXCEPT(std::move(error))
        : _has_value{ false }
    {
        storage.err = std::move(error);
    }

    _STD_API bool is_ok() const noexcept {
        return _has_value;
    }

    _STD_API T get() const noexcept {
        return std::move(storage.ok);
    }

    _STD_API E get_err() const noexcept {
        return std::move(storage.err);
    }
};

template<typename T, typename E>
using _Stud_result = _Move_only_result<T, E>;

template<typename T, typename E>
class _STD_NODISCARD result : public _Stud_result<T, E> {
public:
    using _Base = _Stud_result<T, E>;

    using _Base::_Base;

    _STD_API ~result() noexcept {
        if constexpr (std::is_same_v<_Base, _Move_only_result<T, E>>) {
            _Base::~_Move_only_result();
        }
    }

    using _Base::is_ok;
    using _Base::get;
    using _Base::get_err;
};

_STD_API_END

_STD_API_BEGIN

template<class T, class E>
class _STD_NODISCARD Result {
private:
    std::variant<T, E> m_variant;
public:
    _STD_API Result() noexcept = delete;
    _STD_API Result(T&& success) noexcept {
        m_variant = std::move(success);
    }
    _STD_API Result(E&& failure) noexcept {
        m_variant = std::move(failure);
    }

    _NODISCARD _STD_API bool is_okay() const noexcept {
        return std::get_if<0>(&m_variant) != nullptr;
    }
    _NODISCARD _STD_API bool is_err() const noexcept {
        return std::get_if<1>(&m_variant) != nullptr;
    }

    _NODISCARD _STD_API T get() const noexcept {
        return std::move(std::get<0>(m_variant));
    }
    _NODISCARD _STD_API E get_err() const noexcept {
        return std::move(std::get<1>(m_variant));
    }

    _NODISCARD _STD_API const T& view() const noexcept {
        return std::get<0>(m_variant);
    }
    _NODISCARD _STD_API const E& view_err() const noexcept {
        return std::get<1>(m_variant);
    }
};

struct placeholder {};
static constexpr placeholder nothing = {};

_STD_API_END

#define _STD_RESULT
#endif