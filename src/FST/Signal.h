#pragma once

#include <string>
#include <vector>
#include <imgui.h>
#include <array>

#include "../../libs/fstapi/fstapi.h"


typedef std::vector<std::array<ImU64, 2>> valuesList;
typedef std::vector<int> errorsList;

class Signal {
private:

public:
    fstHandle id;
    std::string name;
    std::string scopeName;
    valuesList values;
    errorsList errors;

    Signal() = default;
    Signal(std::string name, fstHandle id, std::string scopeName);
    Signal(fstHier hier, std::string scopeName);
};



