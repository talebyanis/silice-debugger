//
// Created by antoine on 16/04/2021.
//

#include "FSTReader.h"
#include <vector>
#include <string>

#ifndef SILICE_TEXT_EDITOR_FSTWINDOW_H
#define SILICE_TEXT_EDITOR_FSTWINDOW_H

typedef struct {
    std::vector<int> x_data;
    std::vector<int> y_data;
    std::string name;
    fstHandle signalId;
} Plot;

class FSTWindow {
public:
    FSTWindow(std::string file);
    void render();
private:
    void addPlot(fstHandle signal);
    void removePlot(fstHandle signal);
    bool isDisplayed(fstHandle signal);
    void showPlots();
    void showPlotMenu();
};


#endif //SILICE_TEXT_EDITOR_FSTWINDOW_H
