#include "FSTWindow.h"
#include <algorithm>
#include <list>
#include <iostream>
#include "imgui.h"
#include <bitset>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

//-------------------------------------------------------

//Shows the plots' names on the right to click on them and display matching plots
void FSTWindow::showPlotMenu() {
    //For every scope we have one TreeNode
    for (const auto &item : g_Reader->getScopes()) {
        if (ImGui::TreeNode(item.c_str())) {
            int count = 0;
            for (const auto &signal : g_Reader->getSignals(item)) {
                std::string name = g_Reader->getSignalName(signal);
                if (name.find(filterBuffer) != std::string::npos) {
                    //std::cout << filterBuffer << " " << name << " " << (name.find(filterBuffer) == std::string::npos) << std::endl;
                    if (hover == signal) {
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1, 0.35, 0.10, 1));
                    }
                    ImGui::PushID(count);
                    if (ImGui::MenuItem(name.c_str(), "", FSTWindow::isDisplayed(signal))) {
                        if (!FSTWindow::isDisplayed(signal)) {
                            this->addPlot(signal);
                        } else {
                            this->removePlot(signal);
                        }
                    }
                    if (ImGui::IsItemHovered()) {
                        hoveredSignal = signal;
                    }
                    ImGui::PopID();
                    if (hover == signal) {
                        ImGui::PopStyleColor();
                    }
                    count++;
                }
            }
            ImGui::TreePop();
        }
    }

    Plot *plot = nullptr;
    for (Plot &item : g_Plots) {
        if (item.signalId == hoveredSignal) {
            plot = &item;
        }
    }
    if (plot) {
        if (ImGui::BeginPopupContextWindow()) {
            ImGui::Text(g_Reader->getSignalName(hoveredSignal).c_str());
            if (ImGui::MenuItem("Binary")) {
                plot->type = BINARY;
            }
            if (ImGui::MenuItem("Decimal")) {
                plot->type = DECIMALS;
            }
            if (ImGui::MenuItem("Hexadecimal")) {
                plot->type = HEXADECIMAL;
            }
            ImGui::ColorPicker4("Color Picker", (float *) &plot->color);
            ImGui::EndPopup();
        }
    }
    ImGui::InputText("Filter", filterBuffer, sizeof(filterBuffer));
}

//-------------------------------------------------------

//Adds a plot to the list
void FSTWindow::addPlot(fstHandle signal) {
    if (!g_Reader) {
        return;
    }
    Plot plot;
    plot.signalId = signal;
    std::string signalName = g_Reader->getSignalName(signal);
    plot.name = signalName;
    plot.type = DECIMALS;
    plot.color = ImVec4(1, 1, 1, 1);
    valuesList values = g_Reader->getValues(signal);
    for (const auto &item : values) {
        plot.x_data.push_back(item.first);
        plot.y_data.push_back(item.second);
    }

    g_Plots.push_back(plot);
}

//-------------------------------------------------------

//Removes a plot from the list
void FSTWindow::removePlot(fstHandle signal) {
    g_Plots.erase(remove_if(
            g_Plots.begin(),
            g_Plots.end(),
            [signal](Plot plot) {
                return plot.signalId == signal;
            }), g_Plots.end());
}

//-------------------------------------------------------

bool FSTWindow::isDisplayed(fstHandle signal) {
    auto res = std::find_if(g_Plots.begin(), g_Plots.end(), [signal](Plot plot) {
        return plot.signalId == signal;
    });
    return res != g_Plots.end();
}

//-------------------------------------------------------

//Shows plots on the right of the window
int payload;

void FSTWindow::showPlots() {
    ImVec2 wSize = ImGui::GetWindowSize();
    ImGui::BeginGroup();
    //reset hover to not change the color of a name if no plot is hovered
    hover = 0;
    for (int i = 0; i < g_Plots.size(); i++) {
        Plot item = g_Plots[i];

        //set the plots Y limits to just below the lowest value to just upper the highest
        double max = *std::max_element(item.y_data.begin(), item.y_data.end());
        ImPlot::SetNextPlotLimitsY(0.0 - max / 3, max + max / 3);

        //Cloning in other values to prevent LinkNextPlot from modifying values (SetNextPlotLimitsX not working idk why)
        double xMin = plotXLimits->Min;
        double xMax = plotXLimits->Max;
        ImPlot::LinkNextPlotLimits(&xMin, &xMax, nullptr, nullptr);

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::Button("  ");
        //For drag&drop
        if (ImGui::IsItemHovered() && !ImGui::IsAnyMouseDown()) {
            payload = i;
        }
        ImGui::SetCursorScreenPos(cursor);

        //Drag&drop source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("PlotPayload", &payload, sizeof(fstHandle));
            ImGui::Text(g_Reader->getSignalName(g_Plots[payload].signalId).c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::PushID(i);
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(10, 0));
        //Coloring the line
        ImPlot::PushStyleColor(ImPlotCol_Line, item.color);
        if (ImPlot::BeginPlot(item.name.c_str(), NULL, NULL, ImVec2(-1, 100),
                              ImPlotFlags_NoLegend | ImPlotFlags_NoChild, NULL,
                              ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels)) {

            //Drag&drop target
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *pload = ImGui::AcceptDragDropPayload("PlotPayload")) {
                    if (payload > i) {
                        for (int k = payload; k > i; k--) {
                            std::swap(g_Plots[k], g_Plots[k - 1]);
                        }
                    } else if (i > payload) {
                        for (int k = payload; k < i; k++) {
                            std::swap(g_Plots[k], g_Plots[k + 1]);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            ImPlot::DragLineX("Marker", &markerX, true, ImVec4(1, 0.5, 0.5, 1), 1);
            ImPlot::PlotStairs(item.name.c_str(), (int *) &item.x_data[0], (int *) &item.y_data[0], item.x_data.size());
            //If the mouse is hover the plot, we take it's id to change the color of the right name on the left
            //and we set global X limits to this plot's
            if (ImPlot::IsPlotHovered()) {
                ImPlotLimits limits = ImPlot::GetPlotLimits();
                hover = item.signalId;
                plotXLimits->Min = limits.X.Min;
                plotXLimits->Max = limits.X.Max;
                ImGui::SetKeyboardFocusHere();
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow), true)) {
                    for (int ii = 0; ii < item.x_data.size(); ii++) {
                        int x = item.x_data[ii];
                        if (markerX < x) {
                            markerX = x;
                            if (markerX > plotXLimits->Max || markerX < plotXLimits->Min) {
                                double distance = plotXLimits->Max - plotXLimits->Min;
                                plotXLimits->Min = markerX - (distance / 2);
                                plotXLimits->Max = markerX + (distance / 2);
                            }
                            break;
                        }
                    }
                }
                if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow), true)) {
                    for (int ii = item.x_data.size() - 1; ii >= 0; ii--) {
                        int x = item.x_data[ii];
                        if (markerX > x) {
                            markerX = x;
                            if (markerX > plotXLimits->Max || markerX < plotXLimits->Min) {
                                double distance = plotXLimits->Max - plotXLimits->Min;
                                plotXLimits->Min = markerX - (distance / 2);
                                plotXLimits->Max = markerX + (distance / 2);
                            }
                            break;
                        }
                    }
                }
            }
            ImPlot::PushStyleColor(ImPlotCol_LegendText, ImVec4(0.15, 0.35, 0.15, 1));
            //displaying values on the plot
            for (int i = 0; i < item.x_data.size(); i++) {
                std::basic_string<char> value;
                switch (item.type) {
                    case BINARY:
                        value = std::bitset<16>(item.y_data[i]).to_string();
                        //remove all 0 in front
                        value.erase(0, value.find_first_not_of('0'));
                        break;
                    case DECIMALS:
                        value = std::to_string(item.y_data[i]);
                        break;
                    case HEXADECIMAL:
                        std::stringstream stream;
                        stream << std::hex << item.y_data[i];
                        value = stream.str();
                        break;
                }
                ImVec2 offset = ImVec2(0, 0);

                if (i > 0) {
                    if (item.y_data[i] > item.y_data[i - 1]) {
                        offset.y = -6;
                    } else {
                        offset.y = 6;
                    }
                }
                ImPlot::PlotText(value.c_str(), item.x_data[i], item.y_data[i], false, offset);
            }


            ImPlot::PopStyleColor(2);
            ImPlot::PopStyleVar();
            ImPlot::EndPlot();

            ImGui::PopID();
        }
    }
    ImGui::EndGroup();
}

//-------------------------------------------------------

void FSTWindow::render() {
    int treeWidth = 250;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(treeWidth + 500, 500), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowDockID(0x2);
    ImGui::Begin("PlotWindow", nullptr,
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    {
        ImVec2 wPos = ImGui::GetCursorScreenPos();
        ImVec2 wSize = ImGui::GetWindowSize();
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
        ImGui::BeginChild("Tree", ImVec2(treeWidth, wSize.y), true,
                          ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            this->showPlotMenu();
        }
        ImGui::EndChild();

        ImGui::SetNextWindowPos(ImVec2(wPos.x + treeWidth, wPos.y));
        ImGui::BeginChild("Graph", ImVec2(wSize.x - treeWidth, wSize.y), true,
                          ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            this->showPlots();
        }
        ImGui::End();
    }
    ImGui::End();
    ImGui::PopStyleVar(); // Padding

    int index = -1;
    int tmp = 0;

    for (int ii = 1; ii < qindexValues.size(); ii++) {
        if (markerX < qindexValues[ii].first && markerX >= qindexValues[tmp].first) {
            index = tmp - 1;
            break;
        }
        tmp = ii;
    }

    if (index != -1) {
        editor->FSMframeAtIndex(editor->openedFile, index);
    } else {
        editor->FSMunframe();
    }
}

//-------------------------------------------------------

json FSTWindow::save() {
    json j;
    j["filePath"] = this->fstFilePath;
    j["rangeMin"] = range.Min;
    j["rangeMax"] = range.Max;
    j["markerX"] = markerX;
    std::vector<fstHandle> displayedPlots;
    std::vector<ConvertType> displayedTypes;
    for (const auto &item : g_Plots) {
        displayedPlots.push_back(item.signalId);
        displayedTypes.push_back(item.type);
    }
    j["displayedSignals"] = displayedPlots;
    j["displayedTypes"] = displayedTypes;
    return j;
}

//-------------------------------------------------------

FSTWindow::FSTWindow(std::string file, TextEditor &editors) {
    this->fstFilePath = file;
    g_Reader = new FSTReader(file.c_str());
    if (plotXLimits == nullptr) {
        plotXLimits = &range;
        double maxTime = g_Reader->getMaxTime();
        plotXLimits->Min = 0 - (maxTime / 20);
        plotXLimits->Max = maxTime + (maxTime / 20);
    }
    this->editor = &editors;

    for (const auto &item : g_Reader->getScopes()) {
        for (const auto &signal : g_Reader->getSignals(item)) {
            if (g_Reader->getSignalName(signal).find("_q_index") != std::string::npos) {
                qindex = signal;
            }
        }
    }
    valuesList valuesList = g_Reader->getValues(qindex);
    for (const auto &item : valuesList) {
        qindexValues.push_back(std::make_pair(item.first, item.second));
    }
}

FSTWindow::FSTWindow(json data, TextEditor &editors) {
    this->fstFilePath = data["filePath"];
    g_Reader = new FSTReader(this->fstFilePath.c_str());
    range = ImPlotRange(data["rangeMin"], data["rangeMax"]);
    plotXLimits = &range;
    this->editor = &editors;
    markerX = data["markerX"];

    for (const auto &item : g_Reader->getScopes()) {
        for (const auto &signal : g_Reader->getSignals(item)) {
            if (g_Reader->getSignalName(signal).find("_q_index") != std::string::npos) {
                qindex = signal;
            }
        }
    }
    valuesList valuesList = g_Reader->getValues(qindex);
    for (const auto &item : valuesList) {
        qindexValues.push_back(std::make_pair(item.first, item.second));
    }
    for (int i = 0; i < data["displayedSignals"].size(); ++i) {
        this->addPlot(data["displayedSignals"][i]);
        g_Plots[i].type = data["displayedTypes"][i];
    }
}