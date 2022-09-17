cmake_minimum_required(VERSION 3.10)
project(ulid CXX)

add_library(${PROJECT_NAME} ulid.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall
    -Wextra
    -fsanitize=address,undefined
    -Werror
    )

target_link_options(${PROJECT_NAME} PUBLIC "-fsanitize=address,undefined")

add_executable(${PROJECT_NAME}-test main.cpp ulid.cpp)
target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_17)
