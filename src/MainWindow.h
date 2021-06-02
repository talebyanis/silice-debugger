#include "FST/FSTReader.h"
#include "FSTWindow.h"

/*
MainWindow :

Is used to spawn elements in an ImGui frame

*/
class MainWindow {
private:
    FSTWindow fstWindow;
    //TextEditor editor;
    // We want to open every silice file linked w/ the design
    std::map<std::string, TextEditor> editors;

    void getSiliceFiles();
public:
    void ShowDockSpace();
    void ShowCodeEditors(TextEditor& editor);
    void ZoomMouseWheel(TextEditor& editor);
    void Init();
    void Render();
};
