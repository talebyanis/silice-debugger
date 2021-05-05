#pragma once

#include <string>

#include "Signal.h"

class DQPair {
private:

public:
    std::string name;
    Signal *d;
    Signal *q;

    DQPair() = default;
    DQPair(std::string name);
};

