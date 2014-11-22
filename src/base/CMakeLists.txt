
project(base)
set(ACTUAL_SOURCES ostream_operators.cpp)

if (WIN32)
	set(SOURCES ${ACTUAL_SOURCES} ${HEADERS} ${RESFILES})
else()
	set(SOURCES ${ACTUAL_SOURCES})
endif()

add_library(war_base ${SOURCES})
set_target_properties(war_base PROPERTIES DEBUG_OUTPUT_NAME war_baseD)
