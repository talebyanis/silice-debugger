#include <filesystem>
#include <TextEditor.h>

#ifdef WIN32
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

class MainWindow {

private:
    uint width = 800, height = 600;
    TextEditor editor;

public:
    void ShowDockSpace();
    void ShowCodeEditor();
};