cmake_minimum_required(VERSION 3.15)

project(enginemath VERSION 0.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(enginemath INTERFACE)
add_library(enginemath::enginemath ALIAS enginemath)

target_compile_features(enginemath INTERFACE cxx_std_17)

target_include_directories(enginemath INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(ENGINEMATH_IS_TOP_LEVEL OFF)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(ENGINEMATH_IS_TOP_LEVEL ON)
endif()

option(ENGINEMATH_BUILD_TESTS "Build enginemath tests" ${ENGINEMATH_IS_TOP_LEVEL})
option(ENGINEMATH_BUILD_EXAMPLES "Build enginemath examples" ${ENGINEMATH_IS_TOP_LEVEL})

if(ENGINEMATH_BUILD_TESTS)
    enable_testing()
    add_executable(enginemath_tests tests/test_vectors.cpp)
    target_link_libraries(enginemath_tests PRIVATE enginemath::enginemath)
    add_test(NAME enginemath_tests COMMAND $<TARGET_FILE:enginemath_tests>)
endif()

if(ENGINEMATH_BUILD_EXAMPLES)
    add_executable(basic examples/basic.cpp)
    target_link_libraries(basic PRIVATE enginemath::enginemath)
endif()

if(ENGINEMATH_IS_TOP_LEVEL)

    install(TARGETS enginemath EXPORT enginemathTargets)

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/enginemathConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/enginemathConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/enginemathConfig.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/enginemath"
    )

    install(EXPORT enginemathTargets
        NAMESPACE enginemath::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/enginemath"
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/enginemathConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/enginemathConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/enginemath"
    )

endif()