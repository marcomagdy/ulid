add_library(${PROJECT_NAME} ulid.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE
    -fno-exceptions
    -fno-rtti
    -Wall
    -Wextra
    -Werror)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
