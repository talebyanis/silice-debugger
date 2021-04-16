#include "FSTReader.h"

typedef struct {
    std::vector<int> x_data;
    std::vector<int> y_data;
    std::string name;
} Plot;

class MainWindow {
public:
    void ShowDockSpace();
    void ShowCodeEditor();
    void AddPlot(std::string file, fstHandle signal);
    void showPlots();
};
