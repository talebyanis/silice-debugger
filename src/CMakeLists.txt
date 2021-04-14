add_subdirectory(TextEditor)
add_subdirectory(tinyfiledialogs)

add_executable(silice-text-editor
        main.cpp
        FileDialog.h
        FileDialog.cpp
        mainFST.cpp)

target_link_libraries(silice-text-editor
        LibSL
        LibSL_gl
        texteditor
        tinyfiledialogs
        )
