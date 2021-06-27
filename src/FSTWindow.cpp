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

inline void FSTWindow::showSignalsMenu(Scope &scope, int &hiddenCount, bool internal) {
    auto show = [this, &hiddenCount](std::unordered_map<fstHandle, Signal> &signals) {
        bool color = false;
        for (const auto &signal : signals) {
            std::string name = signal.second.name;
            std::vector<fstHandle> sig = {signal.second.id};
            if (name.find(m_FilterBuffer) != std::string::npos) {
                color = false;
                if (m_HoverHighLight == signal.second.id) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4, 0.4, 0.9, 1));
                    color = true;
                } else if(signal.second.type == "output") {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0,0,1));
                    color = true;
                } else if(signal.second.type == "input") {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0,1,0,1));
                    color = true;
                }
                if (ImGui::MenuItem((name.size() > 25 ? (name.substr(0, 25) + "...").c_str() : name.c_str()),
                                    "",
                                    FSTWindow::isDisplayed(sig))) {
                    if (!FSTWindow::isDisplayed(sig)) {
                        this->addPlot(sig);
                    } else {
                        this->removePlot(sig);
                    }
                }
                if (ImGui::IsItemHovered()) {
                    m_HoverRightClickMenu = signal.second.id;
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", name.c_str());
                    ImGui::EndTooltip();
                }
                if (color) {
                    ImGui::PopStyleColor();
                }
            } else {
                hiddenCount++;
            }
        }
    };
    show(internal ? scope.signalsInternal : scope.signalsUser);
}

inline void FSTWindow::showPairsMenu(Scope &scope, int &hiddenCount, bool internal) {
    auto show = [this, &hiddenCount](std::unordered_map<std::string, DQPair*> &pairs) {
        bool color = false;
        for (const auto &signal : pairs) {
            std::string name = signal.second->name;
            std::vector<fstHandle> pair = {signal.second->d->id, signal.second->q->id};
            if (name.find(m_FilterBuffer) != std::string::npos) {
                color = false;
                if (m_HoverHighLight == signal.second->q->id || m_HoverHighLight == signal.second->d->id) {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4, 0.4, 0.9, 1));
                    color = true;
                } else if(signal.second->type == "output") {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,0,0,1));
                    color = true;
                } else if(signal.second->type == "input") {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0,1,0,1));
                    color = true;
                }
                std::vector<fstHandle> vectorA = std::vector<fstHandle>({pair[0]});
                std::vector<fstHandle> vectorB = std::vector<fstHandle>({pair[1]});
                if (ImGui::MenuItem((name.size() > 25 ? (name.substr(0, 25) + "...").c_str() : name.c_str()),
                                    "",
                                    FSTWindow::isDisplayed(vectorA) ||
                                    FSTWindow::isDisplayed(vectorB))) {
                    if (!FSTWindow::isDisplayed(pair)) {
                        this->addPlot(pair);
                    } else {
                        this->removePlot(pair);
                    }
                }
                if (ImGui::IsItemHovered()) {
                    m_HoverRightClickMenu = signal.second->d->id;
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", name.c_str());
                    ImGui::EndTooltip();
                }
                if (color) {
                    ImGui::PopStyleColor();
                }
            } else {
                hiddenCount++;
            }
        }
    };
    show(internal ? scope.pairsInternal : scope.pairsUser);
}

void FSTWindow::showScope(Scope &scope) {
    std::string name = scope.name;
    //count for hidden signals
    int hiddenCount = 0;

    bool open = ImGui::TreeNode(name.c_str());
    if (ImGui::BeginPopupContextItem(name.c_str())) {
        ImGui::Text("%s", name.c_str());
        ImGui::ColorPicker4("Color Picker", (float *) &m_ScopeColors.at(name));
        ImGui::EndPopup();
    }
    if(!scope.d_name.empty()) {
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s", scope.d_name.c_str());
            ImGui::EndTooltip();
        }
    }
    if (open) {
        for(size_t i=0; i<scope.children.size(); i++) {
            this->showScope(*scope.children[i]);
        }

        ImGui::Separator();

        showSignalsMenu(scope, hiddenCount, false);
        if (!scope.pairsUser.empty()) { ImGui::Separator(); }
        showPairsMenu(scope, hiddenCount, false);


        if(ImGui::TreeNode("Internal")) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5,0.5,0.5,1));
            showSignalsMenu(scope, hiddenCount, true);
            if (!scope.pairsInternal.empty()) { ImGui::Separator(); }
            showPairsMenu(scope, hiddenCount, true);
            ImGui::TreePop();
            ImGui::PopStyleColor();
        }

        if (hiddenCount != 0) {
            ImGui::MenuItem(("Hidden items : " + std::to_string(hiddenCount)).c_str(), NULL, false, false);
        }

        ImGui::TreePop();
    }
}

//Shows the plots' names on the left to click on them and display matching plots
void FSTWindow::showPlotMenu() {
    if (m_Reader != nullptr) {
        ImGui::InputText("  Filter", m_FilterBuffer, sizeof(m_FilterBuffer));
        //For every scope we have one TreeNode
        for(size_t i=0; i<m_Reader->scopes.size(); i++) {
            this->showScope(*m_Reader->scopes[i]);
        }
    }
}

//-------------------------------------------------------

void FSTWindow::showRightClickPlotSettings(fstHandle signal) {
    Plot *plot = nullptr;
    for (Plot &item : m_Plots) {
        if (item.signalId == signal) {
            plot = &item;
        }
    }
    if (plot) {
        if (ImGui::BeginPopupContextWindow()) {
            ImGui::Text("   Value Format");
            ImGui::Separator();
            if (ImGui::MenuItem("Binary")) {
                plot->type = BINARY;
            }
            if (ImGui::MenuItem("Decimal")) {
                plot->type = DECIMALS;
            }
            if (ImGui::MenuItem("Hexadecimal")) {
                plot->type = HEXADECIMAL;
            }
            if (ImGui::InputText("  Custom", m_CustomFilterBuffer, sizeof(m_CustomFilterBuffer))) {
                plot->customtype_string = m_CustomFilterBuffer;
                plot->type = CUSTOM;
            }
            (this->m_BitLeftCustom == -1)
            ? ImGui::Text("Bad custom expression")
            : ImGui::Text("Bit left for custom printing : %d", this->m_BitLeftCustom);
            ImGui::EndPopup();
        }
    }
}

//-------------------------------------------------------

//Adds a plot to the list
void FSTWindow::addPlot(const std::vector<fstHandle>& signals) {
    if (!m_Reader) {
        return;
    }
    for (const auto &signal : signals) {
        std::vector<fstHandle> vector = std::vector<fstHandle>({signal});
        if (!this->isDisplayed(vector)) {
            Plot plot;
            plot.signalId = signal;
            std::string signalName = m_Reader->getSignal(signal)->name;
            plot.name = signalName;
            plot.type = HEXADECIMAL;
            if (signalName[1] == 'd') {
                if (signalName.find("index") == std::string::npos) {
                    plot.fold = false;
                } else {
                    plot.fold = true;
                }
            } else if (signalName[1] == 'q') {
                if (signalName.find("index") == std::string::npos) {
                    plot.fold = true;
                } else {
                    plot.fold = false;
                }
            } else {
                plot.fold = false;
            }
            plot.color = ImVec4(1, 1, 1, 1);
            valuesList values = m_Reader->getValues(signal);

            plot.maxY = 0;

            for (const auto &item : values) {
                plot.x_data.push_back(item[0]);
                plot.y_data.push_back(item[1]);
                if (item[1] > plot.maxY) plot.maxY = item[1];
            }

            m_Plots.push_back(plot);
        }
    }
}

//-------------------------------------------------------

//Removes a plot from the list
void FSTWindow::removePlot(std::vector<fstHandle> signals) {
    for (const auto &signal : signals) {
      auto pos = std::find_if(
        m_Plots.begin(),
        m_Plots.end(),
        [signal](Plot plot) {
          return plot.signalId == signal;
        });
      if (pos != m_Plots.end()) {
        m_Plots.erase(pos);
      }
    }
}

//-------------------------------------------------------

bool FSTWindow::isDisplayed(std::vector<fstHandle> &signals) {
    for (const auto &signal : signals) {
        for (size_t i = 0; i < m_Plots.size(); i++) {
            if (m_Plots[i].signalId == signal) return true;
        }
    }
    return false;
}

//-------------------------------------------------------

// Function to convert binary to decimal
int FSTWindow::binaryToDecimal(std::string n) {
    std::string num = n;
    int dec_value = 0;

    int base = 1;

    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }

    return dec_value;
}

//-------------------------------------------------------

std::pair<std::string, int> FSTWindow::parseCustomExp(const std::string &expression, int value) {
    std::pair<std::string, int> res = std::pair("", 16);

    // Converting value to Binary
    std::string binaryVal;
    binaryVal = std::bitset<16>(value).to_string();

    char current = '0';
    std::string number;
    int numberInt = 0;
    std::stringstream stream;
    std::string buffer;

    //ToDo : regex

    // Parsing the expression and generating res
    for (char c: expression) {
        switch (c) {
            case 'b': // binary
            case 'd': // signed decimal
            case 'u': // unsigned decimal
            case 'x': // hex
                if (current != '0') return std::pair("", -1);
                current = c;
                break;
            case ';':
                if (current == '0' || number.empty()) return std::pair("", -1);
                numberInt = stoi(number);
                if (numberInt > binaryVal.length()) return std::pair("", -1);

                res.second -= numberInt;
                if (res.second < 0) // no bit left to print (max 16)
                {
                    return std::pair("", -1);
                }

                for (int i = 0; i < numberInt; i++) {
                    buffer += binaryVal[0]; // putting the first bit in the buffer
                    binaryVal.erase(binaryVal.begin()); // removing the first bit
                }

                switch (current) {
                    case 'b':
                        if (buffer.empty()) buffer = "0";
                        res.first += "b(" + buffer + ")";
                        break;
                    case 'u':
                        res.first += "u(" + std::to_string(this->binaryToDecimal(buffer)) + ")";
                        break;
                    case 'd':
                        if (buffer[0] == '1') {
                            buffer.erase(buffer.begin()); // removing the first bit
                            res.first += "d(-" + std::to_string(this->binaryToDecimal(buffer)) + ")";
                            break;
                        }
                        res.first += "d(" + std::to_string(this->binaryToDecimal(buffer)) + ")";
                        break;
                    case 'x':
                        stream << std::hex << this->binaryToDecimal(buffer);
                        res.first += "x(" + stream.str() + ")";
                        break;
                    default:
                        break;
                }
                current = '0';
                number = "";
                break;
            default:  // number
                if (current == '0') return std::pair("", -1);
                if (isdigit(c)) //checking if the char is a digit value
                {
                    number += c;
                } else {
                    return std::pair("", -1);
                }
                break;
        }
    }
    return res;
}

//-------------------------------------------------------

//Shows plots on the right of the window
void FSTWindow::showPlots() 
{
  bool do_reset_limits = m_ResetPlotLimits;
  m_ResetPlotLimits = false;
  //reset hoverHighLight to not change the color of a name if no plot is hovered
  m_HoverHighLight = 0;
    for (int i = 0; i < m_Plots.size(); i++) {
        Plot *item = &m_Plots[i];    

        ImGui::PushID(i);
        ImGui::BeginGroup();

        ImVec2 cursor = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(cursor.x, cursor.y),
                                                  ImVec2(cursor.x + 3000, cursor.y + 25),
                                                  IM_COL32(51, 64, 74, 255));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::Button(item->name.c_str());
        ImGui::SameLine();
        ImGui::SetCursorScreenPos(
                ImVec2(cursor.x + ImGui::GetWindowSize().x - 50, cursor.y));

        //Folding button
        if (item->fold) {
            if (ImGui::Button("^")) {
                item->fold = !item->fold;
            }
        } else {
            if (ImGui::Button("v")) {
                item->fold = !item->fold;
            }
        }


        ImGui::SameLine();
        ImGui::PopStyleVar(2);

        //Button to close plot
        if (ImGui::Button("x")) {
            this->removePlot(std::vector<fstHandle>({item->signalId}));
            continue;
        }
        
        if (!item->fold) {
            ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(10, 0));
            //Coloring the line
            ImPlot::PushStyleColor(ImPlotCol_Line, m_ScopeColors.at(m_Reader->getSignal(item->signalId)->scopeName));
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);

            //set the plots Y limits to just below the lowest value to just upper the highest
            if (item->maxY == 0) {
                ImPlot::SetNextPlotLimitsY(-0.5,0.5);
            } else {
                ImPlot::SetNextPlotLimitsY(0.0 - (double) item->maxY / 3.0,
                  (double)item->maxY + (double)item->maxY / 3.0);
            }

            //Cloning in other values to prevent LinkNextPlot from modifying values (SetNextPlotLimitsX not working idk why)
            double xMin = m_PlotXLimits->Min;
            double xMax = m_PlotXLimits->Max;
            if (do_reset_limits) {
              ImPlot::SetNextPlotLimitsX(m_PlotXLimits->Min, m_PlotXLimits->Max, ImGuiCond_Always);
            } else {
              ImPlot::LinkNextPlotLimits(&xMin, &xMax, nullptr, nullptr);
            }

            size_t leftIndex = 0;
            size_t midIndex;
            size_t rightIndex = item->x_data.size() - 1;

            if(item->x_data.size() > 2 ) {
                auto dicho = [item](size_t leftIndex, size_t rightIndex, ImS64 toFind) {
                    if (toFind < 0) { return (size_t)0; }
                    size_t midIndex = rightIndex;
                    while (leftIndex < rightIndex - 1) {
                        midIndex = (leftIndex + rightIndex) / 2;
                        if (item->x_data[midIndex] <= toFind) {
                          leftIndex = midIndex;
                        } else if (item->x_data[midIndex] > toFind) {
                          rightIndex = midIndex;
                        }
                    }
                    return midIndex;
                };
                
                leftIndex = dicho(leftIndex, rightIndex, xMin);
                rightIndex = dicho(leftIndex, rightIndex, xMax);

                if (leftIndex > 0) --leftIndex;
                if (rightIndex < item->x_data.size() - 1) ++rightIndex;
            }

            if (ImPlot::BeginPlot(item->name.c_str(), NULL, NULL, ImVec2(-1, 100),
                                  ImPlotFlags_NoLegend | ImPlotFlags_NoChild | ImPlotFlags_NoMousePos |
                                  ImPlotFlags_NoMenus | ImPlotFlags_NoTitle,
                                  0,
                                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoTickLabels)) {

                //Marker
                ImPlot::DragLineX("Marker", &m_MarkerX, true, ImVec4(1, 0.5, 0.5, 1), 1);

                if (!item->x_data.empty()) {
                  //If there is too much data to display (more samples than pixels)
                  //we remove some data that won't be display anyway
                  size_t pixels = ImPlot::GetPlotSize().x;
                  size_t dataSize = rightIndex - leftIndex + 1;
                  if (dataSize > pixels) {
                    std::vector<int> x_data;
                    std::vector<int> y_data;
                    for (size_t i = 0; i < pixels; i++) {
                      //there is (x = dataSize/pixels) times more data than pixels
                      //so we take one sample out of x 
                      size_t index = leftIndex + i * (dataSize / pixels);
                      x_data.push_back(item->x_data[index]);
                      y_data.push_back(item->y_data[index]);
                    }
                    ImPlot::PlotStairs(item->name.c_str(), &x_data[0], &y_data[0],
                      pixels);
                    this->drawValues(item, leftIndex, rightIndex, (dataSize / pixels) * 15);
                  } else {
                    ImPlot::PlotStairs(item->name.c_str(), &item->x_data[leftIndex], &item->y_data[leftIndex],
                      dataSize);
                    this->drawValues(item, leftIndex, rightIndex, 1);
                  }

                  this->drawUnknowns(item);
                }

                if (ImPlot::IsPlotHovered()) {

                    //If the mouse is hover the plot, we take it's id to change the color of the right name on the left
                    m_HoverHighLight = item->signalId;
                    m_HoverRightClickMenu = item->signalId;

                    //modifies the range for all plots to be sync
                    ImPlotLimits limits = ImPlot::GetPlotLimits();
                    m_PlotXLimits->Min = limits.X.Min;
                    m_PlotXLimits->Max = limits.X.Max;

                    //Arrows to move to values change
                    bool rl = this->listenArrows(item);
                    m_ResetPlotLimits |= rl;

                    //Double click to move marker
                    if (ImGui::IsMouseDoubleClicked(0)) {
                        m_MarkerX = ImPlot::GetPlotMousePos().x;
                    }

                }
                ImPlot::PopStyleColor(1);
                ImPlot::PopStyleVar();
                ImPlot::EndPlot();

            }
        }
        ImGui::EndGroup();

       //Drag&drop source
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
            ImGui::SetDragDropPayload("PlotPayload", &i, sizeof(fstHandle));
            ImGui::Text("%s", item->name.c_str());
            ImGui::EndDragDropSource();
        }
        ImGui::PopID();
        //Drag&drop target
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("PlotPayload")) {
                int data = *(const int*) payload->Data;
                if (data > i) {
                    for (int k = data; k > i; k--) {
                        std::swap(m_Plots[k], m_Plots[k - 1]);
                    }
                } else if (i > data) {
                    for (int k = data; k < i; k++) {
                        std::swap(m_Plots[k], m_Plots[k + 1]);
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }
    }
    this->showRightClickPlotSettings(m_HoverRightClickMenu);
}

inline void FSTWindow::drawUnknowns(Plot *item) {
    auto unknowns = m_Reader->getUnkowns(item->signalId);
    for (const auto &ukn : unknowns) {
        int next = -1;
        for (const auto &x : item->x_data) {
            if (ukn < x) {
                next = x;
                break;
            }
        }
        ImPlot::PushPlotClipRect();
        ImVec2 min = ImPlot::PlotToPixels(ImPlotPoint(ukn, 0.0 - item->maxY));
        ImVec2 max = ImPlot::PlotToPixels(ImPlotPoint(next, 2.0 * item->maxY));
        ImPlot::GetPlotDrawList()->AddRectFilled(min, max, IM_COL32(255, 0, 0, 100));
        ImPlot::PopPlotClipRect();
    }
}

inline bool FSTWindow::listenArrows(Plot* item) {
    ImGui::SetKeyboardFocusHere();
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow), true)) { // TODO: std::binary_search
      for (ImU64 x : item->x_data) {
            if (m_MarkerX < x) {
                m_MarkerX = x;
                if (m_MarkerX > m_PlotXLimits->Max || m_MarkerX < m_PlotXLimits->Min) {
                    double distance = m_PlotXLimits->Max - m_PlotXLimits->Min;
                    m_PlotXLimits->Min = m_MarkerX - (distance / 2.0);
                    m_PlotXLimits->Max = m_MarkerX + (distance / 2.0);
                }
                return true;
            }
        }
    }
    if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow), true)) {
        for (int ii = item->x_data.size() - 1; ii >= 0; ii--) { // TODO: std::binary_search
            ImU64 x = item->x_data[ii];
            if (m_MarkerX > x) {
                m_MarkerX = x;
                if (m_MarkerX > m_PlotXLimits->Max || m_MarkerX < m_PlotXLimits->Min) {
                    double distance = m_PlotXLimits->Max - m_PlotXLimits->Min;
                    m_PlotXLimits->Min = m_MarkerX - (distance / 2.0);
                    m_PlotXLimits->Max = m_MarkerX + (distance / 2.0);
                }
                return true;
            }
        }
    }
    if(ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow),true)) {
        double min = m_PlotXLimits->Min;
        double max = m_PlotXLimits->Max;
        m_PlotXLimits->Min = m_MarkerX - (max - min) / 2.0;
        m_PlotXLimits->Max = m_MarkerX + (max - min) / 2.0;
        return true;
    }
    return false;
}

inline void FSTWindow::drawValues(Plot *item, size_t leftIndex, size_t rightIndex, size_t ratio) {
    for (size_t i = leftIndex; i < rightIndex; i+=ratio) {
        std::stringstream stream;
        std::basic_string<char> value;
        switch (item->type) {
            case BINARY:
                value = std::bitset<16>(item->y_data[i]).to_string();
                break;
            case DECIMALS:
                value = 'd' + std::to_string(item->y_data[i]);
                break;
            case HEXADECIMAL:
                stream << 'h' << std::hex << item->y_data[i];
                value = stream.str();
                break;
            case CUSTOM:
                std::pair<std::string, int> pair = parseCustomExp(item->customtype_string, item->y_data[i]);
                value = pair.first;
                this->m_BitLeftCustom = pair.second;
                if (value.empty()) {
                    value = std::to_string(item->y_data[i]); // Using Decimal if the expression is bad
                }
                break;
        }

        //ImPlot::PushStyleColor(ImPlotCol_InlayText, ImVec4(1, 1, 1, 1));
        //Offset to place the value correctly
        ImVec2 offset = ImVec2(0, 0);
        if (i > 0) {
            if (item->y_data[i] > item->y_data[i - 1]) {
                offset.y = -6;
            } else {
                offset.y = 6;
            }
        }
        
        ImPlot::PlotText(value.c_str(), item->x_data[i], item->y_data[i], false, offset);

       //ImGui::PopStyleColor();
    }
}

//-------------------------------------------------------

void FSTWindow::render() {
    int treeWidth = 250;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(treeWidth + 500, 500), ImGuiCond_FirstUseEver);
    ImGui::Begin("PlotWindow", nullptr,
                 ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    {
        ImVec2 wPos = ImGui::GetCursorScreenPos();
        ImVec2 wSize = ImGui::GetWindowSize();
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y));
        ImGui::BeginChild("Tree", ImVec2(treeWidth, wSize.y - 25), true,
                          ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            this->showPlotMenu();
        }
        ImGui::EndChild();

        ImGui::SetNextWindowPos(ImVec2(wPos.x + treeWidth, wPos.y));
        ImGui::BeginChild("Graph", ImVec2(wSize.x - treeWidth, wSize.y - 25), true,
                          ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysVerticalScrollbar);
        {
            this->showPlots();
        }
        ImGui::End();
    }
    ImGui::End();
    ImGui::PopStyleVar(); // Padding

    std::list<std::pair<std::string, int>> indexes;

    for (const auto &[algoname, list_val] : this->m_qindexValues)
    {
        int tmp = 0;
        for (int ii = 1; ii < list_val.size(); ii++) {
            if (m_MarkerX < list_val[ii].first && m_MarkerX >= list_val[tmp].first) {
                indexes.emplace_back(algoname, list_val[tmp].second);
                break;
            }
            tmp = ii;
        }
    }

    if (this->m_Editors->empty()) return;

    for (auto &editor : *this->m_Editors)
    {
        (indexes.empty()) ? editor.second.first.unsetSelectedIndex()
                          : editor.second.first.setSelectedIndex(indexes);
    }

}

//-------------------------------------------------------

json FSTWindow::save() {
    json j;
    j["filePath"] = this->m_FstFilePath;
    j["rangeMin"] = m_Range.Min;
    j["rangeMax"] = m_Range.Max;
    j["markerX"] = m_MarkerX;
    std::vector<fstHandle> displayedPlots;
    std::vector<ConvertType> displayedTypes;
    for (const auto &item : m_Plots) {
        displayedPlots.push_back(item.signalId);
        displayedTypes.push_back(item.type);
    }
    j["displayedSignals"] = displayedPlots;
    j["displayedTypes"] = displayedTypes;
    for (const auto &item : m_ScopeColors) {
        ImVec4 col = item.second;
        j["color"][item.first] = {col.x, col.y, col.z, col.w};
    }
    return j;
}

//-------------------------------------------------------

void FSTWindow::clean() {
    m_Plots.clear();
    m_qindexValues.clear();
    m_Reader = nullptr;
}

//-------------------------------------------------------

void FSTWindow::loadQindex(Scope &scope) {
        for (const auto &algoname : this->m_AlgosToColorize)
        {
            if (m_qindexValues_save[algoname].empty())
            {
                if (scope.name.find(algoname) != std::string::npos) {
                    for (const auto &pair : scope.pairsInternal) {
                        if (pair.second->name.find("index") != std::string::npos) {
                            for (const auto &value : m_Reader->getValues(pair.second->q->id))
                            {
                                m_qindexValues_save[algoname].emplace_back(value[0], value[1]);
                                m_qindexValues[algoname].emplace_back(value[0], value[1]);
                            }
                            break;
                        }
                    }
                } else {
                    for(auto & i : scope.children) {
                        loadQindex(*i);
                    }
                }
            }
            else
            {
                m_qindexValues[algoname] = m_qindexValues_save[algoname];
            }
        }
}

//-------------------------------------------------------

void FSTWindow::setAlgoToColorize(std::map<std::string, bool>& algos) {
    this->m_AlgosToColorize.clear();
    for (const auto &[algoname, must_be_colored] : algos) {
        if (must_be_colored) {
            this->m_AlgosToColorize.push_back(algoname);
        }
    }
    this->m_qindexValues.clear();

    for (auto & scope : m_Reader->scopes) {
        loadQindex(*scope);
    }
}

//-------------------------------------------------------

void loadColors(Scope *scope, std::map<std::string, ImVec4> &scopeColors) {
    scopeColors.insert({scope->name, ImVec4(1, 1, 1, 1)});
    for (const auto &item : scope->children) {
        loadColors(item,scopeColors);
    }
}

void FSTWindow::load(const std::string& file, std::map<std::string, std::pair<TextEditor, std::list<std::string>>>& editors, LogParser& logParser) {
    this->clean();
    this->m_FstFilePath = file;
    m_Reader = new FSTReader(file.c_str(), logParser);
    if (m_PlotXLimits == nullptr) {
        m_PlotXLimits = &m_Range;
        m_PlotXLimits->Min = 0;
        m_PlotXLimits->Max = 5000;
    }
    this->m_Editors = &editors;

    for (size_t i = 0; i < m_Reader->scopes.size(); ++i) {
        loadQindex(*m_Reader->scopes[i]);
    }

    for (const auto &item : m_Reader->scopes) {
        loadColors(item, this->m_ScopeColors);
    }
}

//-------------------------------------------------------

void FSTWindow::load(json data, std::map<std::string, std::pair<TextEditor, std::list<std::string>>>& editors, LogParser& logParser) {
    this->clean();
    this->m_FstFilePath = data["filePath"];
    m_Reader = new FSTReader(this->m_FstFilePath.c_str(), logParser);
    m_Range = ImPlotRange(data["rangeMin"], data["rangeMax"]);
    m_PlotXLimits = &m_Range;
    this->m_Editors = &editors;
    m_MarkerX = data["markerX"];
    for (int i = 0; i < data["displayedSignals"].size(); ++i) {
        fstHandle signal = data["displayedSignals"][i];
        std::vector<fstHandle> vec = std::vector<fstHandle>({signal});
        this->addPlot(vec);
        m_Plots[i].type = data["displayedTypes"][i];
    }

    for (auto & i : m_Reader->scopes) {
        auto vals = data["color"][i->name];
        m_ScopeColors.insert({i->name, ImVec4(vals[0], vals[1], vals[2], vals[3])});
    }

    for (size_t i = 0; i < m_Reader->scopes.size(); ++i) {
        loadQindex(*m_Reader->scopes[i]);
    }
}