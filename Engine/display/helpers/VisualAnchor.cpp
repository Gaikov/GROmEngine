//
// Created by Roman on 10/28/2024.
//

#include "VisualAnchor.h"
#include "RenAux.h"

void nsVisualAnchor::GetLocalBounds(nsRect &bounds) {
    auto size2 = size / 2;
    bounds = {size2, size2, size, size};
}

void nsVisualAnchor::Loop() {

}

void nsVisualAnchor::DrawContent(const nsVisualContext2d &context) {
    nsGizmos::DrawCross({}, size, color);
}
