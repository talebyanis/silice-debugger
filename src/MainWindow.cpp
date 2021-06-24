#include "MainWindow.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <LibSL.h>
#include <LibSL_gl.h>
#include "imgui.h"
#include "imgui_internal.h"
#include "FileDialog.h"
#include "../libs/implot/implot.h"
#include "FST/FSTReader.h"
#include "sourcePath.h"
#include "FSTWindow.h"
#include <nlohmann/json.hpp>
#include <thread>
#include <mutex>

using json = nlohmann::json;

namespace fs = std::filesystem;

// Todo : set fileFullPath when doing "make debug" to show the file name in the editor

ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar
                                | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                                | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

bool p_open_dockspace = true;
bool p_open_editor = true;

static GLuint g_FontTexture;

static ImFont *font_general;
static ImFont *font_code; // font used for the TextEditor's code

std::map<std::string, bool> checked_algos;

bool show_fullpath = false;

//-------------------------------------------------------

static bool ImGui_Impl_CreateFontsTexture(const std::string &general_font_name, const std::string &code_font_name) {
    // Build texture atlas
    ImGuiIO &io = ::ImGui::GetIO();
    unsigned char *pixels;
    int width, height;
#if 0
    ImFontConfig font_cfg = ImFontConfig();
    font_cfg.SizePixels = font_size;
    io.Fonts->AddFontDefault(&font_cfg);
#else
    std::string font_path = std::string(SRC_PATH "/src/data/fonts/" + general_font_name);
    if (LibSL::System::File::exists(font_path.c_str())) {
        ImFontConfig cfg;
        cfg.OversampleH = 2;
        cfg.OversampleV = 2;
        cfg.PixelSnapH = true;
        font_general = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 18, &cfg,
                                                    io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    } else {
        std::cerr << Console::red << "General Font '" << font_path << "' not found" << std::endl;
    }
    font_path = std::string(SRC_PATH "/src/data/fonts/" + code_font_name);
    if (LibSL::System::File::exists(font_path.c_str())) {
        ImFontConfig cfg;
        cfg.OversampleH = 2;
        cfg.OversampleV = 2;
        cfg.PixelSnapH = true;
        font_code = io.Fonts->AddFontFromFileTTF(font_path.c_str(), 22, &cfg,
                                                 io.Fonts->GetGlyphRangesChineseSimplifiedCommon());
    } else {
        std::cerr << Console::red << "Code Font '" << font_path << "' not found" << std::endl;
    }


    io.Fonts->GetTexDataAsRGBA32(&pixels, &width,
                                 &height);   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    // Upload texture to graphics system
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void *) (intptr_t) g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);

    // Cleanup
    io.Fonts->ClearTexData();
    io.Fonts->ClearInputData();
#endif

    return true;
}

//-------------------------------------------------------

void MainWindow::ShowDockSpace()
{
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
// because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
// all active windows docked into it will lose their parent and become undocked.
// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar();
    ImGui::PopStyleVar(2);

// DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

        static auto first_time = true;
        if (first_time)
        {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
            ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

            // split the dockspace into 2 nodes -- DockBuilderSplitNode takes in the following args in the following order
            //   window ID to split, direction, fraction (between 0 and 1), the final two setting let's us choose which id we want (which ever one we DON'T set as NULL, will be returned by the function)
            //                                                              out_id_at_dir is the id of the node in the direction we specified earlier, out_id_at_opposite_dir is in the opposite direction
            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.5f, nullptr, &dockspace_id);

            // we now dock our windows into the docking node we made above
            ImGui::DockBuilderDockWindow("PlotWindow", dock_id_left);
            for (const auto &[filename, editor] : this->editors)
            {
                ImGui::DockBuilderDockWindow(fs::path(editor.first.file_path).filename().string().c_str(), dockspace_id);
            }
            ImGui::DockBuilderFinish(dockspace_id);
        }
    }

    bool error = false;

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open fst")) {
                auto fullpath = openFileDialog(OFD_FILTER_ALL);
                if (!fullpath.empty()) {
                    fstWindow.load(fullpath, this->editors, this->lp);
                    std::cout << "file " << fullpath << " opened" << std::endl;
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Load debug")) {
                if (exists(SRC_PATH "/.save/save.dat")) {
                    std::ifstream stream(SRC_PATH "/.save/save.dat");
                    json data;
                    stream >> data;
                    fstWindow.load(data, this->editors, this->lp);
                    std::cout << "debug opened with file " << data["filePath"] << std::endl;
                } else {
                    error = true;
                }
            }
            if (ImGui::MenuItem("Save debug", nullptr, false, fstWindow.g_Reader)) {
                if (fstWindow.g_Reader) {
                    if (!exists(SRC_PATH "/.save")) {
                        createDirectory(SRC_PATH "/.save");
                    }
                    std::ofstream save(SRC_PATH "/.save/save.dat");
                    json fstWindowJSON = fstWindow.save();
                    //std::cout << std::setw(4) << fstWindowJSON << std::endl;
                    save << std::setw(4) << fstWindowJSON << std::endl;
                }
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Quit", "Alt-F4")) {
                ImGui::End();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Docking")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            if (ImGui::MenuItem("NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            if (ImGui::MenuItem("NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            if (ImGui::MenuItem("NoDockingInCentralNode", "",
                                (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            if (ImGui::MenuItem("PassthruCentralNode", "",
                                (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            if (ImGui::MenuItem("AutoHideTabBar", "",
                                (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::End();
}

//-------------------------------------------------------

bool test_ptr = true;

void MainWindow::ShowCodeEditors(TextEditor& editor, std::list<std::string>& algo_list) {
    auto cpos = editor.GetCursorPosition();
    ImGui::Begin(fs::path(editor.file_path).filename().string().c_str(), nullptr, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_MenuBar);
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save", "Ctrl + S", nullptr, editor.file_path.empty())) {
                auto textToSave = editor.GetText();
                std::string path = editor.file_path;
                if (!path.empty()) {
                    std::fstream file(editor.file_path);
                    file << textToSave;
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            bool ro = editor.IsReadOnly();
            if (ImGui::MenuItem("Read-only mode", nullptr, &ro)) {
                editor.SetReadOnly(ro);
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo())) {
                editor.Undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo())) {
                editor.Redo();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection())) {
                editor.Copy();
            }
            if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection())) {
                editor.Cut();
            }
            if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection())) {
                editor.Delete();
            }
            if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr)) {
                editor.Paste();
            }

            ImGui::Separator();

            if (ImGui::MenuItem("Select all", nullptr, nullptr))
                editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Dark palette")) {
                editor.SetPalette(TextEditor::GetDarkPalette());
            }
            if (ImGui::MenuItem("Light palette")) {
                editor.SetPalette(TextEditor::GetLightPalette());
            }
            if (ImGui::MenuItem("Retro blue palette")) {
                editor.SetPalette(TextEditor::GetRetroBluePalette());
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Toggle index colorization", nullptr, editor.hasIndexColorization())) {
                editor.mIndexColorization = !editor.mIndexColorization;
            }
            if (ImGui::MenuItem("Show silice file's fullpath", nullptr, show_fullpath)) {
                show_fullpath = !show_fullpath;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    if (editor.hasIndexColorization()) {
        ImGui::Separator();
        for (const auto &item : algo_list)
        {
            ImGui::SameLine();
            if (ImGui::Checkbox(item.c_str(), &checked_algos[item.c_str()]))
            {
                fstWindow.setAlgoToColorize(checked_algos);
            }
        }
        ImGui::Separator();
    }

    if (show_fullpath)
    {
        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins",
                    editor.CanUndo() ? "*" : " ",
                    editor.GetLanguageDefinition().mName.c_str(), editor.file_path.c_str());
    }
    else
    {
        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins",
                    editor.CanUndo() ? "*" : " ",
                    editor.GetLanguageDefinition().mName.c_str(), fs::path(editor.file_path).filename().string().c_str());
    }


    ImGui::PushFont(font_code);
    p_open_editor = ImGui::IsWindowFocused();
    editor.Render("TextEditor");
    this->ZoomMouseWheel(editor);
    ImGui::PopFont();
    ImGui::End();
}

//-------------------------------------------------------

void MainWindow::ZoomMouseWheel(TextEditor& editor) {
    if (ImGui::GetIO().KeysDown[LIBSL_KEY_CTRL] && (p_open_editor || editor.p_open_editor)) {
        if (ImGui::GetIO().MouseWheel > 0 || ImGui::GetIO().KeysDown[LIBSL_KEY_UP]) {
            if (ImGui::GetFontSize() < 28) {
                editor.ScaleFont(true);
            }
        } else if (ImGui::GetIO().MouseWheel < 0 || ImGui::GetIO().KeysDown[LIBSL_KEY_DOWN]) {
            if (ImGui::GetFontSize() > 13) {
                editor.ScaleFont(false);
            }
        }
    }
}

//-------------------------------------------------------

void MainWindow::getSiliceFiles() {
    // Looks for every Silice files needed in the design
    std::ifstream file(PROJECT_DIR "build.v.files.log");

    static std::mutex mutex;
    std::vector<std::thread> thread_vector;
    std::string filename;

    if (file.is_open())
    {
        while (file.good())
        {
            filename = "";
            file >> filename;
            if (!filename.empty())
            {
                if (fs::is_regular_file(filename) && fs::path(filename).extension() == ".ice")
                {
                    thread_vector.emplace_back([this](std::string file_path) {
                        std::list<std::string> list = this->lp.getAlgos(file_path);
                        mutex.lock();
                        this->editors.insert(std::make_pair(file_path, std::make_pair(TextEditor(file_path, this->lp), list)));
                        mutex.unlock();
                    }, filename);
                }
            }
        }
        for (auto &thread : thread_vector)
        {
            thread.join();
        }
        file.close();
    }
}

//-------------------------------------------------------

void MainWindow::Init() {
    ImGui_Impl_CreateFontsTexture("NotoSans-Regular.ttf", "JetBrainsMono-Bold.ttf");

    const std::string str = PROJECT_DIR "icarus.fst";
    this->getSiliceFiles();
    fstWindow.load(str, this->editors, this->lp);

    // Initializing checkboxes
    for (const auto &[filepath, editor] : this->editors)
    {
        for (const auto &algoname : editor.second)
        {
            checked_algos[algoname] = true;
        }
    }

    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;

    ImVec4 *colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

void MainWindow::Render() {
    ImGui::PushFont(font_general);
    this->ShowDockSpace();
    for (auto &[filename, editor] : this->editors)
    {
        this->ShowCodeEditors(editor.first, editor.second);
    }
    fstWindow.render();

    ImGui::PopFont();
}
