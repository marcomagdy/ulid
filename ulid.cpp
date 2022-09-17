#include "ulid.h"

#include <chrono>

using u8 = uint8_t;
using u32 = uint32_t;
using u64 = uint64_t;

namespace ulid {

static u64 time_seed()
{
    const auto now = std::chrono::steady_clock::now();
    const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return ms.count();
}

// xorshift64*
static u64 rng()
{
    static u64 x = time_seed();
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
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

std::string ulid_string()
{
    u8 binary[16];
    ulid_create(binary);
    u8 output[26];
    ulid_encode(binary, output);
    return { reinterpret_cast<const char*>(output), 26 };
}

ulid_t ulid()
{
    ulid_t out;
    ulid_create(out.bits);
    return out;
}
}
