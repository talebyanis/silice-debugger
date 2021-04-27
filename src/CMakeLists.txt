add_subdirectory(ImGuiColorTextEdit)
add_subdirectory(tinyfiledialogs)

add_executable(silice-text-editor
        tests/test-fstapi.h
        tests/test-fstapi.cpp
        LogParser.h
        LogParser.cpp
        FileDialog.h
        FileDialog.cpp
        FSTReader.h
        FSTReader.cpp
        FSTWindow.h
        FSTWindow.cpp 
        MainWindow.h
        MainWindow.cpp
        main.cpp
        )

FIND_PACKAGE(Threads REQUIRED)

target_link_libraries(silice-text-editor
        LibSL
        LibSL_gl
        texteditor
        tinyfiledialogs
        fstapi
        Threads::Threads
        implot
        nlohmann_json
        )


