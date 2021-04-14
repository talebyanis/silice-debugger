#include <iostream>
#include <LibSL.h>
#include "../libs/fstapi/fstapi.h"

int mainFST() {
    void *g_Wave = fstReaderOpen("icarus.fst");
    struct fstHier *hier = fstReaderIterateHier(g_Wave);
    do {
        switch (hier->htyp) {
            case FST_HT_SCOPE:
                std::cerr << "\nscope : " << hier->u.scope.name << std::endl;
                break;
            case FST_HT_ATTRBEGIN:
                //std::cerr << "attr  : " << hier->u.attr.name << std::endl;
                break;
            case FST_HT_VAR:
                std::cerr << "signal: " << hier->u.var.name << std::endl;
                std::cerr << fstReaderGetStartTime(hier) << std::endl;
                break;
            default:
                //std::cerr << "unknown" << std::endl;
                break;
        }
        hier = fstReaderIterateHier(g_Wave);
    } while (hier != NULL);

    fstReaderSetFacProcessMaskAll(g_Wave);


    return 0;
}
