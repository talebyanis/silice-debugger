#include <iostream>
#include <filesystem>
#include <LibSL.h>
#include "imgui.h"
#include "TextEditor/TextEditor.h"
#include "FileDialog.h"
#include "tests/test-fstapi.h"
#include "MainWindow.h"

static fs::path fileFullPath;
ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
                                | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
bool p_open_dockspace = true;

void MainWindow::ShowDockSpace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &p_open_dockspace, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Docking"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            if (ImGui::MenuItem("Flag: NoSplit",                "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            if (ImGui::MenuItem("Flag: NoResize",               "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            if (ImGui::MenuItem("Flag: PassthruCentralNode",    "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))     dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            if (ImGui::MenuItem("Flag: AutoHideTabBar",         "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            ImGui::Separator();
            if (ImGui::MenuItem("Close DockSpace", nullptr, false, p_open_dockspace != NULL))
                p_open_dockspace = false;
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void MainWindow::ShowCodeEditor()
{
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
}