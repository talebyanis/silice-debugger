#include <list>
#include <mutex>
#include <thread>
#include <LibSL.h>
#include <LibSL_gl.h>
#include <imgui.h>

#include "FSTReader.h"

void *g_Wave = nullptr;
std::map<fstHandle, valuesList> g_Values;
std::map<fstHandle, std::vector<int>> g_Errors;

std::mutex g_Mutex;

// ---------------------------------------------------------------------

int decodeValue(const char *str) {
    int val = 0;
    if (*str == 'x') { return -1; }
    while (*str != '\0') {
        val = (val << 1) | (*str == '1' ? 1 : 0);
        str++;
    }
    return val;
}

// ---------------------------------------------------------------------

void FSTReader::initMaps() {
    //Generate all scopes and their's signals
    Scope *currentScope = nullptr;
    fstHier *hier = fstReaderIterateHier(g_Wave);
    do {
        switch (hier->htyp) {
            case FST_HT_SCOPE:
                //std::cerr << "scope " << hier->u.scope.name << std::endl;
                currentScope = new Scope(*hier);
                this->scopes.push_back(currentScope);
                break;
            case FST_HT_ATTRBEGIN:
//                std::cerr << "FST HT ATTRBEGIN" << hier->u.attr.arg << std::endl;
                break;
            case FST_HT_VAR:
                if (currentScope == nullptr) {
                    std::cerr << "current scope null" << std::endl;
                } else {
                    //std::cerr << "  reader add " << hier->u.var.name << " " << "to " << currentScope->name << std::endl;
                    currentScope->add(*hier);
                }
                break;
            default:
//                std::cerr << "default " << hier->u.var.name << std::endl;
                break;
        }
        hier = fstReaderIterateHier(g_Wave);
    } while (hier != NULL);

    fstReaderSetFacProcessMaskAll(g_Wave);

    //Get all values in g_Values & g_Errors
    std::thread th([this]() {
        auto l = [](void *user_callback_data_pointer, uint64_t time, fstHandle facidx, const unsigned char *value) {
            std::unique_lock<std::mutex> lock(g_Mutex);
            int dvalue = decodeValue(reinterpret_cast<const char *>(value));
            if (dvalue != -1) {
                g_Values[facidx].push_back(std::make_pair((int) time, dvalue));
            } else {
                g_Errors[facidx].push_back((int) time);
            }
            std::this_thread::yield();
        };
        fstReaderIterBlocks(g_Wave, l, NULL, NULL);
    });

    th.join();

    //Find max time to add a point on the plots to the end
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

    //Fill values & errors
    for (auto item : g_Values) {
        fstHandle currentHandle = item.first;
        for (auto scope : this->scopes) {
            Signal *current = scope->getSignal(currentHandle);
            if (current)
                current->values = item.second;
        }
    }
    for (const auto item : g_Errors) {
        fstHandle currentHandle = item.first;
        for (Scope *scope : this->scopes) {
            Signal *current = scope->getSignal(currentHandle);
            if (current)
                current->errors = item.second;
        }
    }
}

// ---------------------------------------------------------------------

Signal *FSTReader::getSignal(fstHandle signal) {
    Signal *res;
    for (const auto &scope : scopes) {
        if ((res = scope->getSignal(signal))) break;
    }
    return res;
}

// ---------------------------------------------------------------------

valuesList FSTReader::getValues(fstHandle signal) {
    return this->getSignal(signal)->values;
}

// ---------------------------------------------------------------------

std::vector<int> FSTReader::getErrors(fstHandle signal) {
    return this->getSignal(signal)->errors;
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
    std::list<int> values;

    int a = -1;

    for (const auto &scope : scopes) {
        if (scope->name == "__main")
        {
            for (const auto &signal : scope->signals) {
                if(signal.second.name.find("_q_index") != std::string::npos) {
                    for (const auto &item : signal.second.values) {
                        values.emplace_back(item.second);
                    }
                    return values;
                }
            }
        }

    }

    return values;
}

// ---------------------------------------------------------------------

void clean() {
    g_Values.clear();
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