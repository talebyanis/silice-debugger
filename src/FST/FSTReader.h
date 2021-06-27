#pragma once

#include <list>
#include <string>
#include <map>
#include <vector>
#include "TextEditor.h"

#include "../../libs/fstapi/fstapi.h"
#include "imgui.h"

#include "Scope.h"

/**
 * This class is used to read .fst files and store informations in Scopes
 */
class FSTReader {
private :
    void initMaps();
    LogParser lp;
public:
    std::vector<Scope*> scopes;

    FSTReader(const char *file, LogParser& logparser);

    Signal* getSignal(fstHandle signal);

    void loadData();

    ImU64 getMaxTime();
    valuesList getValues(fstHandle signal);
    unknownList getUnkowns(fstHandle signal);
    std::list<int> get_q_index_values();
};
