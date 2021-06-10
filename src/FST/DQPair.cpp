#include "DQPair.h"

// ---------------------------------------------------------------------

DQPair::DQPair(std::string name, std::string type) {
    this->name = name;
    this->d = nullptr;
    this->q = nullptr;
    this->type = type;
}

// ---------------------------------------------------------------------

