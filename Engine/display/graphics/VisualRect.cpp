//
// Created by Roman on 11/5/2024.
//

#include "VisualRect.h"
#include "RenAux.h"

void nsVisualRect::GetLocalBounds(nsRect &bounds) {
    bounds = rect;
}

void nsVisualRect::Loop() {

}

void nsVisualRect::DrawContent(const nsVisualContext2d &context) {
    nsGizmos::DrawRect(rect, color);
}
