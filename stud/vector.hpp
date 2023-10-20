
#ifndef _STD_VECTOR_H

#include <new>
#include <initializer_list>
#include <vector>

#include "forward.hpp"
#include "utility.hpp"
#include "option.hpp"

_STD_API_BEGIN

// FIXME: .at() returns garbage.

template<class T>
class Vector {
public:
    using size_type = size_t;
    using pointer = T*;
    using const_pointer = const pointer;
    using reference = T&;
    using const_reference = const T&;
private:
    T* ptr_{ nullptr };
    size_t size_{ 0 };
    size_t cap_{ 0 };
public:
    _STD_API Vector() = default;
    _STD_API Vector(std::initializer_list<T> elems) noexcept {
        for (const auto element : elems) {
            push_back(element);
        }
    }

    _STD_API Vector(const Vector& other) noexcept {
        T* _Ptr = other.data();
        for (size_type offset = 0; offset < other.size(); ++offset) {
            this->push_back(*(_Ptr + offset));
        }
    }
    _STD_API Vector(Vector&& other) {
        auto* _Elems = other.drain();
        cap_ = other.cap_;
        size_ = other.size_;
        ptr_ = _Elems;
    }

    _STD_API ~Vector() noexcept {
        if (ptr_) ::free(ptr_);
    }

    _STD_API void push_back(const T& element) noexcept {
        _Verify_state();
        ptr_[size_] = element;
        size_++;
    }
    template<typename... Ts>
    _STD_API void emplace_back(Ts&&... args) noexcept {
        this->push_back(T{ std::forward<Ts>(args)... });
    }

    _STD_API reference at(size_type offset) noexcept {
        panic(IF(offset > size()), "cannot offset into vector at a position greater than the vectors size.");
        return ptr_[offset];
    }

    // transfer ownership of the internal data to the caller.
    _STD_API T* drain() noexcept {
        auto* _Copy = ptr_;
        ptr_ = nullptr;
        size_ = 0;
        cap_ = 0;
        return _Copy;
    }

    _STD_API size_type size() const noexcept {
        return size_;
    }
    _STD_API size_type capacity() const noexcept {
        return cap_;
    }

    _STD_API pointer data() noexcept {
        return ptr_;
    }
    _STD_API const_pointer data() const noexcept {
        return ptr_;
    }
private:

    _STD_API void _Verify_state() noexcept {
        if (!cap_) {
            // we are not yet initialized, we should allocate
            // some memory.
            ptr_ = static_cast<pointer>(::malloc(sizeof(T) * 2));
            cap_ = 2;
            return;
        }
        if (cap_ == size_) {
            // double in size.
            auto* _Old = ptr_;
            ptr_ = static_cast<pointer>(::malloc(cap_ * 2));
            cap_ = cap_ * 2;
            if (!ptr_) {
                ptr_ = static_cast<pointer>(::malloc(sizeof(T) * 2));
                cap_ = 2;
                return;
            }
            std::memcpy(ptr_, _Old, size_);
            ::free(_Old);
            return;
        }
    }
};

_STD_API_END

#define _STD_VECTOR_H
#endif