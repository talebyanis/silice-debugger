#pragma once

#include <list>
#include <string>
#include <map>
#include <vector>

#include "../../libs/fstapi/fstapi.h"
#include "imgui.h"

#include "Scope.h"

class FSTReader {
private :
    void initMaps();
public:
    std::vector<Scope*> scopes;

    FSTReader(const char *file);

    Signal* getSignal(fstHandle signal);

    ImU64 getMaxTime();
    valuesList getValues(fstHandle signal);
    errorsList getErrors(fstHandle signal);
};
