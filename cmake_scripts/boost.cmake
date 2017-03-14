
include (CheckIncludeFiles)

if (WIN32)
    # The cmake find boost script is usually broken for Windows.
    # In stead, we will require BOOST_ROOT and BOOST_LIBRARYDIR
    # to be set manually.

    if (WAR_USE_WIN32_DEFAULTS AND NOT DEFINED BOOST_ROOT)
            set (BOOST_ROOT C:/devel/boost_1_63_0)
            message(STATUS "Warning. No BOOST_ROOT defined. Defaulting to ${BOOST_ROOT}")
    else()
            message(STATUS "Using BOOST from ${BOOST_ROOT}")
    endif()

    if (WAR_USE_WIN32_DEFAULTS AND NOT DEFINED BOOST_LIBRARYDIR)
            set (BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/x64/lib)
            message(STATUS "Warning. No BOOST_ROOT defined. Defaulting to ${BOOST_ROOT}")
    endif()

    include_directories(${BOOST_ROOT})
    link_directories(${BOOST_LIBRARYDIR})

    if (EXISTS ${BOOST_ROOT}/boost/type_index.hpp)
        set(RESTC_CPP_HAVE_BOOST_TYPEINDEX 1)
    endif()

    set(Boost_USE_STATIC_LIBS ON)
    set(Boost_USE_MULTITHREADED ON)
    unset(Boost_INCLUDE_DIR CACHE)
    unset(Boost_LIBRARY_DIRS CACHE)

# Msvc and possible some other Windows-compilers will link
# to the correct libraries trough #pragma directives in boost headers.
endif()

if (UNIX OR (WIN32 AND NOT WAR_USE_WIN32_DEFAULTS))
    set(Boost_USE_MULTITHREADED ON)
    find_package(Boost REQUIRED COMPONENTS
        system
        program_options
        filesystem
        date_time
        context
        coroutine
        chrono
        serialization
        iostreams
        regex
        unit_test_framework
        )

    include_directories(${Boost_INCLUDE_DIRS})
endif()

if (UNIX)
    set (BOOST_LIBRARIES
        debug ${Boost_SYSTEM_LIBRARY_DEBUG} optimized ${Boost_SYSTEM_LIBRARY_RELEASE}
        debug ${Boost_PROGRAM_OPTIONS_LIBRARY_DEBUG} optimized ${Boost_PROGRAM_OPTIONS_LIBRARY_RELEASE}
        debug ${Boost_FILESYSTEM_LIBRARY_DEBUG} optimized ${Boost_FILESYSTEM_LIBRARY_RELEASE}
        debug ${Boost_DATE_TIME_LIBRARY_DEBUG} optimized ${Boost_DATE_TIME_LIBRARY_RELEASE}
        debug ${Boost_COROUTINE_LIBRARY_DEBUG} optimized ${Boost_COROUTINE_LIBRARY_RELEASE}
        debug ${Boost_CONTEXT_LIBRARY_DEBUG} optimized ${Boost_CONTEXT_LIBRARY_RELEASE}
        debug ${Boost_CHRONO_LIBRARY_DEBUG} optimized ${Boost_CHRONO_LIBRARY_RELEASE}
        debug ${Boost_SERIALIZATION_LIBRARY_DEBUG} optimized ${Boost_SERIALIZATION_LIBRARY_RELEASE}
        debug ${Boost_IOSTREAMS_LIBRARY_DEBUG} optimized ${Boost_IOSTREAMS_LIBRARY_RELEASE}
        debug ${Boost_REGEX_LIBRARY_DEBUG} optimized ${Boost_REGEX_LIBRARY_RELEASE}
        )

    set (BOOST_UNIT_TEST_LIBRARIES
        debug ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_DEBUG} optimized ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY_RELEASE}
        )

    if (EXISTS ${Boost_INCLUDE_DIRS}/boost/type_index.hpp)
        set(WAR_HAVE_BOOST_TYPEINDEX 1)
    endif()
    CHECK_INCLUDE_FILES(${Boost_INCLUDE_DIRS}/boost/type_index.hpp WAR_HAVE_BOOST_TYPEINDEX)
endif()

if (UNIX)
    set (DEFAULT_LIBRARIES
        ${DEFAULT_LIBRARIES}
        ${BOOST_LIBRARIES}
        )
endif()

# Asio trigger deprecation warnings
add_definitions(-DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1)
add_definitions(-DBOOST_COROUTINE_NO_DEPRECATION_WARNING=1)

message(STATUS "Default libraries: ${DEFAULT_LIBRARIES}")

