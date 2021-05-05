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

void Scope::addPair(DQPair pair) {
    this->pairs.at(pair.name) = pair;
}

void Scope::add(fstHier hier) {
    std::string name = hier.u.var.name;
    //FIXME
    if(false/*name[0] == '_' && (name[1] == 'q' || name[1] == 'd') && name[2] == '_'*/) {
        DQPair* current;
        if(this->pairs.find(name) == this->pairs.end()) {
            current = new DQPair(name.substr(3));
            this->addPair(*current);
        } else {
            current = &this->pairs.at(name.substr(3));
        }
        if(name[1] == 'd') current->d = new Signal(hier);
        else current->q = new Signal(hier);
    } else {
        Signal signal = Signal(hier);
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
            if(item.second.q->id == handle) signal = item.second.q;
            else if(item.second.d->id == handle) signal = item.second.d;
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

