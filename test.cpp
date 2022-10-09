#include "ulid.h"

#include <string>

int main()
{
    auto count = 100;
    while (count--) {
        auto binary = ulid::generate();
        const std::string id = binary.str();
        printf("%s\n", id.c_str());
        auto parsed = ulid::parse(id);
        if (!parsed) {
            puts("Failed to parse");
            return 1;
        }
        if (memcmp(parsed->bits, binary.bits, 16) != 0) {
            printf("%s\n", parsed->str().c_str());
            puts("Failed to parse - binary data mismatch");
            return 1;
        }

        if (parsed->str() != id) {
            puts("Failed to roundtrip");
            return 1;
        }

        // printf("%s\n", id.c_str());
    }

    return 0;
}
