cmake_minimum_required(VERSION 3.0)
project (warlib VERSION 2.1 LANGUAGES CXX)

include(cmake_scripts/external-projects.cmake)

set (WARLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED WITH_APIDOC)
    option(WITH_APIDOC "Generate Doxygen documentation")
endif()

if (NOT DEFINED WAR_WITH_UNIT_TESTS)
    option(WAR_WITH_UNIT_TESTS "Enable Unit Testing" ON)
endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

# Include Boost as an imported target
find_package(Boost 1.50 REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
    INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

add_subdirectory(core)

if (WIN32)
    set (WAR_SYSTEM_EOL \"\\r\\n\")
else()
    set (WAR_SYSTEM_EOL \"\\n\")
endif()

CONFIGURE_FILE(config.h.template ${CMAKE_BINARY_DIR}/generated-include/${PROJECT_NAME}/config.h)

