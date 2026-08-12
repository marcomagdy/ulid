add_executable(${PROJECT_NAME}-c-test c_api.c)
set_target_properties(${PROJECT_NAME}-c-test PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
    LINKER_LANGUAGE CXX)
target_include_directories(${PROJECT_NAME}-c-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../source)
target_link_libraries(${PROJECT_NAME}-c-test PRIVATE ${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}-c-test PRIVATE -Wall -Wextra -Werror -pedantic)

if (ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME}-c-test PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME}-c-test PUBLIC "-fsanitize=address,undefined")
endif()

add_test(NAME ${PROJECT_NAME}-c-api COMMAND ${PROJECT_NAME}-c-test)

find_package(doctest QUIET)
if (NOT doctest_FOUND)
    message(WARNING "'doctest' does not appear to be installed.\n"
                        "Use your package manager to install it.\n"
                        "On macOS, run: brew install doctest.")
    return()
endif()
add_executable(${PROJECT_NAME}-test main.cpp)
target_include_directories(${PROJECT_NAME}-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../source)
target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}-test PRIVATE doctest::doctest)

if (ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME}-test PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME}-test PUBLIC "-fsanitize=address,undefined")
endif()

include(doctest)
doctest_discover_tests(${PROJECT_NAME}-test)
