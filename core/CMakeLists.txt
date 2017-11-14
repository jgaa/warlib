
project (warcore VERSION 2.1 LANGUAGES CXX)

set(ACTUAL_SOURCES
    src/WarLog.cpp
    src/ostream_operators.cpp
    src/WarThreadpool.cpp
    src/WarPipeline.cpp
    )

if (WIN32)
    set(SOURCES ${ACTUAL_SOURCES}
        src/win/filecheck.cpp
        src/win/minidump.cpp
        ${HEADERS} ${RESFILES})
else()
    set(SOURCES ${ACTUAL_SOURCES})
endif()

set(HEADERS
    ${WARLIB_ROOT}/include/restc-cpp/restc-cpp.h
    )

add_library(warcore ${SOURCES})

target_include_directories(warcore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

set(CMAKE_CXX_STANDARD 14)

target_compile_definitions(${PROJECT_NAME} PUBLIC -DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DBOOST_COROUTINE_NO_DEPRECATION_WARNING=1)

target_link_libraries(${PROJECT_NAME} PUBLIC ${Boost_LIBRARIES})

install(TARGETS warcore EXPORT MyLibraryConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if (WAR_WITH_UNIT_TESTS)

    enable_testing()

    # Unit tests
    add_executable(warcore_test_tasks
        tests/test_tasks.cpp)
    target_link_libraries(warcore_test_tasks
        boost
        warcore)
    add_dependencies(warcore_test_tasks externalLest)
    target_include_directories(warcore_test_tasks PRIVATE tests)
    add_and_run_test(warcore_test_tasks ${CMAKE_CURRENT_BINARY_DIR})

    # Performance tests
    add_executable(warcore_perf_test
        tests/perftests.cpp)
    target_link_libraries(warcore_perf_test
        boost
        warcore)
    add_dependencies(warcore_perf_test externalLest)
    target_include_directories(warcore_perf_test PRIVATE tests)
    add_test(warcore_perf_test warcore_perf_test)
endif()
