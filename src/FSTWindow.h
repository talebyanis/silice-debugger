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
    std::vector<ImU64> x_data;
    std::vector<ImU64> y_data;
    std::string name;
    fstHandle signalId;
    ConvertType type;
    std::string customtype_string;
    ImVec4 color;
    bool fold;
    int maxY;
} Plot;

class FSTWindow {
public:
    std::string fstFilePath;
    FSTReader *g_Reader = nullptr;

    FSTWindow() = default;

    void load(const std::string& file, std::map<std::string, std::pair<TextEditor, std::list<std::string>>>& editors, LogParser& logParser);
    void load(json data, std::map<std::string, std::pair<TextEditor, std::list<std::string>>>& editors, LogParser& logParser);

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

    std::map<std::string, std::pair<TextEditor, std::list<std::string>>>* editors;
    std::list<std::string> algos_to_colorize;

    void clean();

    void addPlot(const std::vector<fstHandle>& signals);
    void removePlot(std::vector<fstHandle> signals);
    bool isDisplayed(std::vector<fstHandle> &signals);

    void showPlots();
    inline void drawErrors(Plot* item);
    inline void listenArrows(Plot* item);
    inline void drawValues(Plot* item, size_t leftIndex, size_t rightIndex);
    std::pair<std::string, int> parseCustomExp(const std::string& expression, int value);
    int binaryToDecimal(std::string n);

    void showRightClickPlotSettings(fstHandle signal);

    void showPlotMenu();
    void showScope(Scope &scope);
    void showSignalsMenu(Scope &scope, int &hiddenCount, bool internal);
    void showPairsMenu(Scope &scope, int &hiddenCount, bool internal);

    void loadQindex(Scope &scope);
};


#endif //SILICE_TEXT_EDITOR_FSTWINDOW_H
