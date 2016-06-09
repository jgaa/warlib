
project(warlib_unit_tests)
set(ACTUAL_SOURCES
    test_tasks.cpp
    )

set(HEADERS
	${WARLIB_ROOT_DIR}/include/tasks/WarPipeline.h
	)

if (WIN32)
	set(SOURCES ${ACTUAL_SOURCES} ${HEADERS} ${RESFILES})
else()
	set(SOURCES ${ACTUAL_SOURCES})
endif()

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(warlib_unit_tests ${SOURCES})
target_link_libraries(warlib_unit_tests
    ${DEFAULT_LIBRARIES}
    ${BOOST_UNIT_TEST_LIBRARIES}
    war_log
    war_tasks
  )

add_executable(perftests perftests.cpp)
target_link_libraries(perftests
    ${DEFAULT_LIBRARIES}
    war_log
    war_tasks)

add_test(WARLIB_UNITTESTS warlib_unit_tests)
add_test(WARLIB_PERFORMANCE_TEST perftests)
