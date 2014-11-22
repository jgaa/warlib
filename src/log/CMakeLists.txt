
project(war_log)
set(ACTUAL_SOURCES WarLog.cpp)
set(HEADERS ${WARLIB_ROOT_DIR}/include/log/WarLog.h)


if (WIN32)
	set(SOURCES ${ACTUAL_SOURCES} ${HEADERS} ${RESFILES})
else()
	set(SOURCES ${ACTUAL_SOURCES})
endif()

add_library(war_log ${SOURCES})
set_target_properties(war_log PROPERTIES DEBUG_OUTPUT_NAME war_logD)
target_link_libraries(war_log war_base)
