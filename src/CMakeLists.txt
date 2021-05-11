add_subdirectory(ImGuiColorTextEdit)
add_subdirectory(tinyfiledialogs)

add_executable(silice-text-editor
        LogParser.h
        LogParser.cpp
        FileDialog.h
        FileDialog.cpp
        FSTWindow.h
        FSTWindow.cpp 
        MainWindow.h
        MainWindow.cpp
        main.cpp
        FST/Signal.h FST/DQPair.h FST/Scope.h FST/FSTReader.h
        FST/Signal.cpp FST/DQPair.cpp FST/Scope.cpp FST/FSTReader.cpp)

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


