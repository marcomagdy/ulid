#include "../ulid.h"

#include <string>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("ulid::generate() generates a valid ULID", "[ulid]")
{
    const auto ulid = ulid::generate();
    REQUIRE(ulid.str().size() == 26);
}

TEST_CASE("Invalid ULID does not parse", "[ulid]")
{
    std::string invalid_ulid = "01B3EAF48P97LF8X1ANX1BMA6X"; // contains invalid character 'L'
    REQUIRE(ulid::from_str(invalid_ulid) == std::nullopt);
}

TEST_CASE("ULID with lowercase characters parses successfully", "[ulid]")
{
    std::string lowercase_ulid = "01b3eaf48p97mf8x1anx1bma6x";
    REQUIRE(ulid::from_str(lowercase_ulid) != std::nullopt);
}

TEST_CASE("ULID with mixed-case characters parses successfully", "[ulid]")
{
    std::string lowercase_ulid = "01b3eaF48P97MF8X1anx1bma6x";
    REQUIRE(ulid::from_str(lowercase_ulid) != std::nullopt);
}

TEST_CASE("ULID with invalid length does not parse", "[ulid]")
{
    std::string invalid_ulid = "01b3eaf48p97mf";
    REQUIRE(ulid::from_str(invalid_ulid) == std::nullopt);
}

TEST_CASE("ULID with extra characters does not parse", "[ulid]")
{
    std::string invalid_ulid = "01B3EAF48P97KF8X1ANX1BMA6XN0PE";
    REQUIRE(ulid::from_str(invalid_ulid) == std::nullopt);
}

// ULID round-trip test (encode -> decode -> encode)
TEST_CASE("ULID round-trip", "[ulid]")
{
    const std::string ulid_str = ulid::generate().str();
    const auto ulid = ulid::from_str(ulid_str);
    REQUIRE(ulid != std::nullopt);
    REQUIRE(ulid->str() == ulid_str);
}

TEST_CASE("ULID generate into a pre-allocated buffer is null-terminated", "[ulid]")
{
    char output[27];
    ulid::generate(output);
    REQUIRE(output[26] == '\0');
    REQUIRE(ulid::from_str(output) != std::nullopt);
}

