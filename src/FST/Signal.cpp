#include "Signal.h"

// ---------------------------------------------------------------------

Signal::Signal(fstHier hier , std::string scopeName) {
    this->name = hier.u.var.name;
    this->id = hier.u.var.handle;
    this->scopeName = scopeName;
}

Signal::Signal(std::string name, fstHandle id, std::string scopeName) {
    this->name = name;
    this->id = id;
    this->scopeName = scopeName;
}

// ---------------------------------------------------------------------