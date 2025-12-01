//
// Created by Roman on 10/28/2024.
//

#include "VisualAnchor.h"
#include "RenAux.h"

nsVisualAnchor::nsVisualAnchor() {
    renState = _device->StateLoad("default/rs/gui.ggrs");
}

void nsVisualAnchor::GetLocalBounds(nsRect &bounds) {
    const auto size2 = size / 2;
    bounds = {-size2, -size2, size, size};
}

void nsVisualAnchor::Loop() {

}

void nsVisualAnchor::DrawContent(const nsVisualContext2d &context) {
    _device->StateApply(renState);
    nsGizmos::DrawCross({}, size, color);
}

