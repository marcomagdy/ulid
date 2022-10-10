cmake_minimum_required(VERSION 3.10)
project(ulid CXX)

option(ENABLE_SANITIZERS "Enables ASAN and UBSAN" ON)
option(ENABLE_TESTS "Enables unit tests and UBSAN" OFF)

add_library(${PROJECT_NAME} ulid.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE
    -fno-exceptions
    -fno-rtti
    -Wall
    -Wextra
    -Werror
    )


if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

if (ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME} PUBLIC "-fsanitize=address,undefined")

    target_compile_options(${PROJECT_NAME}-test PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME}-test PUBLIC "-fsanitize=address,undefined")
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES ulid.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

