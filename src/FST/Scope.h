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
    std::string d_name;
    std::unordered_map<fstHandle, Signal> signalsInternal;
    std::unordered_map<std::string, DQPair*> pairsInternal;
    std::unordered_map<fstHandle, Signal> signalsUser;
    std::unordered_map<std::string, DQPair*> pairsUser;

    Scope* parent;
    std::vector<Scope*> children;

    Scope(fstHier hier, std::string d_name, Scope *parent);

    void addSignal(Signal signal, bool internal);
    void addPair(DQPair *pair, bool internal);
    void add(fstHier hier, bool internal, std::string usage);

    Signal* getSignal(fstHandle handle);
    DQPair* getPair(std::string pairName);
};


