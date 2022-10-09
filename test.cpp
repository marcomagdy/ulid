#include "ulid.h"

#include <string>

int main()
{
    auto count = 100;
    while (count --> 0) {
        std::string id = ulid::generate().str();
        printf("%s\n", id.c_str());
    }

    puts("Testing generating into a preallocated buffer.");
    count = 10;
    while (count-- > 0) {
        char output[27];
        ulid::generate(output);
        printf("%s\n", output);

        // try to parse the output back
        std::optional<ulid::ulid_t> result = ulid::parse(output);
        assert(result.has_value());
        assert(result.value().str() == output);
    }


    return 0;
}
