#include "FSTReader.h"
#include <vector>
#include <string>
#include "./ImGuiColorTextEdit/TextEditor.h"
#include "../libs/implot/implot.h"
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
    std::vector<Plot> g_Plots;
    FSTReader *g_Reader = nullptr;
    ImPlotRange range = ImPlotRange(-1, -1);
    ImPlotRange *plotXLimits = nullptr;
    fstHandle hover = 0;
    fstHandle hoveredSignal = 0;
    fstHandle qindex = 0;
    std::vector<std::pair<int, int>> qindexValues;
    double markerX = 0;
    char filterBuffer[100] = {};
    TextEditor* editor;
    void addPlot(fstHandle signal);
    void removePlot(fstHandle signal);
    bool isDisplayed(fstHandle signal);
    void showPlots();
    void showPlotMenu();
};


#endif //SILICE_TEXT_EDITOR_FSTWINDOW_H
