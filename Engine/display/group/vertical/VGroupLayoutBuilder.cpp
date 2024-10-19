//
// Created by Roman on 10/19/2024.
//

#include "VGroupLayoutBuilder.h"
#include "display/layouts/VGroupLayout.h"

nsVisualObject2d *nsVGroupLayoutBuilder::CreateDefault() {
    return new nsVGroupLayout;
}
