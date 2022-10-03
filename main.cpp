#include "ulid.h"

#include <string>

int main()
{
    using ulid::ulid;
    auto count = 100;
    while (count --> 0) {
        std::string id = ulid().str();
        printf("%s\n", id.c_str());
    }
    return 0;
}
