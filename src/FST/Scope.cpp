#include "Scope.h"
#include <iostream>

// ---------------------------------------------------------------------

Scope::Scope(fstHier hier) {
    this->name = hier.u.scope.name;
}

// ---------------------------------------------------------------------

void Scope::addSignal(Signal signal) {
    this->signals.insert(std::pair<fstHandle,Signal>(signal.id,signal));
}

void Scope::addPair(DQPair *pair) {
    this->pairs.insert(std::pair<std::string, DQPair*>(pair->name, pair));
}

void Scope::add(fstHier hier) {
    std::string name = hier.u.var.name;
    if(name[0] == '_' && (name[1] == 'q' || name[1] == 'd') && name[2] == '_') {
        DQPair* current;
        std::string subName = name.substr(3);
        if(this->pairs.find(subName) == this->pairs.end()) {
            current = new DQPair(subName);
            this->addPair(current);
        } else {
            current = this->pairs.at(subName);
        }
        if(name[1] == 'd') {
            Signal* d = new Signal(hier,this->name);
            current->d = d;
        } else {
            Signal* q = new Signal(hier,this->name);
            current->q = q;
        }
    } else {
        Signal signal = Signal(hier, this->name);
        this->addSignal(signal);
    }
}

// ---------------------------------------------------------------------

Signal* Scope::getSignal(fstHandle handle) {
    Signal* signal = nullptr;
    if(this->signals.find(handle) != this->signals.end()) {
        signal = &this->signals.at(handle);
    } else {
        for (const auto &item : this->pairs) {
            if(item.second->q) {
                if(item.second->q->id == handle) {
                    signal = item.second->q;
                } else if(item.second->d) {
                    if (item.second->d->id == handle) {
                        signal = item.second->d;
                    }
                }
            }
        }
    }
    return signal;
}
/*
DQPair* Scope::getPair(std::string pairName) {
    if(this->pairs.at(pairName)) {
        return this->pairs.at(pairName);
    }
    return nullptr;
}*/


// ---------------------------------------------------------------------

