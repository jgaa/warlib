
project(war_tasks)
set(ACTUAL_SOURCES
    WarPipeline.cpp
    WarThreadpool.cpp
    )
set(HEADERS
    war_tasks.h
    ${WARLIB_ROOT_DIR}/include/tasks/WarPipeline.h
    ${WARLIB_ROOT_DIR}/include/tasks/WarThreadpool.h
    ${WARLIB_ROOT_DIR}/include/war_asio.h
    )

if (WIN32)
    ADD_MSVC_PRECOMPILED_HEADER(war_tasks.h pch.cpp "${ACTUAL_SOURCES}")
    add_definitions(-DWAR_PCH)
    set(SOURCES ${ACTUAL_SOURCES} pch.cpp ${HEADERS} ${RESFILES})
else()
    set(SOURCES ${ACTUAL_SOURCES})
endif()

add_library(war_tasks ${SOURCES})
set_target_properties(war_tasks PROPERTIES DEBUG_OUTPUT_NAME war_tasksD)
target_link_libraries(war_tasks ${BOOST_LIBRARIES} war_log
    boost_unit_test_framework
    boost_system
    boost_filesystem)
