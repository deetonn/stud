
#ifndef _STD_ARRAY

#include <initializer_list>
#include <format>
#include <string>

#include "forward.hpp"
#include "panic.hpp"
#include "stddef.hpp"
#include "concept.hpp"
#include "type_traits.hpp"
#include "iterator.hpp"

_STD_API_BEGIN

template <class T, size_t N>
class Array;

//template<class _Ty, std::size_t _Size>
//class iterator<Array<_Ty, _Size>> {
//private:
//    _Ty* _ptr;
//public:
//    using difference_type = ptrdiff;
//    using value_type = _Ty;
//    using pointer = _Ty*;
//    using reference = _Ty&;
//    using iterator_category = std::forward_iterator_tag;
//
//    _STD_API _Array_iterator(_Ty* pointer) noexcept
//        : _ptr{ pointer }
//    {}
//
//    _STD_API _Array_iterator& operator++() noexcept {
//        _ptr++;
//        return *this;
//    }
//    _STD_API _Array_iterator operator++(int) noexcept {
//        auto ret = *this;
//        (*this)++;
//        return ret;
//    }
//
//    _STD_API bool operator==(_Array_iterator other) const noexcept {
//        return _ptr == other._ptr;
//    }
//    _STD_API bool operator!=(_Array_iterator other) const noexcept {
//        return _ptr != other._ptr;
//    }
//
//    _Ty& operator*() noexcept {
//        return *_ptr;
//    }
//};
//
//template <class _Ty, std::size_t _Size>
//class reverse_iterator<Array<_Ty, _Size>> {
//    _Ty* back;
//public:
//    using difference_type = ptrdiff;
//    using value_type = _Ty;
//    using pointer = _Ty*;
//    using reference = _Ty&;
//    using iterator_category = std::forward_iterator_tag;
//
//
//};

template<class T, size_t N>
class Array : public Duplicate<Array<T, N>>
#if defined (_STD_IDENTIFY_OBJECTS_)
    , public identity<array<T, N>>
#endif
{
private:
    T _elems[N];
public:
    static constexpr size_t SIZE = N;

    using iterator = iterator<Array<T, N>>;
    using value_type = T;

    _STD_API Array() = default;
    _STD_API Array(std::initializer_list<T> elements) noexcept {
        panic(IF(elements.size() > N), "cannot initialize an array<T, {}> with an initializer of length {}", N, elements.size());
        copy_to<N>(_elems, elements);
    }
    _STD_API Array(const T& initializer) noexcept {
        auto _Inner = data();
        for (size_t offset = 0; offset < N; ++offset) {
            _Inner[offset] = initializer;
        }
    }
    _STD_API Array(const T(&init)[N]) noexcept {
        for (size_t off = 0; off < N; ++off) {
            _elems[off] = init[off];
        }
    }

    _NODISCARD _STD_API T& at(size_t offset) noexcept {
#if defined (_DEBUG)
        panic(IF(offset > N), "offset out of range: ({} > {})", offset, N);
#endif
        return _elems[offset];
    }
    _NODISCARD _STD_API const T& at(size_t offset) const noexcept {
#if defined (_DEBUG)
        panic(IF(offset > N), "offset out of range: ({} > {})", offset, N);
#endif
        return _elems[offset];
    }

    _STD_API size_t insert(size_t pos, const T& value) noexcept {
        panic(IF(pos > size()), "cannot insert at position ({}) with an array of length ({})", pos, N);
        _elems[pos] = value;
        return pos;
    }

    _STD_API void fill(const T& element) noexcept {
        for (size_t index = 0; index < N; ++index) {
            _elems[index] = element;
        }
    }

    _NODISCARD _STD_API size_t size() const noexcept {
        return N;
    }
    _NODISCARD _STD_API size_t capacity() const noexcept {
        return N;
    }

    _NODISCARD _STD_API T* data() noexcept {
        return _elems;
    }
    _NODISCARD _STD_API const T* data() const noexcept {
        return _elems;
    }

    _NODISCARD _STD_API T& operator[](const size_t pos) noexcept {
#if defined (_DEBUG)
        panic(IF(pos > N), "out of bounds subscript ({} > {} <- array length)", pos, N);
#endif
        return _elems[pos];
    }
    _NODISCARD _STD_API const T& operator[](const size_t pos) const noexcept {
#if defined (_DEBUG)
        panic(IF(pos > N), "out of bounds subscript ({} > {} <- array length)", pos, N);
#endif
        return _elems[pos];
    }

    _NODISCARD _STD_API iterator begin() noexcept {
        return iterator(data());
    }
    _NODISCARD _STD_API iterator end() noexcept {
        return iterator(data() + N);
    }

    /// <summary>
    /// Linear search for an item that matches the parameter.
    /// </summary>
    /// <param name="item"></param>
    /// <returns></returns>
    _NODISCARD _STD_API bool contains(const T& item) const {
        for (std::size_t i = 0; i < this->size(); ++i) {
            const T& _This_item = this->at(i);
            if (_This_item == item)
                return true;
        }
        return false;
    }

    _STD_API std::string to_string()
    {
        auto result = std::string{"["};
        for (const auto& element : *this) {
            if constexpr (ToString<T>) {
                result.append(std::format("{}, ", element.to_string()));
            }
            if constexpr (is_pointer_v<T>) {
                result.append(std::format("{}, ", (const void*)element));
            }
            else {
                result.append(std::format("{}, ", element));
            }
        }

        result.append("]");
        return result;
    }
};

template<class ...Ts>
_STD_API auto make_array(Ts&&... args) noexcept {
    return Array<
        std::common_type_t<Ts...>,
        sizeof...(Ts)
    >({ std::forward<Ts>(args)... });
}

_STD_API_END

#define _STD_ARRAY
#endif