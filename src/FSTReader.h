#ifndef FSTReaderGuard
#define FSTReaderGuard

#include "../../libs/fstapi/fstapi.h"

typedef std::list<std::pair<ImU64 , ImU64>> valuesList;

class FSTReader {
private :
    ImU64 getMaxTime();
    void initMaps();
    void value_change_callback(void *user_callback_data_pointer, uint64_t time, fstHandle facidx, const unsigned char *value);
public:
    FSTReader(const char *file);
    std::list<std::string> getScopes();
    std::list<fstHandle> getSignals(std::string scope);
    valuesList getValues(fstHandle signal);
    std::string getSignalName(fstHandle signal);
};

#endif