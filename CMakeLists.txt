# Main project definition for warlibs
#
# This project is currently being developed with Debian "testing", using the
# latest clang++ and g++ compilers and libraries.
#
# AFAIK, the Microsoft C++ compiler is not yet up to specs with C++14, so I have
# not made any attempts to prepare this CMake project for msvc.
# C++14 mile-stone.
#

cmake_minimum_required(VERSION 3.0)
project (Warlib)

include(cmake_scripts/boost.cmake)

if (NOT DEFINED WITH_APIDOC)
	option(WITH_APIDOC "Generate Doxygen documentation")
endif()

if (NOT DEFINED WAR_WITH_UNIT_TESTS)
	option(WAR_WITH_UNIT_TESTS "Enable Unit Testing" ON)
endif()

if (NOT DEFINED WARLIB_ROOT_DIR)
	set(WARLIB_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# We create a configuration file so that other code that
# include our header files gets the correct configuration.
set(CONF_PATH ${PROJECT_SOURCE_DIR}/include/war_config.h)

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

if (UNIX)
  # For now, assume we use g++/clang++
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG=1 -o3 ")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1 -D_DEBUG=1 -o0 -g ")
    add_definitions(-D__USE_LARGEFILE64=1 -D__USE_FILE_OFFSET64=1
        -Wall -fPIC -std=c++1y -pthread
    )
endif()

if (BOOST_ROOT)
	if (NOT BOOST_INCLUDE_DIR)
		set(BOOST_INCLUDE_DIR ${BOOST_ROOT})
	endif()

	if (NOT BOOST_LIBRARY_DIRS)
		set(BOOST_LIBRARY_DIRS ${BOOST_ROOT}/stage/lib CACHE PATH "path to prebuilt library")
	endif()
else()
    message(STATUS "No BOOST_ROOT set. Assuming that boost is installed and available for the compiler")
endif()

if (BOOST_INCLUDE_DIR)
	message(STATUS "Setting boost include-dir to: ${BOOST_INCLUDE_DIR}")
	include_directories(${BOOST_INCLUDE_DIR})
endif()

if (BOOST_LIBRARY_DIRS)
	message(STATUS "Setting boost libdir to: ${BOOST_LIBRARY_DIRS}")
	link_directories(${BOOST_LIBRARY_DIRS} )
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib CACHE PATH "Destination location")
link_directories(${LIBRARY_OUTPUT_PATH})

include_directories(
	${PROJECT_SOURCE_DIR}/include
	)

add_subdirectory(src)

if (WAR_WITH_UNIT_TESTS)
	add_subdirectory(tests)
endif()


if (WIN32)
    set (WAR_SYSTEM_EOL \"\\r\\n\")
else()
    set (WAR_SYSTEM_EOL \"\\n\")
endif()

message(STATUS "Writing the current configuration to ${CONF_PATH}")
CONFIGURE_FILE(war_config.h.template ${CONF_PATH})

include(cmake_scripts/doxygen.cmake)
