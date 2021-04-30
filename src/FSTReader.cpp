#include <list>
#include <mutex>
#include <thread>
#include <LibSL.h>
#include <LibSL_gl.h>
#include <imgui.h>

#include "FSTReader.h"

void *g_Wave = nullptr;
std::map<std::string, std::list<fstHandle>> g_ScopeToSignals;
std::map<fstHandle, std::string> g_HandleToName;
std::map<fstHandle, valuesList> g_Values;
std::map<fstHandle, std::vector<int>> g_Errors;
std::list<int> _q_index_values;

std::mutex g_Mutex;

// ---------------------------------------------------------------------

int decodeValue(const char *str) {
    int val = 0;
    if(*str == 'x') { return -1; }
    while (*str != '\0') {
        val = (val << 1) | (*str == '1' ? 1 : 0);
        str++;
    }
    return val;
}

// ---------------------------------------------------------------------

void FSTReader::initMaps() {
    std::string currentScope;
    std::list<fstHandle> currentSignals = {};

    struct fstHier *hier = fstReaderIterateHier(g_Wave);
    do {
        switch (hier->htyp) {
            case FST_HT_SCOPE:
                if (currentScope.compare(hier->u.var.name) != 0) {
                    currentSignals.sort();
                    if (!currentScope.empty()) g_ScopeToSignals.insert(std::make_pair(currentScope, currentSignals));
                    currentSignals = {};
                    currentScope = hier->u.var.name;
                }
                break;
            case FST_HT_ATTRBEGIN:
                break;
            case FST_HT_VAR:
                g_HandleToName.insert(std::make_pair(hier->u.var.handle, hier->u.var.name));
                currentSignals.push_back(hier->u.var.handle);
                break;
            default:
                break;
        }
        hier = fstReaderIterateHier(g_Wave);
    } while (hier != NULL);

    fstReaderSetFacProcessMaskAll(g_Wave);

    std::thread th([this]() {
        auto l = [](void *user_callback_data_pointer, uint64_t time, fstHandle facidx, const unsigned char *value) {
            std::unique_lock<std::mutex> lock(g_Mutex);
            int dvalue = decodeValue(reinterpret_cast<const char *>(value));
            if(dvalue != -1) {
                g_Values[facidx].push_back(std::make_pair((int) time, dvalue));
                if (g_HandleToName[facidx].find("_q_index") != std::string::npos)
                {
                    if ((std::find(_q_index_values.begin(), _q_index_values.end(), dvalue) == _q_index_values.end()))
                    {
                        _q_index_values.push_back(dvalue);
                    }
                }
            } else {
                g_Errors[facidx].push_back((int) time);
            }
            std::this_thread::yield();
        };
        fstReaderIterBlocks(g_Wave, l, NULL, NULL);
    });

    th.join();

    ImU64 maxTime = getMaxTime();
    for (auto &item : g_Values) {
        valuesList *values = &item.second;
        auto res = std::find_if(values->begin(), values->end(), [maxTime](std::pair<ImU64, ImU64> pair) {
            return pair.first == maxTime;
        });
        if (res == values->end()) {
            ImU64 lastValue = 0;
            ImU64 lastTime = 0;
            for (const auto &value : *values) {
                lastTime = value.first;
                lastValue = value.second;
            }
            values->push_back(std::make_pair(maxTime, lastValue));
        }
    }
}

// ---------------------------------------------------------------------

valuesList FSTReader::getValues(fstHandle signal) {
    return g_Values[signal];
}

// ---------------------------------------------------------------------

std::vector<int> FSTReader::getErrors(fstHandle signal) {
    return g_Errors[signal];
}

// ---------------------------------------------------------------------

std::string FSTReader::getSignalName(fstHandle signal) {
    return g_HandleToName[signal];
}

// ---------------------------------------------------------------------

std::list<std::string> FSTReader::getScopes() {
    std::list<std::string> scopes = {};
    for (const auto &item : g_ScopeToSignals) {
        scopes.push_back(item.first);
    }
    return scopes;
}

// ---------------------------------------------------------------------

std::list<fstHandle> FSTReader::getSignals(std::string scope) {
    return g_ScopeToSignals[scope];
}

// ---------------------------------------------------------------------

ImU64 FSTReader::getMaxTime() {
    ImU64 max = 0;
    for (const auto &item : g_Values) {
        valuesList values = item.second;
        for (const auto &value : values) {
            if (value.first > max) {
                max = value.first;
            }
        }
    }
    return max;
}

// ---------------------------------------------------------------------

std::list<int> FSTReader::get_q_index_values()
{
    return _q_index_values;
}

// ---------------------------------------------------------------------

void clean() {
    g_Values.clear();
    g_ScopeToSignals.clear();
    g_HandleToName.clear();
}

// ---------------------------------------------------------------------

FSTReader::FSTReader(const char *file) {
    clean();
    if (!LibSL::System::File::exists(file)) {
        std::cerr << "Could not open fst wave file " << file << std::endl;
    }
    g_Wave = fstReaderOpen(file);
    initMaps();
}