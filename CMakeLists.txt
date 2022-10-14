cmake_minimum_required(VERSION 3.10)
project(ulid VERSION 0.1.0 LANGUAGES CXX)

option(ENABLE_SANITIZERS "Enables ASAN and UBSAN" ON)

add_subdirectory(source)

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

if (ENABLE_SANITIZERS)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address,undefined)
    target_link_options(${PROJECT_NAME} PUBLIC "-fsanitize=address,undefined")
endif()

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} EXPORT ulid DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT ulid NAMESPACE ULID:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake)
install(FILES ulid.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_DOCDIR})

