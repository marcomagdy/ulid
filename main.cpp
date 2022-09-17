#include "ulid.h"

int main()
{
    auto count = 100;
    while (count --> 0) {
        printf("%s\n", ulid::ulid_string().c_str());
    }
    auto id = ulid::ulid();
    printf("This one from binary: %s\n", id.str().c_str());
    return 0;
}
