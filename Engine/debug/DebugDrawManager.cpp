//
// Created by Roman on 6/26/2025.
//

#include "DebugDrawManager.h"

void nsDebugDrawManager::AddPolicy(nsDebugDrawPolicy::sp_t policy) {
    _list.push_back(policy);
}

void nsDebugDrawManager::Draw() {
    for (auto &policy : _list) {
        policy->Draw();
    }
}
