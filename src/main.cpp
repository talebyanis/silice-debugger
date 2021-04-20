#include <LibSL_gl.h>
#include "imgui.h"
#include "tests/test-fstapi.h"
#include "MainWindow.h"
#include "../libs/implot/implot.h"

MainWindow mainWindow;

void render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainWindow.Render();
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
}

void mainKeyPressed(uchar _k) { }

void mainScanCodePressed(uint _sc) { }

void mainScanCodeUnpressed(uint _sc) { }

void mainMouseMoved(uint _x, uint _y) { }

void mainMousePressed(uint _x, uint _y, uint _button, uint _flags) {  }

int main() {
    SimpleUI::init(1280, 720, "");

    SimpleUI::onRender = render;
    SimpleUI::onReshape = onResize;
    SimpleUI::onKeyPressed = mainKeyPressed;

    //to handle shortcuts
    SimpleUI::onScanCodePressed = mainScanCodePressed;

    SimpleUI::onMouseMotion = mainMouseMoved;
    SimpleUI::onMouseButtonPressed = mainMousePressed;

    SimpleUI::bindImGui();
    SimpleUI::initImGui();

    ImGui::CreateContext();
    ImPlot::CreateContext();

    SimpleUI::onReshape(1280, 720);

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    /*ImGui::GetIO().GetClipboardTextFn = GetClipboard;
    ImGui::GetIO().SetClipboardTextFn = SetClipboard;*/

    SimpleUI::loop();

    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    return 0;
}
