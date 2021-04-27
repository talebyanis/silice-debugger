#include "FSTReader.h"
#include "FSTWindow.h"

/*
MainWindow :

Is used to spawn elements in an ImGui frame

*/
class MainWindow {
private:
    FSTWindow fstWindow;
    TextEditor editor;
public:
    void ShowDockSpace();
    void ShowCodeEditor();
    void ChangeStyle();
    void Render();
};
