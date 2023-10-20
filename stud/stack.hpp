
#ifndef _STD_STACK

#include <initializer_list>
#include <algorithm>
#include <iterator>

#include "forward.hpp"
#include "panic.hpp"

_STD_API_BEGIN

template<class T, size_t N>
class StaticStack {
protected:
    T _stack[N];
    size_t _pos{ 0 };
public:

    // we do not initialize anything in the stack itself.
    _STD_API StaticStack() noexcept = default;
    _STD_API StaticStack(std::initializer_list<T> init) noexcept {
        panic(IF(init.size() > N), "cannot initialize a stack of size ({}) to an array of elements of size ({})", N, init.size());
        for (const auto& element : init) {
            _stack[_pos++] = element;
        }
    }
    _STD_API StaticStack(std::nullptr_t)
        : StaticStack()
    {}

    _STD_API T* push(T&& value) noexcept {
        panic(IF(++_pos > N), "static_stack: limit exceeded");
        _stack[_pos] = std::move(value);
        return &_stack[_pos];
    }

    template<typename... Ts>
    _STD_API T* emplace(Ts&&... args) _STD_NOEXCEPT(T(std::forward<Ts>(args)...)) {
        panic(IF(++_pos > N), "static_stack: limit exceeded.");
        _stack[_pos] = T(std::forward<Ts>(args)...);
        return &_stack[_pos];
    }

    _NODISCARD _STD_API T&& pop() noexcept {
        panic(IF(_pos == 0), "cannot pop from an empty stack.");
        return std::move(_stack[(_pos--) - 1]);
    }

    _NODISCARD _STD_API T* peek() noexcept {
        panic(IF(_pos == 0), "cannot peek into an empty stack.");
        return &_stack[_pos];
    }
    _NODISCARD _STD_API const T* peek() const noexcept {
        panic(IF(_pos == 0), "cannot peek into an empty stack.");
        return &_stack[_pos];
    }

    _NODISCARD _STD_API T* data() noexcept {
        return &_stack;
    }

    _NODISCARD _STD_API T* front() noexcept {
        return _stack;
    }
    _NODISCARD _STD_API const T* front() const noexcept {
        return _stack;
    }

    _NODISCARD _STD_API T* back() noexcept {
        return _stack + N;
    }
    _NODISCARD _STD_API const T* back() const noexcept {
        return _stack + N;
    }

    _NODISCARD _STD_API size_t size() const noexcept {
        return N;
    }
    _NODISCARD _STD_API size_t capacity() const noexcept {
        return N;
    }
};

_STD_API_END

#define _STD_STACK
#endif