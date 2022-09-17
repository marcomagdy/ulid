#include "ulid.h"

int main()
{
    auto count = 100;
    while (count --> 0) {
        printf("%s\n", ulid::create().c_str());
    }
    return 0;
}
