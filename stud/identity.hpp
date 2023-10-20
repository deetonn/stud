
#ifndef _STD_IDENTITY

#include <typeinfo>
#include <string>

#include "forward.hpp"

_STD_API_BEGIN

using WideInt = struct __Wide_int {
    size_t address, identifiers;
};

/// <summary>
/// provides a strong way of identifying objects. this is deep and tracks
/// just one single instance. this includes its address and characteristics.
/// </summary>
/// <typeparam name="B">The type of the identified object.</typeparam>
template<typename B>
class Identity {
private:
    // left will be the address, right is characteristics.
    WideInt m_data;
    const B* m_inst{ nullptr };
public:
    _STD_API Identity() noexcept = delete;
    _STD_API Identity(const B& instance) noexcept {
        m_data.address = reinterpret_cast<size_t>(&instance);
        
        auto* const name = typeid(B).name();
        const auto length = std::char_traits<char>::length(name);
        char* right = reinterpret_cast<char*>(&m_data.identifiers);

        for (int i = 0; i < ((length > 8) ? 7 : length); ++i) {
            right[i] = name[i];
        }

        m_inst = &instance;
    }

    _NODISCARD _STD_API const WideInt& data() const noexcept {
        return m_data;
    }

    _NODISCARD _STD_API bool is_equal_to(const Identity& other) const noexcept {
        return this->address() == other.address()
            && this->identifiers() == other.identifiers();
    }

    _STD_API void moved(const B& instance) noexcept {
        const Identity new_identity = Identity(instance);

        if (new_identity.identifiers() == this->identifiers())
        {
            m_data.address = new_identity.address();
        }

        m_inst = instance;
    }

    _NODISCARD _STD_API size_t address() const noexcept {
        return m_data.address;
    }

    _NODISCARD _STD_API size_t identifiers() const noexcept {
        return m_data.identifiers;
    }

    _NODISCARD _STD_API const B& instance() const noexcept {
        return m_inst;
    }
};

_STD_API_END

#define _STD_IDENTITY
#endif