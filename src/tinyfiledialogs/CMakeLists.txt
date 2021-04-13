SET(tinyfiledialogs_SOURCE
        tinyfiledialogs.c
        tinyfiledialogs.h
        )


add_library(tinyfiledialogs ${tinyfiledialogs_SOURCE})

SET_PROPERTY(TARGET tinyfiledialogs APPEND PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        )