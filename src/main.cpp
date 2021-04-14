#include <iostream>
#include <filesystem>
#include <LibSL.h>
#include <LibSL_gl.h>
#include "imgui.h"
#include "TextEditor/TextEditor.h"
#include "FileDialog.h"

#ifdef WIN32
namespace fs = std::experimental::filesystem;
#else
namespace fs = std::filesystem;
#endif

uint width = 800, height = 600;

TextEditor editor;

static fs::path fileFullPath = "";

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto cpos = editor.GetCursorPosition();
    ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {

            if (ImGui::MenuItem("New", "Ctrl + N")) {
                auto fullpath = newFileDialog(OFD_EXTENSIONS);
                if (!fullpath.empty()) {
                    const fs::path path = fs::path(fullpath);
                    std::fstream newfile;
                    editor.SetText("");
                    fileFullPath = fullpath;
                }
            }

            if (ImGui::MenuItem("Open", "Ctrl + O")) {
                auto fullpath = openFileDialog(OFD_EXTENSIONS);
                if (!fullpath.empty()) {
                    fs::path path = fs::path(fullpath);
                    std::fstream newfile;
                    newfile.open(path,std::ios::in);
                    if (newfile.is_open()){
                        std::string tp;
                        editor.SetText("");
                        while(getline(newfile, tp)){
                            editor.InsertText(tp + "\n");
                        }
                        fileFullPath = path;
                        newfile.close();
                    }
                }
            }

            if (ImGui::MenuItem("Save","Ctrl + S", nullptr, !fileFullPath.string().empty())) {
                auto textToSave = editor.GetText();
                std::string path = fileFullPath.string();
                if(!path.empty()) {
                    std::fstream file(fileFullPath);
                    file << textToSave;
                }
            }

            if (ImGui::MenuItem("Save as", "Ctrl + Maj + S")) {
                auto textToSave = editor.GetText();
                std::string fullpath = saveFileDialog("file", OFD_FILTER_ALL);
                fileFullPath = fs::path(fullpath);
                if(!fullpath.empty()) {
                    std::fstream file(fullpath);
                    file.open(fullpath, std::ios::out);
                    file << textToSave;
                }
            }

            if (ImGui::MenuItem("Quit", "Alt-F4")) {
                std::cout << "quit";
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            bool ro = editor.IsReadOnly();
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro)){
                editor.SetReadOnly(ro);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo())) {
                editor.Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo())) {
                editor.Redo();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection())) {
                editor.Copy();
            }
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection())) {
                editor.Cut();
            }
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection())) {
                editor.Delete();
            }
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr)) {
                editor.Paste();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Dark palette")) {
                editor.SetPalette(TextEditor::GetDarkPalette());
            }
            if (ImGui::MenuItem("Light palette")) {
                editor.SetPalette(TextEditor::GetLightPalette());
            }
            if (ImGui::MenuItem("Retro blue palette")) {
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                editor.IsOverwrite() ? "Ovr" : "Ins",
                editor.CanUndo() ? "*" : " ",
                editor.GetLanguageDefinition().mName.c_str(), extractFileName(fileFullPath).c_str());

    editor.Render("TextEditor");

    ImGui::End();
    ImGui::Render();
}



/*
static ImVector<char> clipboard;

static const char *GetClipboard(void *) {
    if (clipboard != nullptr)
        std::cout << clipboard << std::endl;
    return clipboard->begin();
}

static void SetClipboard(void *, const char *text) {
    std::cout << text << std::endl;
    clipboard = ImVector(text);
}
*/

void onResize(uint _width, uint _height) {
    width = _width;
    height = _height;
}

void mainKeyPressed(uchar _k) { std::cout << "okok"; }

void mainScanCodePressed(uint _sc) { std::cout << "okok"; }

void mainScanCodeUnpressed(uint _sc) {}

void mainMouseMoved(uint _x, uint _y) {
    //std::cout << "okok";
    //fflush(stdout);
}

void mainMousePressed(uint _x, uint _y, uint _button, uint _flags) {  }

int main() {
    SimpleUI::init(800, 600, "");

    SimpleUI::onRender = render;
    SimpleUI::onReshape = onResize;
    SimpleUI::onKeyPressed = mainKeyPressed;

    //to handle shortcuts
    SimpleUI::onScanCodePressed = mainScanCodePressed;

    SimpleUI::onMouseMotion = mainMouseMoved;
    SimpleUI::onMouseButtonPressed = mainMousePressed;

    SimpleUI::bindImGui();
    SimpleUI::initImGui();

    /*ImGui::GetIO().GetClipboardTextFn = GetClipboard;
    ImGui::GetIO().SetClipboardTextFn = SetClipboard;*/

    SimpleUI::loop();

    return 0;
}
