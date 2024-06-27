
#ifndef _STD_ALGO

#include <vector>

#include "forward.hpp"
#include "clone.hpp"

_STD_API_BEGIN

template<class T>
using _Algo_vector = std::vector<T>;

template<class L, class R>
struct pair {
    L key;
    R value;
};

template<class Container, typename T = typename Container::value_type>
_NODISCARD 
_STD_API 
_Algo_vector<pair<size_t, T&>> 
enumerate(Container& container) noexcept {
    _Algo_vector<pair<size_t, T&>> result = {};

    for (size_t index = 0; index < container.size(); ++index) {
        T& el = container.at(index);
        result.emplace_back(index, el);
    }

    return result;
}

_STD_API 
size_t 
size(const auto& container) noexcept {
    return container.size();
}

template<class T>
_NODISCARD 
_STD_API 
T 
clone(const Duplicate<T>& duplicate) noexcept {
    return duplicate.clone();
}

_STD_API_END

#define _STD_ALGO
#endif