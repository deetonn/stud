
#ifndef _STD_TYPE_TRAITS

#include <type_traits>

#include "forward.hpp"

_STD_API_BEGIN

template<class T, T N>
class integral_constant {
public:
    _STD_API static T value = N;
};

template<bool B>
using bool_constant = integral_constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template<class T, class Other>
class is_same : public false_type {};

template<class T>
class is_same<T, T> : public true_type {};

template<class T, class T2>
_STD_API static bool is_same_v = is_same<T, T2>::value;

template<class T>
class is_array : public false_type {};

template<class T>
class is_array<T[]> : public true_type {};

template<class T>
_STD_API static bool is_array_v = is_array<T>::value;

template<class T>
class is_const : public false_type {};

template<class T>
class is_const<const T> : public true_type {};

template<class T>
_STD_API static bool is_const_v = is_const<T>::value;

template<class T>
class is_volatile : public false_type {};

template<class T>
class is_volatile<volatile T> : public true_type {};

template<class T>
_STD_API static bool is_volatile_v = is_volatile<T>::value;

template<class T>
_STD_API static bool is_const_volatile_v = is_const_v<T> && is_volatile_v<T>;

template<class T>
class is_reference : public false_type {};

template<class T>
class is_reference<T&> : public true_type {};

template<class T>
_STD_API static bool is_reference_v = is_reference<T>::value;

template<class T>
class is_pointer : public false_type {};

template<class T>
class is_pointer<T*> : public true_type {};

template<class T>
_STD_API static bool is_pointer_v = is_pointer<T>::value;

template <class T>
_STD_API static bool is_class_v = __is_class(T);

_STD_API_END

#define _STD_TYPE_TRAITS
#endif