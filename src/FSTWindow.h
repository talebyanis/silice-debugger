#include "FSTReader.h"
#include <vector>
#include <string>
#include "./ImGuiColorTextEdit/TextEditor.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#ifndef SILICE_TEXT_EDITOR_FSTWINDOW_H
#define SILICE_TEXT_EDITOR_FSTWINDOW_H

enum ConvertType {
    BINARY = 0,
    DECIMALS = 1,
    HEXADECIMAL = 2
};

typedef struct {
    std::vector<int> x_data;
    std::vector<int> y_data;
    std::string name;
    fstHandle signalId;
    ConvertType type;
    ImVec4 color;
} Plot;

class FSTWindow {
public:
    std::string fstFilePath;
    FSTWindow(std::string file, TextEditor& editor);
    FSTWindow(json data, TextEditor &editors);
    void render();
    json save();
private:
    TextEditor* editor;
    void addPlot(fstHandle signal);
    void removePlot(fstHandle signal);
    bool isDisplayed(fstHandle signal);
    void showPlots();
    void showPlotMenu();
};


#endif //SILICE_TEXT_EDITOR_FSTWINDOW_H
