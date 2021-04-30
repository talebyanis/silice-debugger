#ifndef FSTReaderGuard
#define FSTReaderGuard

#include "../../libs/fstapi/fstapi.h"
#include <list>
#include "imgui.h"
#include <string>
#include <map>
#include <vector>

typedef std::list<std::pair<ImU64 , ImU64>> valuesList;

class FSTReader {
private :

    void initMaps();
public:
    ImU64 getMaxTime();
    FSTReader(const char *file);
    std::list<std::string> getScopes();
    std::list<fstHandle> getSignals(std::string scope);
    valuesList getValues(fstHandle signal);
    std::vector<int> getErrors(fstHandle signal);
    std::string getSignalName(fstHandle signal);
};

#endif