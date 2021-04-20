set(TextEditor_SOURCE
        TextEditor.h
        TextEditor.cpp
)

add_library(texteditor
        ${TextEditor_SOURCE}
)

set_property(TARGET texteditor APPEND PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_link_libraries(texteditor
        LibSL_gl
        )