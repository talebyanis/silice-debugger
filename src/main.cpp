#include <iostream>
#include <LibSL.h>
#include <LibSL_gl.h>
#include "imgui.h"

uint width = 800, height = 600;
float my_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

void render() {
    char *buf;
    float f;
    bool my_tool1_active;
    bool my_tool2_active;

    glClearColor(0.5, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    //ImGui::SetNextWindowSize(ImVec2(width+2, height+2), ImGuiCond_Always);
    //ImGui::SetNextWindowPos(ImVec2(.0f, .0f), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(400, 0));
    ImGui::SetNextWindowSize(ImVec2(400, 100));

    ImGui::Begin("My First Tool", &my_tool1_active, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
            if (ImGui::MenuItem("Save", "Ctrl+S"))   { /* Do stuff */ }
            if (ImGui::MenuItem("Close", "Ctrl+W"))  { my_tool1_active = false; }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    //Edit a color (stored as ~4 floats)
    ImGui::ColorEdit4("Color", my_color);


// Plot some values
    //const float my_values[] = { 0.2f, 0.1f, 1.0f, 0.5f, 0.9f, 2.2f };
    //ImGui::PlotLines("Frame Times", my_values, IM_ARRAYSIZE(my_values));

// Display contents in a scrolling region
    //ImGui::TextColored(ImVec4(1,1,0,1), "Important Stuff");
    //ImGui::BeginChild("Scrolling");
    //for (int n = 0; n < 50; n++)
    //    ImGui::Text("%04d: Some text", n);


    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(100, 100));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(my_color[0], my_color[1],my_color[2], my_color[3]));
    ImGui::Begin("My Second Tool", &my_tool2_active);
    ImGui::PopStyleColor();
    ImGui::EndChild();
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
void mainMousePressed(uint _x, uint _y, uint _button, uint _flags) {}

int main() {
    SimpleUI::init(800,600,"Hello, ImGui!");

    SimpleUI::onRender = render;
    SimpleUI::onReshape = onResize;
    SimpleUI::onKeyPressed = mainKeyPressed;
    SimpleUI::onScanCodePressed = mainScanCodePressed;
    SimpleUI::onMouseMotion = mainMouseMoved;

    SimpleUI::bindImGui();
    SimpleUI::initImGui();

    SimpleUI::loop();

    return 0;
}
