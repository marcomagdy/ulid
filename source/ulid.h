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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned char bits[16];
} ulid_t;

/**
 * Generate a new ULID.
 */
ulid_t ulid_generate(void);

/**
 * Generate a new ULID using the given entropy source.
 * @param entropy Array of 10 bytes to use as entropy
 */
ulid_t ulid_generate_with_entropy(const unsigned char entropy[10]);

/**
 * Generate a new ULID string (26 characters) in the provided buffer and null terminate it.
 * @param output Buffer of at least 27 characters to store the ULID string
 */
void ulid_generate_string(char output[27]);

/**
 * Serialize the ULID bits into a string.
 * @param ulid The ULID to serialize
 * @param output Buffer of at least 27 characters to store the ULID string
 */
void ulid_to_string(const ulid_t* ulid, char output[27]);

/**
 * Decode a ULID string into its binary format.
 * @param ulid_str The ULID string to decode (must be exactly 26 characters)
 * @param ulid Output parameter to store the decoded ULID
 * @return 1 on success, 0 on failure (invalid string)
 */
int ulid_from_string(const char* ulid_str, ulid_t* ulid);

#ifdef __cplusplus
}
#endif
