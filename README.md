# libulid

![Linux build badge](https://github.com/marcomagdy/ulid/actions/workflows/cmake.yml/badge.svg)
![CodeQL badge](https://github.com/marcomagdy/ulid/actions/workflows/codeql.yml/badge.svg)

Lightweight [`ulid`](https://github.com/ulid/spec) implementation in C++ suitable for embedded environments, with a C99 API:

1. No dynamic allocations
1. No C++ Exceptions
1. Thread-safe

## Installation

- Copy  `ulid.cpp` and `ulid.h` files to your project. That's it.

- You can also build it from source and install it via CMake.

```shell
$ cmake -B build -DCMAKE_BUILD_TYPE=Release
$ cmake --build build
$ cmake --install build
```



## API

No dynamic allocation:
```cpp
  char ulid_str[27]; // 26 characters + null terminator
  ulid::generate(ulid_str);
  printf("%s\n", ulid_str); // ulid_str is null-terminated
```

Simple API:
```cpp
  std::string unique = ulid::generate().str();
```

Decode a ULID string back to bytes:
```cpp
  const char some_ulid[] = "01GF428XRREWQWJHXRRGNN99NV";
  ulid::ulid_t bits = ulid::from_str(some_ulid);
```

C99 (compile `ulid.cpp` as C++ and link it with your C code):
```c
  char ulid_str[ULID_STRING_SIZE];
  ulid_generate_str(ulid_str);

  ulid_t id;
  if (ulid_from_str("01GF428XRREWQWJHXRRGNN99NV", &id) != ULID_OK) {
      /* invalid ULID */
  }
```
