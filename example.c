/*
 * Example of using the ULID C API from a pure C program
 */
#include <stdio.h>
#include <string.h>
#include "source/ulid.h"

int main()
{
    printf("ULID C API Example\n");
    printf("================\n\n");

    // Generate a ULID
    printf("1. Generating a ULID:\n");
    ulid_t ulid = ulid_generate();

    // Convert to string
    char ulid_str[27];
    ulid_to_string(&ulid, ulid_str);
    printf("   Generated ULID: %s\n\n", ulid_str);

    // Generate a ULID string directly
    printf("2. Generating a ULID string directly:\n");
    char direct_ulid_str[27];
    ulid_generate_string(direct_ulid_str);
    printf("   Generated ULID: %s\n\n", direct_ulid_str);

    // Parse a ULID from string
    printf("3. Parsing a ULID from string:\n");
    ulid_t parsed_ulid;
    if (ulid_from_string(ulid_str, &parsed_ulid) == 1) {
        printf("   Successfully parsed ULID\n");

        // Convert back to string to verify
        char verified_str[27];
        ulid_to_string(&parsed_ulid, verified_str);
        printf("   Verified string: %s\n", verified_str);
        printf("   Strings match: %s\n\n", strcmp(ulid_str, verified_str) == 0 ? "Yes" : "No");
    }
    else {
        printf("   Failed to parse ULID\n\n");
    }

    // Test with custom entropy
    printf("4. Generating ULID with custom entropy:\n");
    unsigned char entropy[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
    ulid_t custom_ulid = ulid_generate_with_entropy(entropy);

    char custom_str[27];
    ulid_to_string(&custom_ulid, custom_str);
    printf("   ULID with custom entropy: %s\n", custom_str);

    // Verify entropy is in the last 10 bytes
    printf("   Last 10 bytes match entropy: ");
    int match = 1;
    for (int i = 0; i < 10; i++) {
        if (custom_ulid.bits[6 + i] != entropy[i]) {
            match = 0;
            break;
        }
    }
    printf("%s\n\n", match ? "Yes" : "No");

    // Test invalid ULID parsing
    printf("5. Testing invalid ULID parsing:\n");
    const char* invalid_ulid = "INVALID_ULID_STRING";
    ulid_t invalid_parsed;
    if (ulid_from_string(invalid_ulid, &invalid_parsed) == 1) {
        printf("   Unexpected: Invalid ULID was parsed successfully\n");
    }
    else {
        printf("   Correctly rejected invalid ULID string\n");
    }

    return 0;
}
