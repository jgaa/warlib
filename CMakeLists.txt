cmake_minimum_required(VERSION 3.6)
project (warlib VERSION 2.2 LANGUAGES CXX)

set (WARLIB_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT DEFINED WITH_APIDOC)
    option(WITH_APIDOC "Generate Doxygen documentation")
endif()

if (NOT DEFINED WAR_WITH_UNIT_TESTS)
    option(WAR_WITH_UNIT_TESTS "Enable Unit Testing" ON)
    if (NOT DEFINED WAR_AUTORUN_UNIT_TESTS)
        option(WAR_AUTORUN_UNIT_TESTS "Run unit tests during build" ON)
    endif()
endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

include(cmake_scripts/external-projects.cmake)
include(cmake_scripts/add_and_run_test.cmake)

find_package( Threads )

# Include Boost as an imported target
find_package(Boost 1.65 REQUIRED COMPONENTS
    system
    program_options
    filesystem
    date_time
    context
    coroutine
    chrono
    )
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

install(DIRECTORY ${CMAKE_BINARY_DIR}/generated-include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT warlib-targets DESTINATION lib/warlib)
export(TARGETS warcore FILE warlib-exports.cmake)
