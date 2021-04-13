SET(TextEditor_SOURCE
        TextEditor.cpp
        TextEditor.h
        )


add_library(texteditor ${TextEditor_SOURCE})

SET_PROPERTY(TARGET texteditor APPEND PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        )

TARGET_LINK_LIBRARIES(texteditor
        LibSL_gl
        )


