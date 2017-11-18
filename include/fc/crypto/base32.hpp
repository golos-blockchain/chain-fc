#pragma once

#include <fc/vector.hpp>
#include <fc/string.hpp>

namespace fc {
    std::vector<char> from_base32(const std::string &b32);

    std::string to_base32(const std::vector<char> &vec);

    std::string to_base32(const char *data, size_t len);
}
