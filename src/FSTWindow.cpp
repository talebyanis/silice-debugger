//
// Created by antoine on 16/04/2021.
//
#define IMGUI_ENABLE_MATH_OPERATOR

#include <algorithm>
#include <list>
#include <iostream>
#include "imgui.h"
#include "../libs/implot/implot.h"

#include "FSTWindow.h"

std::vector<Plot> g_Plots = {};
FSTReader *g_Reader = nullptr;

void FSTWindow::showPlotMenu() {
    for (const auto &item : g_Reader->getScopes()) {
        if (ImGui::TreeNode(item.c_str())) {
            for (const auto &item : g_Reader->getSignals(item)) {
                if (ImGui::MenuItem(g_Reader->getSignalName(item).c_str(), "", FSTWindow::isDisplayed(item))) {
                    if (!FSTWindow::isDisplayed(item)) {
                        this->addPlot(item);
                    } else {
                        this->removePlot(item);
                    }
                }
            }
            ImGui::TreePop();
        }
    }
}

//-------------------------------------------------------

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

void FSTWindow::showPlots() {
    ImVec2 wPos = ImGui::GetCursorScreenPos();
    ImVec2 wSize = ImGui::GetWindowSize();
    ImGui::BeginGroup();
    for (const auto &item : g_Plots) {
        ImGui::BeginChild(item.name.c_str(), ImVec2(wSize.x - 20, 100));
        double max = *std::max_element(item.y_data.begin(), item.y_data.end());
        ImPlot::SetNextPlotLimitsY(0.0 - max / 10, max + max / 10);
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(10, 0));
        if (ImPlot::BeginPlot(item.name.c_str(),NULL,NULL,ImVec2(-1,100),ImPlotFlags_NoLegend)) {
            ImPlot::PlotStairs(item.name.c_str(), (int *) &item.x_data[0], (int *) &item.y_data[0], item.x_data.size());
            ImPlot::EndPlot();
        }
        ImPlot::PopStyleVar();
        ImGui::EndChild();
        ImGui::NewLine();
    }
    ImGui::EndGroup();
}

void FSTWindow::render() {
    int treeWidth = 250;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(treeWidth + 500, 500),ImGuiCond_FirstUseEver);
    ImGui::Begin("PlotWindow", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
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
}

FSTWindow::FSTWindow(std::string file) {
    g_Reader = new FSTReader(file.c_str());
}