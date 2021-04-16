#ifndef FSTReaderGuard
#define FSTReaderGuard

#include "../../libs/fstapi/fstapi.h"

typedef std::list<std::pair<uint64_t, uint64_t>> valuesList;

class FSTReader {
public:
    FSTReader(const char *file);
    valuesList getValues(fstHandle signal);
    std::string getSignalName(fstHandle signal);
};

#endif