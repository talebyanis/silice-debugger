#include "Signal.h"

// ---------------------------------------------------------------------

Signal::Signal(fstHier hier) {
    this->name = hier.u.var.name;
    this->id = hier.u.var.handle;
}

Signal::Signal(std::string name, fstHandle id) {
    this->name = name;
    this->id = id;
}

// ---------------------------------------------------------------------