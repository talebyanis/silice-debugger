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
                    currentScope = new Scope(*hier, lp.getAlgoLine(hier->u.scope.name).d_name, nullptr);
                    this->scopes.push_back(currentScope);
                } else {
                    currentScope->children.push_back(new Scope(*hier, lp.getAlgoLine(hier->u.scope.name).d_name, currentScope));
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
                    char *name = const_cast<char *>(hier->u.var.name);
                    std::string splitName = std::strtok(name, " ");
                    report_line rl = lp.getLineFromVName(splitName);

                    std::string signalName = rl.varname;
                    if(signalName.find(rl.token + "_") == 0) {
                        signalName[rl.token.size()] = '.';
                    }
                    if(signalName.find("__block") == 0) {
                        signalName = rl.token;
                    }
                    if(rl.v_name == "#") { //internal
                        currentScope->add(*hier, true, "#");
                    } else { //user
                        if(rl.usage == "ff") { //flip-flop
                            DQPair* current;
                            if(currentScope->pairsUser.find(signalName) == currentScope->pairsUser.end()) {
                                current = new DQPair(signalName, rl.type);
                                currentScope->addPair(current, false);
                            } else {
                                current = currentScope->pairsUser.at(signalName);
                            }
                            if(hier->u.var.name[1] == 'd') {
                                Signal* d = new Signal(*hier,currentScope->name);
                                current->d = d;
                            } else {
                                Signal* q = new Signal(*hier,currentScope->name);
                                current->q = q;
                            }
                        } else {
                            currentScope->addSignal(Signal(signalName, hier->u.var.handle, currentScope->name, rl.type),false);
                        }
                    }
                }
                break;
            default:
//                std::cerr << "default " << hier->u.var.name << std::endl;
                break;
        }
        hier = fstReaderIterateHier(g_Wave);
    } while (hier != NULL);
}

// ---------------------------------------------------------------------

Signal *FSTReader::getSignal(fstHandle signal) {
    Signal *res;
    for (const auto &scope : scopes) {
        if ((res = scope->getSignal(signal))) {
            break;
        }
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

std::list<int> FSTReader::get_q_index_values()
{
    std::list<int> values;

    int a = -1;

    for (const auto &scope : scopes) {
        if (scope->name == "__main")
        {
            for (const auto &signal : scope->pairsInternal) {
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

FSTReader::FSTReader(const char *file, LogParser& logparser) {
    clean();
    if (!LibSL::System::File::exists(file)) {
        std::cerr << "Could not open fst wave file " << file << std::endl;
    }
    g_Wave = fstReaderOpen(file);
    initMaps();
    this->lp = logparser;
}