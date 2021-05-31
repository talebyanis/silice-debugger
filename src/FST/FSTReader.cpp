#include <list>
#include <mutex>
#include <thread>
#include <LibSL.h>
#include <LibSL_gl.h>
#include <imgui.h>

#include "FSTReader.h"

void *g_Wave = nullptr;
std::vector<valuesList> g_Values;
std::vector<std::vector<int>> g_Errors;

std::mutex g_Mutex;

Signal* currentSignal;

// ---------------------------------------------------------------------

/**
 * Transforms raw values from fstapi into decimal (or -1 if it's error)
 * @author sylefeb
 */
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

/**
 * Generating all the scopes and signals, filling them with the values
 */
void FSTReader::initMaps() {
    //Generate all scopes and their's signals
    Scope *currentScope = nullptr;
    fstHier *hier = fstReaderIterateHier(g_Wave);
    do {
        switch (hier->htyp) {
            //Scope
            case FST_HT_SCOPE:
                if(currentScope == nullptr) {
                    currentScope = new Scope(*hier, nullptr);
                    this->scopes.push_back(currentScope);
                } else {
                    currentScope->children.push_back(new Scope(*hier, currentScope));
                    currentScope = currentScope->children.back();
                }
                break;
            case FST_HT_UPSCOPE:
                currentScope = currentScope->parent;
                break;
            case FST_HT_ATTRBEGIN:
//                std::cerr << "FST HT ATTRBEGIN" << hier->u.attr.arg << std::endl;
                break;
            //Signal
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

    //std::cout << this->scopes.front()->name << "\n";
    //std::cout << this->scopes.front()->signals[12].name << "\n";

    //fstReaderSetFacProcessMask(g_Wave, this->scopes.front()->signals[12].id);
/*
    fstReaderSetFacProcessMaskAll(g_Wave);

    //Get all values in g_Values & g_Errors
    std::thread th([]() {
        auto l = [](void *user_callback_data_pointer, uint64_t time, fstHandle facidx, const unsigned char *value) {
            std::unique_lock<std::mutex> lock(g_Mutex);
            //std::cout << "loading fst " << facidx << " " << time << "\n";
            int dvalue = decodeValue(reinterpret_cast<const char *>(value));
            if (dvalue != -1) { //error
                if(g_Values.size() <= facidx) {
                    g_Values.resize(facidx + 1);
                }
                g_Values[facidx].push_back({time, (ImU64) dvalue});
            } else { //value
                if(g_Errors.size() <= facidx) {
                    g_Errors.resize(facidx + 1);
                }
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
        valuesList *values = &item;
        auto res = std::find_if(values->begin(), values->end(), [maxTime](std::array<ImU64, 2> pair) {
            return pair[0] == maxTime;
        });
        if (res == values->end()) {
            ImU64 lastValue = 0;
            ImU64 lastTime = 0;
            for (const auto &value : *values) {
                lastTime = value[0];
                lastValue = value[1];
            }
            values->push_back({maxTime, lastValue});
        }
    }

    //Fill values & errors
    for (size_t i = 0; i < g_Values.size(); ++i) {
        fstHandle currentHandle = i;
        for (auto scope : this->scopes) {
            Signal *current = scope->getSignal(currentHandle);
            if (current)
                current->values = g_Values[i];
        }
    }

    for (size_t i = 0; i < g_Errors.size(); ++i) {
        fstHandle currentHandle = i;
        for (Scope *scope : this->scopes) {
            Signal *current = scope->getSignal(currentHandle);
            if (current)
                current->errors = g_Errors[i];
        }
    }*/
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
    currentSignal = this->getSignal(signal);
    if(currentSignal->errors.empty() && currentSignal->values.empty()) this->loadData();
    return currentSignal->values;
}

std::vector<int> FSTReader::getErrors(fstHandle signal) {
    currentSignal = this->getSignal(signal);
    if(currentSignal->errors.empty() && currentSignal->values.empty()) this->loadData();
    return currentSignal->errors;
}

// ---------------------------------------------------------------------

void FSTReader::loadData() {
    if (currentSignal->errors.empty() && currentSignal->values.empty()) {
        std::cout << "loading " << currentSignal->name << "\n";    
        fstReaderSetFacProcessMask(g_Wave, currentSignal->id);
        auto callback = [](void* user_callback_data_pointer, uint64_t time, fstHandle facidx, const unsigned char* value) {
            int dvalue = decodeValue(reinterpret_cast<const char*>(value));
            if (dvalue != -1) { //value
                currentSignal->values.push_back({ (ImU64)time, (ImU64)dvalue });
            } else {  //error
                currentSignal->errors.push_back(time);
            }
            //std::cout << facidx << " " << time << " " << dvalue << "\n";
        };
        fstReaderIterBlocks(g_Wave, callback, NULL, NULL);
        fstReaderClrFacProcessMask(g_Wave, currentSignal->id);
    }
}

// ---------------------------------------------------------------------

ImU64 FSTReader::getMaxTime() {
    ImU64 max = 0;
    for (const auto &item : g_Values) {
        valuesList values = item;
        for (const auto &value : values) {
            if (value[0] > max) {
                max = value[0];
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
            for (const auto &signal : scope->pairs) {
                if (signal.second->q->name.find("_q_index") != std::string::npos) {
                    for (const auto &item : signal.second->q->values) {
                        values.emplace_back(item[1]);
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