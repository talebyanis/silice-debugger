#include "Scope.h"
#include <iostream>

// ---------------------------------------------------------------------

Scope::Scope(fstHier hier, std::string d_name, Scope *parent) {
    if(d_name.empty()) this->d_name = hier.u.scope.name;
    else this->d_name = d_name;
    this->name = hier.u.scope.name;
    this->parent = parent;
}

// ---------------------------------------------------------------------

void Scope::addSignal(Signal signal, bool internal) {
    std::unordered_map<fstHandle, Signal> *signals;
    if(internal) signals = &this->signalsInternal;
    else signals = &signalsUser;
    signals->insert(std::pair<fstHandle,Signal>(signal.id,signal));
}

void Scope::addPair(DQPair *pair, bool internal) {
    std::unordered_map<std::string, DQPair*> *pairs;
    if(internal) pairs = &this->pairsInternal;
    else pairs = &pairsUser;
    pairs->insert(std::pair<std::string, DQPair*>(pair->name, pair));
}

void Scope::add(fstHier hier, bool internal) {
    std::string name = hier.u.var.name;
    if(name[0] == '_' && (name[1] == 'q' || name[1] == 'd') && name[2] == '_') {
        DQPair* current;
        std::unordered_map<std::string, DQPair*> *pairs;
        if(internal) pairs = &this->pairsInternal;
        else pairs = &pairsUser;
        std::string subName = name.substr(3);
        if(pairs->find(subName) == pairs->end()) {
            current = new DQPair(subName);
            this->addPair(current, internal);
        } else {
            current = pairs->at(subName);
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
        this->addSignal(signal, internal);
    }
}

// ---------------------------------------------------------------------

Signal* Scope::getSignal(fstHandle handle) {
    Signal* signal = nullptr;
    if(this->signalsInternal.find(handle) != this->signalsInternal.end()) {
        signal = &this->signalsInternal.at(handle);
    } else {
        for (const auto &item : this->pairsInternal) {
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
    if(!signal) {
        if (this->signalsUser.find(handle) != this->signalsUser.end()) {
            signal = &this->signalsUser.at(handle);
        } else {
            for (const auto &item : this->pairsUser) {
                if (item.second->q) {
                    if (item.second->q->id == handle) {
                        signal = item.second->q;
                    } else if (item.second->d) {
                        if (item.second->d->id == handle) {
                            signal = item.second->d;
                        }
                    }
                }
            }
        }
    }
    if(!signal) {
        for (const auto &item : children) {
            if ((signal = item->getSignal(handle))) {
                break;
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

