find_package(Catch2 REQUIRED)
add_executable(${PROJECT_NAME}-test main.cpp)
target_compile_features(${PROJECT_NAME}-test PRIVATE cxx_std_17)
target_link_libraries(${PROJECT_NAME}-test PRIVATE ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}-test PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME}-test)
