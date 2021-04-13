#include <iostream>
#include <LibSL.h>
#include <LibSL_gl.h>
#include "imgui.h"
#include "TextEditor/TextEditor.h"

uint width = 800, height = 600;
bool initWindow = false;
std::string content[] = {};

TextEditor editor;
static const char* fileToEdit = "test_editing.ice";

void render() {
    char buf[256] = {0};
    float f;
    bool my_tool1_active;

    //Color
    //glClearColor(0.5, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    static const char* fileToEdit = "test.cpp";


    auto cpos = editor.GetCursorPosition();
    ImGui::Begin("Text Editor Demo", nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                auto textToSave = editor.GetText();
                /// save text....
            }
            if (ImGui::MenuItem("Quit", "Alt-F4"))
                std::cout<<"quit";
                //break;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            bool ro = editor.IsReadOnly();
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                editor.SetReadOnly(ro);
            ImGui::Separator();

            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
                editor.Undo();
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                editor.Redo();

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                editor.Copy();
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                editor.Cut();
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                editor.Delete();
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                editor.Paste();

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }


        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Dark palette"))
                editor.SetPalette(TextEditor::GetDarkPalette());
            if (ImGui::MenuItem("Light palette"))
                editor.SetPalette(TextEditor::GetLightPalette());
            if (ImGui::MenuItem("Retro blue palette"))
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                editor.IsOverwrite() ? "Ovr" : "Ins",
                editor.CanUndo() ? "*" : " ",
                editor.GetLanguageDefinition().mName.c_str(), fileToEdit);

    editor.Render("TextEditor");

    //---


    //ImGui::End();

    //Edit a color (stored as ~4 floats)
//    ImGui::ColorEdit4("Color", my_color);

// Plot some values
    //const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
    //ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

// Display contents in a scrolling region
    //ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
    //ImGui::BeginChild("Scrolling");
    //for (int n = 0; n < 50; n++)
    //    ImGui::Text("%04d: Some text", n);
    //ImGui::EndChild();

    ImGui::End();
    ImGui::Render();
}

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
void mainMousePressed(uint _x, uint _y, uint _button, uint _flags) { std::cout << "click" << std::endl; }

int main() {
    SimpleUI::init(800,600,"Hello, ImGui!");

    SimpleUI::onRender = render;
    SimpleUI::onReshape = onResize;
    SimpleUI::onKeyPressed = mainKeyPressed;
    SimpleUI::onScanCodePressed = mainScanCodePressed;
    SimpleUI::onMouseMotion = mainMouseMoved;
    SimpleUI::onMouseButtonPressed = mainMousePressed;

    SimpleUI::bindImGui();
    SimpleUI::initImGui();

    SimpleUI::loop();

    return 0;
}
