#pragma once

#include <string>

namespace ulid {
struct ulid_t {
    using u8 = unsigned char;
    union {
        u8 bits[16];
        struct {
            u8 time[6];
            u8 random[10];
        };
    };
    std::string str() const;
};

std::string ulid_string();
ulid_t ulid();
}
