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
    std::map<std::string, std::pair<TextEditor, std::list<std::string>>> editors;

    void getSiliceFiles();

    LogParser lp;
public:
    void RenderDockspace();
    void ShowDockSpace();
    void ShowCodeEditors(TextEditor& editor, std::list<std::string>& algo_list);
    void ZoomMouseWheel(TextEditor& editor);
    void Init();
    void Render();
};
