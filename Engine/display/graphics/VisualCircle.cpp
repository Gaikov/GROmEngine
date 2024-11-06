//
// Created by Roman on 11/6/2024.
//

#include "VisualCircle.h"
#include "RenAux.h"

void nsVisualCircle::GetLocalBounds(nsRect &bounds) {
    float size = radius * 2;
    bounds = {
            -radius, -radius, size, size
    };
}

void nsVisualCircle::Loop() {

}

void nsVisualCircle::DrawContent(const nsVisualContext2d &context) {
    nsGizmos::DrawCircle({}, radius, color);
}
