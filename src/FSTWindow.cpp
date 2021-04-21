//
// Created by antoine on 16/04/2021.
//
#define IMGUI_ENABLE_MATH_OPERATOR

#include "FSTWindow.h"

#include <algorithm>
#include <list>
#include <iostream>
#include <sstream>
#include "imgui.h"
#include "../libs/implot/implot.h"
#include <bitset>
#include <iomanip>

FSTReader *g_Reader = nullptr;
std::vector<Plot> g_Plots = {};
ImPlotRange plotXLimits = ImPlotRange(-1, -1);
fstHandle hover = 0;
ConvertType convertType = DECIMALS;

//Shows the plots' names on the right to click on them and display matching plots
void FSTWindow::showPlotMenu() {
    //For every scope we have one TreeNode
    for (const auto &item : g_Reader->getScopes()) {
        if (ImGui::TreeNode(item.c_str())) {
            for (const auto &signal : g_Reader->getSignals(item)) {
                if (hover == signal)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1, 0.35, 0.10, 1));
                if (ImGui::MenuItem(g_Reader->getSignalName(signal).c_str(), "", FSTWindow::isDisplayed(signal))) {
                    if (!FSTWindow::isDisplayed(signal)) {
                        this->addPlot(signal);
                    } else {
                        this->removePlot(signal);
                    }
                }
                if (hover == signal)
                    ImGui::PopStyleColor();
            }
            ImGui::TreePop();
        }
    }
}

//-------------------------------------------------------

//Displays a plot
void FSTWindow::addPlot(fstHandle signal) {
    if (!g_Reader) {
        return;
    }
    Plot plot;
    plot.signalId = signal;
    std::string signalName = g_Reader->getSignalName(signal);
    plot.name = signalName;
    valuesList values = g_Reader->getValues(signal);
    for (const auto &item : values) {
        plot.x_data.push_back(item.first);
        plot.y_data.push_back(item.second);
    }

    g_Plots.push_back(plot);
}

//Hides a plot
void FSTWindow::removePlot(fstHandle signal) {
    g_Plots.erase(remove_if(
            g_Plots.begin(),
            g_Plots.end(),
            [signal](Plot plot) {
                return plot.signalId == signal;
            }), g_Plots.end());
}

bool FSTWindow::isDisplayed(fstHandle signal) {
    auto res = std::find_if(g_Plots.begin(), g_Plots.end(), [signal](Plot plot) {
        return plot.signalId == signal;
    });
    return res != g_Plots.end();
}

//Shows plots on the right of the window
void FSTWindow::showPlots() {
    ImVec2 wSize = ImGui::GetWindowSize();
    ImGui::BeginGroup();
    //reset hover to not change the color of a name if no plot is hovered
    hover = 0;
    for (int i = 0; i < g_Plots.size(); i++) {
        auto item = g_Plots[i];
        //set the plots Y limits to just below the lowest value to just upper the highest
        double max = *std::max_element(item.y_data.begin(), item.y_data.end());
        ImPlot::SetNextPlotLimitsY(0.0 - max / 10, max + max / 10);
        //plot X limits to be synchronized with others plots
        if (plotXLimits.Min == -1 && plotXLimits.Max == -1) {
            plotXLimits.Min = 0;
            plotXLimits.Max = g_Reader->getMaxTime();
        }
        ImPlot::LinkNextPlotLimits(&plotXLimits.Min, &plotXLimits.Max, nullptr, nullptr);
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(10, 0));
        ImGui::PushID(i);

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::Button("  ");
        ImGui::SetCursorScreenPos(cursor);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("PlotPayload", &i, sizeof(int));
            ImGui::Text("Dragging");
            ImGui::EndDragDropSource();
        }

        if (ImPlot::BeginPlot(item.name.c_str(), NULL, NULL, ImVec2(-1, 100),
                              ImPlotFlags_NoLegend | ImPlotFlags_NoChild, NULL,
                              ImPlotAxisFlags_Lock)) {
            ImPlot::PlotStairs(item.name.c_str(), (int *) &item.x_data[0], (int *) &item.y_data[0], item.x_data.size());
            ImPlotLimits limits = ImPlot::GetPlotLimits();
            //If the mouse is hover the plot, we take it's id to change the color of the right name on the left
            //and we set global X limits to this plot's
            if (ImPlot::IsPlotHovered()) {
                hover = item.signalId;
                plotXLimits.Min = limits.X.Min;
                plotXLimits.Max = limits.X.Max;
            }
            ImPlot::PushStyleColor(ImPlotCol_LegendText, ImVec4(0.15, 0.35, 0.15, 1));
            for (int i = 0; i < item.x_data.size(); i++) {
                std::basic_string<char> value;
                switch (convertType) {
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
                ImPlot::PlotText(value.c_str(), item.x_data[i], item.y_data[i]);
            }
            ImPlot::PopStyleColor();
            ImPlot::PopStyleVar();
            ImPlot::EndPlot();

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("PlotPayload")) {
                    int payload_i = *(const int *) payload->Data;
                    for (int k = payload_i; k > i; k--) {
                        std::swap(g_Plots[k],g_Plots[k-1]);
                    }
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::NewLine();
            ImGui::PopID();
        }
    }
    ImGui::EndGroup();
    }
}

void FSTWindow::render() {
    int treeWidth = 250;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(treeWidth + 500, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("PlotWindow", nullptr,
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_MenuBar);
    {
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Format")) {
                if (ImGui::MenuItem("Binary")) {
                    convertType = BINARY;
                }
                if (ImGui::MenuItem("Decimal")) {
                    convertType = DECIMALS;
                }
                if (ImGui::MenuItem("Hexadecimal")) {
                    convertType = HEXADECIMAL;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

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
}

FSTWindow::FSTWindow(std::string file) {
    g_Reader = new FSTReader(file.c_str());
}