add_subdirectory(TextEditor)
add_subdirectory(tinyfiledialogs)

add_executable(silice-text-editor
        tests/test-fstapi.h
        tests/test-fstapi.cpp
        FileDialog.h
        FileDialog.cpp
        MainWindow.h
        MainWindow.cpp
        main.cpp
        FSTReader.cpp FSTReader.h)

FIND_PACKAGE(Threads REQUIRED)

target_link_libraries(silice-text-editor
        LibSL
        LibSL_gl
        texteditor
        tinyfiledialogs
        fstapi
        Threads::Threads
        )
