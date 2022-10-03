/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include "ulid.h"

#include <chrono>
#include <string>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

namespace ulid {

static u64 seed()
{
    const auto now = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

// xorshift64*
static u64 g_state = seed();
static u64 rng()
{
    auto x = g_state;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    g_state = x;
    return x * 0x2545F4914F6CDD1DULL;
}

static void ulid_create(u8* ulid_buffer)
{
    const auto time_epoc = std::chrono::system_clock::now().time_since_epoch();
    const auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(time_epoc).count();

    ulid_buffer[0] = timestamp >> 40;
    ulid_buffer[1] = timestamp >> 32;
    ulid_buffer[2] = timestamp >> 24;
    ulid_buffer[3] = timestamp >> 16;
    ulid_buffer[4] = timestamp >> 8;
    ulid_buffer[5] = timestamp >> 0;

    u32 r = rng();
    ulid_buffer[6] = r >> 24;
    ulid_buffer[7] = r >> 16;
    ulid_buffer[8] = r >> 8;
    ulid_buffer[9] = r;
    r = rng();
    ulid_buffer[10] = r >> 24;
    ulid_buffer[11] = r >> 16;
    ulid_buffer[12] = r >> 8;
    ulid_buffer[13] = r >> 0;
    r = rng();
    ulid_buffer[14] = r >> 8;
    ulid_buffer[15] = r;
}

static void ulid_encode(const u8* ulid_data, u8* output)
{
    static constexpr char lookup[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

    output[0] = lookup[ulid_data[0] >> 5 & 7]; // bits[7,6,5]
    output[1] = lookup[ulid_data[0] & 31];     // bits[4,3,2,1,0]
    output += 2;

    int repeat = 3;
    while (repeat-- > 0) {
        output[0] = lookup[ulid_data[1] >> 3 & 31];                       // bits[7,6,5,4,3]
        output[1] = lookup[(ulid_data[1] & 7) << 2 | ulid_data[2] >> 6];  // bits[2,1,0] | [7, 6]
        output[2] = lookup[ulid_data[2] >> 1 & 31];                       // bits[5,4,3,2,1]
        output[3] = lookup[(ulid_data[2] & 1) << 4 | ulid_data[3] >> 4];  // bits[0] | [7,6,5,4]
        output[4] = lookup[(ulid_data[3] & 15) << 1 | ulid_data[4] >> 7]; // bits[3,2,1,0] | [7]
        output[5] = lookup[ulid_data[4] >> 2 & 31];                       // bits[6,5,4,3,2]
        output[6] = lookup[(ulid_data[4] & 3) << 3 | ulid_data[5] >> 5];  // bits[1,0] | [7,6,5]
        output[7] = lookup[ulid_data[5] & 31];                            // bits[4,3,2,1,0]
        output += 8;
        ulid_data += 5;
    }
}

std::string ulid_t::str() const
{
    u8 output[26];
    ulid_encode(bits, output);
    return { reinterpret_cast<const char*>(output), 26 };
}

ulid_t ulid()
{
    ulid_t out;
    ulid_create(out.bits);
    return out;
}

std::optional<ulid_t> ulid(std::string_view sv)
{
    if (sv.length() != 26) {
        return std::nullopt;
    }
    return ulid();
}
}
