
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

# target_compile_features(warcore
#     PUBLIC cxx_auto_type cxx_raw_string_literals cxx_strong_enums cxx_user_literals
#     )

target_compile_definitions(${PROJECT_NAME} PUBLIC -DBOOST_COROUTINES_NO_DEPRECATION_WARNING=1)
target_compile_definitions(${PROJECT_NAME} PUBLIC -DBOOST_COROUTINE_NO_DEPRECATION_WARNING=1)


target_link_libraries(warcore
    boost
    )

install(TARGETS warcore EXPORT MyLibraryConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

