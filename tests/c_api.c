#include "ulid.h"

#include <stdio.h>
#include <string.h>

static int failures;

#define EXPECT(cond)                                                                                                   \
    do {                                                                                                               \
        if (!(cond)) {                                                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);                                            \
            failures++;                                                                                                \
        }                                                                                                              \
    } while (0)

static void test_generate_str(void)
{
    char dest[ULID_STRING_SIZE];
    memset(dest, 0xFF, sizeof(dest));

    EXPECT(ulid_generate_str(dest) == ULID_OK);
    EXPECT(dest[ULID_STRING_LENGTH] == '\0');
    EXPECT(strlen(dest) == ULID_STRING_LENGTH);

    ulid_t id;
    EXPECT(ulid_from_str(dest, &id) == ULID_OK);
}

static void test_generate_binary(void)
{
    ulid_t id;
    memset(&id, 0, sizeof(id));

    EXPECT(ulid_generate(&id) == ULID_OK);

    char dest[ULID_STRING_SIZE];
    EXPECT(ulid_to_str(&id, dest) == ULID_OK);
    EXPECT(dest[ULID_STRING_LENGTH] == '\0');
    EXPECT(strlen(dest) == ULID_STRING_LENGTH);
}

static void test_generate_with_entropy(void)
{
    uint8_t entropy[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ulid_t id;

    EXPECT(ulid_generate_with_entropy(&id, entropy) == ULID_OK);
    EXPECT(memcmp(id.bits + 6, entropy, sizeof(entropy)) == 0);
}

static void test_round_trip(void)
{
    char generated[ULID_STRING_SIZE];
    EXPECT(ulid_generate_str(generated) == ULID_OK);

    ulid_t id;
    EXPECT(ulid_from_str(generated, &id) == ULID_OK);

    char encoded[ULID_STRING_SIZE];
    EXPECT(ulid_to_str(&id, encoded) == ULID_OK);
    EXPECT(strcmp(generated, encoded) == 0);
}

static void test_known_decode(void)
{
    /* Equivalent UUID: 0183D7F7-0FCB-5CBA-16EF-5408DDDF8BA6 */
    static const char* ulid_str = "01GFBZE3YBBJX1DVTM13EXZ2X6";
    static const uint8_t expected[ULID_SIZE] = {
        0x01, 0x83, 0xD7, 0xF7, 0x0F, 0xCB, 0x5C, 0xBA, 0x16, 0xEF, 0x54, 0x08, 0xDD, 0xDF, 0x8B, 0xA6};

    ulid_t id;
    EXPECT(ulid_from_str(ulid_str, &id) == ULID_OK);
    EXPECT(memcmp(id.bits, expected, ULID_SIZE) == 0);
}

static void test_case_insensitive_parse(void)
{
    ulid_t id;
    EXPECT(ulid_from_str("01b3eaf48p97mf8x1anx1bma6x", &id) == ULID_OK);
    EXPECT(ulid_from_str("01b3eaF48P97MF8X1anx1bma6x", &id) == ULID_OK);
}

static void test_invalid_input(void)
{
    ulid_t id;
    EXPECT(ulid_from_str("01B3EAF48P97LF8X1ANX1BMA6X", &id) == ULID_ERROR); /* 'L' is not Crockford */
    EXPECT(ulid_from_str("01b3eaf48p97mf", &id) == ULID_ERROR);
    EXPECT(ulid_from_str("01B3EAF48P97KF8X1ANX1BMA6XN0PE", &id) == ULID_ERROR);
    EXPECT(ulid_from_str("", &id) == ULID_ERROR);
}

static void test_from_str_n(void)
{
    static const char* known = "01GFBZE3YBBJX1DVTM13EXZ2X6";
    char raw[ULID_STRING_LENGTH];
    memcpy(raw, known, ULID_STRING_LENGTH);

    ulid_t id;
    EXPECT(ulid_from_str_n(raw, ULID_STRING_LENGTH, &id) == ULID_OK);
    EXPECT(ulid_from_str_n(raw, ULID_STRING_LENGTH - 1, &id) == ULID_ERROR);
    EXPECT(ulid_from_str_n(known, ULID_STRING_LENGTH + 1, &id) == ULID_ERROR);
}

static void test_null_arguments(void)
{
    ulid_t id;
    char dest[ULID_STRING_SIZE];
    uint8_t entropy[10] = {0};

    EXPECT(ulid_generate(NULL) == ULID_ERROR);
    EXPECT(ulid_generate_str(NULL) == ULID_ERROR);
    EXPECT(ulid_generate_with_entropy(NULL, entropy) == ULID_ERROR);
    EXPECT(ulid_generate_with_entropy(&id, NULL) == ULID_ERROR);
    EXPECT(ulid_to_str(NULL, dest) == ULID_ERROR);
    EXPECT(ulid_to_str(&id, NULL) == ULID_ERROR);
    EXPECT(ulid_from_str(NULL, &id) == ULID_ERROR);
    EXPECT(ulid_from_str("01GFBZE3YBBJX1DVTM13EXZ2X6", NULL) == ULID_ERROR);
    EXPECT(ulid_from_str_n(NULL, ULID_STRING_LENGTH, &id) == ULID_ERROR);
    EXPECT(ulid_from_str_n("01GFBZE3YBBJX1DVTM13EXZ2X6", ULID_STRING_LENGTH, NULL) == ULID_ERROR);
}

int main(void)
{
    test_generate_str();
    test_generate_binary();
    test_generate_with_entropy();
    test_round_trip();
    test_known_decode();
    test_case_insensitive_parse();
    test_invalid_input();
    test_from_str_n();
    test_null_arguments();

    if (failures != 0) {
        fprintf(stderr, "%d assertion(s) failed\n", failures);
        return 1;
    }
    return 0;
}
