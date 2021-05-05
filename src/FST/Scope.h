#pragma once

#include <string>
#include <map>
#include <vector>

#include <imgui.h>
#include <unordered_map>
#include "../../libs/fstapi/fstapi.h"

#include "DQPair.h"
#include "Signal.h"

class Scope {
private:

public:
    std::string name;
    std::unordered_map<fstHandle, Signal> signals;
    std::unordered_map<std::string, DQPair> pairs;

    Scope(fstHier hier);

    void addSignal(Signal signal);
    void addPair(DQPair pair);
    void add(fstHier hier);

    Signal* getSignal(fstHandle handle);
    DQPair* getPair(std::string pairName);
};


