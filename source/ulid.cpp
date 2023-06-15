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

#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <string_view>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

static u64 rng_seed()
{
    const auto now = std::chrono::steady_clock::now();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return millis.count();
}

// xorshift64*
static std::atomic<u64> state{rng_seed()};
static u64 rng()
{
    u64 expected = state.load(std::memory_order_relaxed);
    u64 number;
    do {
        number = expected;
        number ^= number >> 12;
        number ^= number << 25;
        number ^= number >> 27;
    } while (!state.compare_exchange_weak(expected, number, std::memory_order_relaxed));

    return number * 0x2545F4914F6CDD1DULL;
}

static void ulid_create(u8* ulid_buffer)
{
    using namespace std::chrono;
    const auto time_epoc = system_clock::now().time_since_epoch();
    const auto timestamp = duration_cast<std::chrono::milliseconds>(time_epoc).count();

    ulid_buffer[0] = timestamp >> 40;
    ulid_buffer[1] = timestamp >> 32;
    ulid_buffer[2] = timestamp >> 24;
    ulid_buffer[3] = timestamp >> 16;
    ulid_buffer[4] = timestamp >> 8;
    ulid_buffer[5] = timestamp >> 0;

    u32 num = rng();
    ulid_buffer[6] = num >> 24;
    ulid_buffer[7] = num >> 16;
    ulid_buffer[8] = num >> 8;
    ulid_buffer[9] = num;
    num = rng();
    ulid_buffer[10] = num >> 24;
    ulid_buffer[11] = num >> 16;
    ulid_buffer[12] = num >> 8;
    ulid_buffer[13] = num >> 0;
    num = rng();
    ulid_buffer[14] = num >> 8;
    ulid_buffer[15] = num;
}

static void ulid_create(u8* ulid_buffer, const u8 (&entropy)[10])
{
    using namespace std::chrono;
    const auto time_epoc = system_clock::now().time_since_epoch();
    const auto timestamp = duration_cast<std::chrono::milliseconds>(time_epoc).count();

    ulid_buffer[0] = timestamp >> 40;
    ulid_buffer[1] = timestamp >> 32;
    ulid_buffer[2] = timestamp >> 24;
    ulid_buffer[3] = timestamp >> 16;
    ulid_buffer[4] = timestamp >> 8;
    ulid_buffer[5] = timestamp >> 0;

    ulid_buffer[6] = entropy[0];
    ulid_buffer[7] = entropy[1];
    ulid_buffer[8] = entropy[2];
    ulid_buffer[9] = entropy[3];
    ulid_buffer[10] = entropy[4];
    ulid_buffer[11] = entropy[5];
    ulid_buffer[12] = entropy[6];
    ulid_buffer[13] = entropy[7];
    ulid_buffer[14] = entropy[8];
    ulid_buffer[15] = entropy[9];
}

static void ulid_encode(const u8* __restrict ulid_data, u8* output)
{
    constexpr char lookup[] = "0123456789ABCDEFGHJKMNPQRSTVWXYZ";

    output[0] = lookup[ulid_data[0] >> 5 & 7]; // bits[7,6,5]
    output[1] = lookup[ulid_data[0] & 31];     // bits[4,3,2,1,0]
    output += 2;
    ulid_data += 1;

    int repeat = 3;
    while (repeat--) {
        output[0] = lookup[ulid_data[0] >> 3 & 31];                       // bits[7,6,5,4,3]
        output[1] = lookup[(ulid_data[0] & 7) << 2 | ulid_data[1] >> 6];  // bits[2,1,0] | [7, 6]
        output[2] = lookup[ulid_data[1] >> 1 & 31];                       // bits[5,4,3,2,1]
        output[3] = lookup[(ulid_data[1] & 1) << 4 | ulid_data[2] >> 4];  // bits[0] | [7,6,5,4]
        output[4] = lookup[(ulid_data[2] & 15) << 1 | ulid_data[3] >> 7]; // bits[3,2,1,0] | [7]
        output[5] = lookup[ulid_data[3] >> 2 & 31];                       // bits[6,5,4,3,2]
        output[6] = lookup[(ulid_data[3] & 3) << 3 | ulid_data[4] >> 5];  // bits[1,0] | [7,6,5]
        output[7] = lookup[ulid_data[4] & 31];                            // bits[4,3,2,1,0]
        output += 8;
        ulid_data += 5;
    }
}

static constexpr u8 decode_char(char character)
{
    // Table for decoding uppercase and lowercase base32 characters excluding I, L, O, U (i.e. Crockford's base32).
    // Other characters will decode to 0xff.

    // clang-format off
    constexpr u8 lookup[75] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
        0x11, 0xFF, 0x12, 0x13, 0xFF, 0x14, 0x15, 0xFF,
        0x16, 0x17, 0x18, 0x19, 0x1A, 0xFF, 0x1B, 0x1C,
        0x1D, 0x1E, 0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
        0x11, 0xFF, 0x12, 0x13, 0xFF, 0x14, 0x15, 0xFF,
        0x16, 0x17, 0x18, 0x19, 0x1A, 0xFF, 0x1B, 0x1C,
        0x1D, 0x1E, 0x1F,
    };
    // clang-format on
    return lookup[character - '0'];
}

static void ulid_decode(const char* __restrict ulid_data, u8* output)
{
    output[0] = decode_char(ulid_data[0]) << 5 | decode_char(ulid_data[1]);
    ulid_data += 2;
    output += 1;

    for (int i = 0; i < 3; ++i) {
        u8 decoded[8];
        for (int j = 0; j < 8; ++j) {
            decoded[j] = decode_char(ulid_data[j]);
        }
        output[0] = (decoded[0] << 3) | (decoded[1] >> 2);
        output[1] = (decoded[1] << 6) | (decoded[2] << 1) | (decoded[3] >> 4);
        output[2] = (decoded[3] << 4) | (decoded[4] >> 1);
        output[3] = (decoded[4] << 7) | (decoded[5] << 2) | (decoded[6] >> 3);
        output[4] = (decoded[6] << 5) | decoded[7];
        output += 5;
        ulid_data += 8;
    }
}

static bool validate(std::string_view data)
{
    if (data.size() != 26) {
        return false;
    }

    auto is_decodable = [](char letter) { return decode_char(letter) != 0xff; };

    return std::all_of(data.begin(), data.end(), is_decodable);
}

namespace ulid {

std::string ulid_t::str() const
{
    u8 output[26];
    ulid_encode(bits, output);
    return {reinterpret_cast<const char*>(output), 26};
}

ulid_t generate()
{
    ulid_t out;
    ulid_create(out.bits);
    return out;
}

ulid_t generate(const u8 (&entropy)[10])
{
    ulid_t out;
    ulid_create(out.bits, entropy);
    return out;
}

void generate(char (&output)[27])
{
    ulid_t out;
    ulid_create(out.bits);
    ulid_encode(out.bits, reinterpret_cast<u8*>(output));
    output[26] = '\0';
}

std::optional<ulid_t> from_str(std::string_view ulid_string)
{
    if (!validate(ulid_string)) {
        return std::nullopt;
    }

    ulid_t out;
    u8* out_ptr = out.bits;
    const auto* data = ulid_string.data();
    ulid_decode(data, out_ptr);
    return out;
}
} // namespace ulid
