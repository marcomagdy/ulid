# libulid

`ulid` implementation in C++ with the following feature set that makes it suitable for usage in embedded environments:
1. No dynamic allocations
1. Thread-safe
1. No usage of C++ Exceptions


## Installation

Either add the files `ulid.cpp` and `ulid.h` to your project, or clone the project, build it and install it.

## Usage


No any dynamic allocation:
```cpp
    char ulid_str[27];
    ulid::generate(ulid_str);
    printf("%s\n", ulid_str); // ulid_str is null-terminated
```

For convenience in less-restricted environments:
`std::string unique = ulid::generate().str();`

