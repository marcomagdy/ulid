#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "ulid.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <random>
#include <string>
#include <thread>

#include <uuid/uuid.h>

TEST_CASE("ulid_generate() generates a valid ULID")
{
    char ulid_str[27];
    ulid_generate_string(ulid_str);
    REQUIRE(strlen(ulid_str) == 26);
}

TEST_CASE("Invalid ULID does not parse")
{
    std::string invalid_ulid = "01B3EAF48P97LF8X1ANX1BMA6X"; // contains invalid character 'L'
    ulid_t ulid;
    REQUIRE(ulid_from_string(invalid_ulid.c_str(), &ulid) == 0);
}

TEST_CASE("ULID with lowercase characters parses successfully")
{
    std::string lowercase_ulid = "01b3eaf48p97mf8x1anx1bma6x";
    ulid_t ulid;
    REQUIRE(ulid_from_string(lowercase_ulid.c_str(), &ulid) == 1);
}

TEST_CASE("ULID with mixed-case characters parses successfully")
{
    std::string lowercase_ulid = "01b3eaF48P97MF8X1anx1bma6x";
    ulid_t ulid;
    REQUIRE(ulid_from_string(lowercase_ulid.c_str(), &ulid) == 1);
}

TEST_CASE("ULID with invalid length does not parse")
{
    std::string invalid_ulid = "01b3eaf48p97mf";
    ulid_t ulid;
    REQUIRE(ulid_from_string(invalid_ulid.c_str(), &ulid) == 0);
}

TEST_CASE("ULID with extra characters does not parse")
{
    std::string invalid_ulid = "01B3EAF48P97KF8X1ANX1BMA6XN0PE";
    ulid_t ulid;
    REQUIRE(ulid_from_string(invalid_ulid.c_str(), &ulid) == 0);
}

// ULID round-trip test (encode -> decode -> encode)
TEST_CASE("ULID round-trip")
{
    char ulid_str[27];
    ulid_generate_string(ulid_str);

    ulid_t ulid;
    REQUIRE(ulid_from_string(ulid_str, &ulid) == 1);

    char recovered_str[27];
    ulid_to_string(&ulid, recovered_str);
    REQUIRE(strcmp(ulid_str, recovered_str) == 0);
}

TEST_CASE("ULID generate into a pre-allocated buffer is null-terminated")
{
    char output[27];
    ulid_generate_string(output);
    REQUIRE(output[26] == '\0');

    ulid_t ulid;
    REQUIRE(ulid_from_string(output, &ulid) == 1);
}

TEST_CASE("ULID decodes to correct byte values")
{
    std::string ulid_str = "01GFBZE3YBBJX1DVTM13EXZ2X6";
    ulid_t ulid;
    REQUIRE(ulid_from_string(ulid_str.c_str(), &ulid) == 1);

    // Equivalent UUID of this ULID is 0183D7F7-0FCB-5CBA-16EF-5408DDDF8BA6
    // check each byte individually
    REQUIRE(ulid.bits[0] == 0x01);
    REQUIRE(ulid.bits[1] == 0x83);
    REQUIRE(ulid.bits[2] == 0xD7);
    REQUIRE(ulid.bits[3] == 0xF7);
    REQUIRE(ulid.bits[4] == 0x0F);
    REQUIRE(ulid.bits[5] == 0xCB);
    REQUIRE(ulid.bits[6] == 0x5C);
    REQUIRE(ulid.bits[7] == 0xBA);
    REQUIRE(ulid.bits[8] == 0x16);
    REQUIRE(ulid.bits[9] == 0xEF);
    REQUIRE(ulid.bits[10] == 0x54);
    REQUIRE(ulid.bits[11] == 0x08);
    REQUIRE(ulid.bits[12] == 0xDD);
    REQUIRE(ulid.bits[13] == 0xDF);
    REQUIRE(ulid.bits[14] == 0x8B);
    REQUIRE(ulid.bits[15] == 0xA6);
}

TEST_CASE("ULID generate using entropy from another source")
{
    // Generate 10 random bytes from std::random_device
    std::random_device random_device;
    unsigned char entropy[10];
    std::generate_n(entropy, 10, std::ref(random_device));

    ulid_t ulid = ulid_generate_with_entropy(entropy);

    char ulid_string[27];
    ulid_to_string(&ulid, ulid_string);

    // parse the generated ULID back into a ulid_t
    ulid_t parsed_ulid;
    REQUIRE(ulid_from_string(ulid_string, &parsed_ulid) == 1);

    auto* bits = parsed_ulid.bits + 6; // skip the first 6 bytes (timestamp)

    // check that the last 10 bytes of the ULID match the entropy we provided
    REQUIRE(std::equal(bits, bits + 10, entropy));
}

TEST_CASE("ULID random number generation is thread-safe")
{
    std::vector<std::string> thread_one_ulids;
    std::thread thread_one([&]() {
        for (int i = 0; i < 1000; i++) {
            char ulid_str[27];
            ulid_generate_string(ulid_str);
            thread_one_ulids.emplace_back(ulid_str);
        }
    });

    std::vector<std::string> thread_two_ulids;
    std::thread thread_two([&]() {
        for (int i = 0; i < 1000; i++) {
            char ulid_str[27];
            ulid_generate_string(ulid_str);
            thread_two_ulids.emplace_back(ulid_str);
        }
    });

    thread_one.join();
    thread_two.join();

    std::vector<std::string> all_ulids;
    all_ulids.insert(all_ulids.end(), thread_one_ulids.begin(), thread_one_ulids.end());
    all_ulids.insert(all_ulids.end(), thread_two_ulids.begin(), thread_two_ulids.end());

    std::sort(all_ulids.begin(), all_ulids.end());
    REQUIRE(std::adjacent_find(all_ulids.begin(), all_ulids.end()) == all_ulids.end());
}

namespace {
std::string generate_uuid()
{
    uuid_t uuid;
    uuid_generate_random(uuid);
    char uuid_str[37];
    uuid_unparse(uuid, uuid_str);
    return uuid_str;
}
} // namespace
TEST_CASE("ULID generation is 40% faster than UUID generation")
{
    // Generate 100,000 ULIDs and 100,000 UUIDs and compare the time taken
    constexpr int num_iterations = 100000;
    std::vector<std::string> store;
    store.reserve(num_iterations);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_iterations; i++) {
        char ulid_str[27];
        ulid_generate_string(ulid_str);
        store.push_back(ulid_str);
    }
    auto end = std::chrono::high_resolution_clock::now();
    const auto ulid_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // UUIDs
    store.clear();
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_iterations; i++) {
        store.push_back(generate_uuid());
    }
    end = std::chrono::high_resolution_clock::now();
    const auto uuid_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    REQUIRE(ulid_time.count() < uuid_time.count());
    const auto diff = static_cast<double>(uuid_time.count() - ulid_time.count());
    const auto percentage = (diff / static_cast<double>(uuid_time.count())) * 100;
    REQUIRE(percentage >= 40);
}