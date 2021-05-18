#include "FST/FSTReader.h"
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
    HEXADECIMAL = 2,
    CUSTOM = 3,
};

typedef struct {
    std::vector<int> x_data;
    std::vector<int> y_data;
    std::string name;
    fstHandle signalId;
    ConvertType type;
    std::string customtype_string;
    ImVec4 color;
    bool fold;
} Plot;

class FSTWindow {
public:
    std::string fstFilePath;
    FSTReader *g_Reader = nullptr;

    FSTWindow() = default;

    void load(const std::string& file, TextEditor& editor);
    void load(json data, TextEditor &editors);

    void setAlgoToColorize(std::map<std::string, bool>& algo);
    void render();
    json save();

private:
    std::vector<Plot> g_Plots;
    std::map<std::string, ImVec4> g_ScopeColors;

    ImPlotRange range = ImPlotRange(-1, -1);
    ImPlotRange *plotXLimits = nullptr;

    fstHandle hoverHighLight = 0;
    fstHandle hoverRightClickMenu = 0;

    std::map<std::string, std::vector<std::pair<int, int>>> qindexValues;

    double markerX = 0;

    char filterBuffer[256] = {};
    char customFilterBuffer[256] = {};
    int bit_left_custom = 16;

    TextEditor* editor;
    std::list<std::string> algos_to_colorize;

    void clean();

    void addPlot(const std::vector<fstHandle>& signals);
    void removePlot(std::vector<fstHandle> signals);
    bool isDisplayed(std::vector<fstHandle> signals);

    void showPlots();
    inline void drawErrors(Plot* item);
    inline void listenArrows(Plot* item);
    inline void drawValues(Plot* item);
    std::pair<std::string, int> parseCustomExp(const std::string& expression, int value);
    int binaryToDecimal(std::string n);

    void showRightClickPlotSettings(fstHandle signal);

    void showPlotMenu();
    void showSignalsMenu(Scope scope, int &hiddenCount);
    void showPairsMenu(Scope scope, int &hiddenCount);

    void loadQindex();
};


#endif //SILICE_TEXT_EDITOR_FSTWINDOW_H
