
#include "forward.hpp"

#include <string>
#include "result.hpp"
#include "panic.hpp"

_STD_API_BEGIN

enum class EnvironmentError {
    VariableDoesNotExist,
};

/// STATIC class
class Environment {
public:
    using EnvData = std::vector<std::pair<std::string, std::string>>;

    static Result<std::string, EnvironmentError> get(const std::string& varName) noexcept {
        auto* _That_item = getenv(varName.c_str());
        if (!_That_item)
            return EnvironmentError::VariableDoesNotExist;
        return std::string(_That_item, strlen(_That_item));
    }
    static bool set(const std::string& key, const std::string& value) noexcept {
        if constexpr (is_windows())
        {
            return SetEnvironmentVariableA(key.c_str(), value.c_str());
        }
        panic(ALWAYS(), "This platform is not yet supported for SETTING environment variables.");
    }

    static const EnvData& data() noexcept {
        static EnvData _Static_data = {};

        if (_Static_data.size() == 0) {
            auto** env_ptr = environ;
            auto original_ptr = env_ptr;
            std::size_t length = 0;
            while (*(env_ptr++) != NULL)
                ++length;

            // Populate the vector.
            for (std::size_t i = 0; i < length; ++i)
            {
                auto* this_item = original_ptr[i];
                auto environment_str = std::string(this_item);
                size_t pos_of_equals = environment_str.find('=');

                if (pos_of_equals == std::string::npos)
                    // Invalid environment variable.
                    continue;

                auto key = environment_str.substr(0, pos_of_equals);
                auto value = environment_str.substr(pos_of_equals + 1);

                _Static_data.push_back(std::make_pair(key, value));
            }
        }

        return _Static_data;
    }
};

_STD_API_END