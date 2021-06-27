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

class FSTWindow 
{

public:

    std::string m_FstFilePath;

    FSTWindow() = default;

    void load(const std::string& file, std::map<std::string, std::pair<TextEditor, std::list<std::string>>>& editors, LogParser& logParser);
    void load(json data, std::map<std::string, std::pair<TextEditor, std::list<std::string>>>& editors, LogParser& logParser);

    void setAlgoToColorize(std::map<std::string, bool>& algo);
    void render();
    json save();

    const FSTReader *reader() const { return m_Reader; }

private:

    FSTReader *m_Reader = nullptr;

    std::vector<Plot> m_Plots;
    std::map<std::string, ImVec4> m_ScopeColors;

    ImPlotRange m_Range = ImPlotRange(-1, -1);
    ImPlotRange *m_PlotXLimits = nullptr;

    fstHandle m_HoverHighLight = 0;
    fstHandle m_HoverRightClickMenu = 0;

    std::map<std::string, std::vector<std::pair<ImU64, ImU64>>> m_qindexValues;
    std::map<std::string, std::vector<std::pair<ImU64, ImU64>>> m_qindexValues_save;

    bool   m_ResetPlotLimits = false;

    double m_MarkerX = 0;

    char m_FilterBuffer[256] = {};
    char m_CustomFilterBuffer[256] = {};
    int m_BitLeftCustom = 16;

    std::map<std::string, std::pair<TextEditor, std::list<std::string>>>* m_Editors;
    std::list<std::string> m_AlgosToColorize;

    void clean();

    void addPlot(const std::vector<fstHandle>& signals);
    void removePlot(std::vector<fstHandle> signals);
    bool isDisplayed(std::vector<fstHandle> &signals);

    void showPlots();
    inline void drawErrors(Plot* item);
    inline bool listenArrows(Plot* item);
    inline void drawValues(Plot* item, size_t leftIndex, size_t rightIndex, size_t ratio);
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
