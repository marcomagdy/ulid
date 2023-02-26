#include "ulid.h"

#include <algorithm>
#include <random>
#include <string>
#include <thread>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

TEST_CASE("ulid::generate() generates a valid ULID")
{
    const auto ulid = ulid::generate();
    REQUIRE(ulid.str().size() == 26);
}

TEST_CASE("Invalid ULID does not parse")
{
    std::string invalid_ulid = "01B3EAF48P97LF8X1ANX1BMA6X"; // contains invalid character 'L'
    REQUIRE(ulid::from_str(invalid_ulid) == std::nullopt);
}

TEST_CASE("ULID with lowercase characters parses successfully")
{
    std::string lowercase_ulid = "01b3eaf48p97mf8x1anx1bma6x";
    REQUIRE(ulid::from_str(lowercase_ulid) != std::nullopt);
}

TEST_CASE("ULID with mixed-case characters parses successfully")
{
    std::string lowercase_ulid = "01b3eaF48P97MF8X1anx1bma6x";
    REQUIRE(ulid::from_str(lowercase_ulid) != std::nullopt);
}

TEST_CASE("ULID with invalid length does not parse")
{
    std::string invalid_ulid = "01b3eaf48p97mf";
    REQUIRE(ulid::from_str(invalid_ulid) == std::nullopt);
}

TEST_CASE("ULID with extra characters does not parse")
{
    std::string invalid_ulid = "01B3EAF48P97KF8X1ANX1BMA6XN0PE";
    REQUIRE(ulid::from_str(invalid_ulid) == std::nullopt);
}

// ULID round-trip test (encode -> decode -> encode)
TEST_CASE("ULID round-trip")
{
    const std::string ulid_str = ulid::generate().str();
    const auto ulid = ulid::from_str(ulid_str);
    REQUIRE(ulid != std::nullopt);
    REQUIRE(ulid->str() == ulid_str);
}

TEST_CASE("ULID generate into a pre-allocated buffer is null-terminated")
{
    char output[27];
    ulid::generate(output);
    REQUIRE(output[26] == '\0');
    REQUIRE(ulid::from_str(output) != std::nullopt);
}

TEST_CASE("ULID decodes to correct byte values")
{
    std::string ulid_str = "01GFBZE3YBBJX1DVTM13EXZ2X6";
    const auto ulid = ulid::from_str(ulid_str).value();
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
    ulid::u8 entropy[10];
    std::generate_n(entropy, 10, std::ref(random_device));

    const auto ulid_string = ulid::generate(entropy).str();
    // parse the generated ULID back into a ulid::ulid_t
    std::optional<ulid::ulid_t> ulid = ulid::from_str(ulid_string);
    REQUIRE(ulid != std::nullopt);
    auto* bits = ulid->bits + 6; // skip the first 6 bytes (timestamp)

    // check that the last 10 bytes of the ULID match the entropy we provided
    REQUIRE(std::equal(bits, bits + 10, entropy));
}

TEST_CASE("ULID random number generation is thread-safe")
{
    std::vector<std::string> thread_one_ulids;
    std::thread thread_one([&]() {
        for (int i = 0; i < 1000; i++) {
            thread_one_ulids.emplace_back(ulid::generate().str());
        }
    });

    std::vector<std::string> thread_two_ulids;
    std::thread thread_two([&]() {
        for (int i = 0; i < 1000; i++) {
            thread_two_ulids.emplace_back(ulid::generate().str());
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
