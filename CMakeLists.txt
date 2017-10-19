# Main project definition for warlibs
#
# This project is currently being developed with Debian "stretch", using the
# latest clang++ and g++ compilers and libraries.
#

cmake_minimum_required(VERSION 3.0)
project (warlib)

if (NOT DEFINED WARLIB_ROOT_DIR)
    set(WARLIB_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (NOT DEFINED WITH_APIDOC)
	option(WITH_APIDOC "Generate Doxygen documentation")
endif()

if (NOT DEFINED WAR_WITH_UNIT_TESTS)
	option(WAR_WITH_UNIT_TESTS "Enable Unit Testing" ON)
endif()

if (WIN32 AND NOT DEFINED WAR_USE_WIN32_DEFAULTS)
    option(WAR_USE_WIN32_DEFAULTS "Use C:/devel ... for openssl, zlib" ON)
endif()


# We create a configuration file so that other code that
# include our header files gets the correct configuration.
set(CONF_PATH ${WARLIB_ROOT_DIR}/include/war_config.h)

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

if (NOT EMBEDDED_WARLIB)

    if (NOT OPENSSL_ROOT_DIR AND WIN32 AND WAR_USE_WIN32_DEFAULTS)
        # Cmake's find_package(OpenSSL) is badly broken, and fails to work
        # with the default openssl build on windows.
        # In order to resolve the problems, make a 64 (or 32) bit build of
        # openssl and copy inc32/openssl to ${OPENSSL_ROOT_DIR}/include, and
        # the content of out32dll to ${OPENSSL_ROOT_DIR}/lib/VC
        # Make sure to also add ${OPENSSL_ROOT_DIR}/lib/VC to your path
        # when you run programs compiled with restc_cpp under windows, or
        # copy the dll's to the directory with your binaries.
        # (Using 3rd party C libraries under Windows with their own build systems
        # is a major pain-point. I'm planning to see if I can use Windows crypto
        # library in sted of openssl, (or somerthing that can be built as a
        # submodule to git with cmake)).
        set(OPENSSL_ROOT_DIR C:/devel/openssl)
        message(STATUS "Setting OPENSSL_ROOT_DIR to ${OPENSSL_ROOT_DIR}")
    endif()

    if (UNIX)
        set(THREADLIBS pthread)
    endif()

    set (DEFAULT_LIBRARIES
        ${DEFAULT_LIBRARIES}
        ${THREADLIBS}
        ${OPENSSL_LIBRARIES}
    )

    include(cmake_scripts/boost.cmake)
    include(cmake_scripts/pch.cmake)
    include(cmake_scripts/doxygen.cmake)

    if (UNIX)
    # For now, assume we use g++/clang++
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG=1 -o3 ")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -D_DEBUG=1 -o0 -g ")
        add_definitions(-D__USE_LARGEFILE64=1 -D__USE_FILE_OFFSET64=1
            -Wall -fPIC -std=c++1y -pthread
        )
    elseif(WIN32)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG=1 ")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -D_DEBUG=1 ")
        # We will support windows from Windows Vista
        add_definitions(-D_WIN32_WINNT=0x0600)
    endif()

    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib CACHE PATH "Destination location")
    link_directories(${LIBRARY_OUTPUT_PATH})

    include_directories(include)
endif()

add_subdirectory(src)

if (WAR_WITH_UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()


if (WIN32)
    set (WAR_SYSTEM_EOL \"\\r\\n\")
else()
    set (WAR_SYSTEM_EOL \"\\n\")
endif()

message(STATUS "Writing the current warlib configuration to ${CONF_PATH}")
CONFIGURE_FILE(war_config.h.template ${CONF_PATH})

