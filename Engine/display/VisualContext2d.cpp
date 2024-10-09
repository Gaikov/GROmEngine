//
// Created by Roman on 7/31/2024.
//

#include "VisualContext2d.h"
#include "engine/display/context/VisualMaskManager.h"

nsVisualMaskManager *nsVisualContext2d::GetMaskManager() const {
    if (!_maskManager) {
        _maskManager = new nsVisualMaskManager();
    }
    return _maskManager;
}

nsVisualContext2d::~nsVisualContext2d() {
    delete _maskManager;
}
