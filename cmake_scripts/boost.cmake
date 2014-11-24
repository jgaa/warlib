
if (WIN32)
    # Msvc and possible some other Windows-compilers will link
    # to the correct libraries trough #pragma directives in boost headers.
else ()
    set(LIB_BOOST_PROGRAM_OPTIONS boost_program_options)
    set(LIB_BOOST_SERIALIZATION boost_serialization)
    set(LIB_BOOST_FILESYSTEM boost_filesystem)
    set(LIB_BOOST_DATE_TIME boost_date_time)
    set(LIB_BOOST_IOSTREAMS boost_iostreams)
    set(LIB_BOOST_SYSTEM boost_system)
    set(LIB_BOOST_REGEX boost_regex)
    set(LIB_BOOST_CONTEXT boost_context)
    set(LIB_BOOST_COROUTINE boost_coroutine)
    set(LIB_BOOST_CHRONO boost_chrono)
endif ()

set (BOOST_LIBRARIES
    ${LIB_BOOST_SYSTEM}
    ${LIB_BOOST_PROGRAM_OPTIONS}
    ${LIB_BOOST_SERIALIZATION}
    ${LIB_BOOST_FILESYSTEM}
    ${LIB_BOOST_DATE_TIME}
    ${LIB_BOOST_IOSTREAMS}
    ${LIB_BOOST_REGEX}
    ${LIB_BOOST_CONTEXT}
    ${LIB_BOOST_COROUTINE}
    ${LIB_BOOST_CHRONO}
    )

set (DEFAULT_LIBRARIES
    ${DEFAULT_LIBRARIES}
    pthread
    ${BOOST_LIBRARIES}
    )
