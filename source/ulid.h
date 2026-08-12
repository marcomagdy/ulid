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

#ifndef ULID_H
#define ULID_H

#ifdef __cplusplus
#    include <cstddef>
#    include <optional>
#    include <string>
#    include <string_view>
#else
#    include <stddef.h>
#    include <stdint.h>
#endif

#define ULID_SIZE 16
#define ULID_STRING_LENGTH 26
#define ULID_STRING_SIZE (ULID_STRING_LENGTH + 1)

enum { ULID_OK = 0, ULID_ERROR = -1 };

#ifdef __cplusplus

namespace ulid {
using u8 = unsigned char;
struct ulid_t {
    u8 bits[16];

    /**
     * Serialize the ULID bits into a string.
     */
    std::string str() const;
};

/**
 * Generate a new ULID.
 */
ulid_t generate();

/**
 * Generate a new ULID using the given entropy source.
 */
ulid_t generate(const u8 (&entropy)[10]);

/**
 * Generate a new ULID string (26 characters) in the provided buffer and null terminate it.
 */
void generate(char (&output)[27]);

/**
 * Decode a ULID string into its binary format.
 */
std::optional<ulid_t> from_str(std::string_view ulid_str);

} // namespace ulid

extern "C" {

/**
 * Generate a new ULID. Returns ULID_OK or ULID_ERROR.
 */
int ulid_generate(ulid::ulid_t* out);

/**
 * Generate a new ULID using the given 10-byte entropy source.
 */
int ulid_generate_with_entropy(ulid::ulid_t* out, const unsigned char entropy[10]);

/**
 * Generate a new ULID string into dest (26 characters + NUL). dest must have room for 27 bytes.
 */
int ulid_generate_str(char dest[ULID_STRING_SIZE]);

/**
 * Encode a ULID as a 26-character NUL-terminated string. dest must have room for 27 bytes.
 */
int ulid_to_str(const ulid::ulid_t* id, char dest[ULID_STRING_SIZE]);

/**
 * Decode a NUL-terminated ULID string into its binary format.
 */
int ulid_from_str(const char* str, ulid::ulid_t* out);

/**
 * Decode a ULID string of explicit length into its binary format.
 */
int ulid_from_str_n(const char* str, size_t len, ulid::ulid_t* out);
}

#else /* C */

typedef struct ulid_t {
    uint8_t bits[ULID_SIZE];
} ulid_t;

/**
 * Generate a new ULID. Returns ULID_OK or ULID_ERROR.
 */
int ulid_generate(ulid_t* out);

/**
 * Generate a new ULID using the given 10-byte entropy source.
 */
int ulid_generate_with_entropy(ulid_t* out, const uint8_t entropy[10]);

/**
 * Generate a new ULID string into dest (26 characters + NUL). dest must have room for 27 bytes.
 */
int ulid_generate_str(char dest[ULID_STRING_SIZE]);

/**
 * Encode a ULID as a 26-character NUL-terminated string. dest must have room for 27 bytes.
 */
int ulid_to_str(const ulid_t* id, char dest[ULID_STRING_SIZE]);

/**
 * Decode a NUL-terminated ULID string into its binary format.
 */
int ulid_from_str(const char* str, ulid_t* out);

/**
 * Decode a ULID string of explicit length into its binary format.
 */
int ulid_from_str_n(const char* str, size_t len, ulid_t* out);

#endif /* __cplusplus */

#endif /* ULID_H */
