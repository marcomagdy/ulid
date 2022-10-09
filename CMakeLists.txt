cmake_minimum_required(VERSION 3.10)
project(libulid CXX)

option(ENABLE_SANITIZERS "Enables ASAN and UBSAN" ON)

add_library(${PROJECT_NAME} ulid.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    -fno-exceptions
    -fno-rtti
    -Wall
    -Wextra
    -Werror
    )

add_executable(${PROJECT_NAME}-test test.cpp)
target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME})

if (ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME} PUBLIC "-fsanitize=address,undefined")

    target_compile_options(${PROJECT_NAME}-test PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME}-test PUBLIC "-fsanitize=address,undefined")
endif()

# TODO: Add proper testing (make test)
# TODO: Add proper CMake installation to install under <ulid/ulid.h>
