add_subdirectory(TextEditor)

add_executable(silice-text-editor main.cpp TextEditor/TextEditor.h)

target_link_libraries(silice-text-editor
        LibSL
        LibSL_gl
)
