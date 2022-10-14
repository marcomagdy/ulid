find_package(Catch2 REQUIRED)
add_executable(${PROJECT_NAME}-test main.cpp)
target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME}-test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../source)
target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}-test PRIVATE Catch2::Catch2WithMain)

if (ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME}-test PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME}-test PUBLIC "-fsanitize=address,undefined")
endif()

include(Catch)
catch_discover_tests(${PROJECT_NAME}-test)
