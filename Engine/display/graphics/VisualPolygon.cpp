//
// Created by Roman on 6/9/2026.
//

#include "VisualPolygon.h"
#include "Engine/RenManager.h"

void nsVisualPolygon::GetLocalBounds(nsRect &bounds) {
    if (points.empty()) {
        bounds = {};
        return;
    }

    float minX = points[0].x;
    float minY = points[0].y;
    float maxX = points[0].x;
    float maxY = points[0].y;

    for (size_t i = 1; i < points.size(); i++) {
        auto &p = points[i];
        if (p.x < minX) minX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.x > maxX) maxX = p.x;
        if (p.y > maxY) maxY = p.y;
    }

    bounds = {
        minX, minY,
        maxX - minX, maxY - minY
    };
}

void nsVisualPolygon::Loop() {
}

void nsVisualPolygon::DrawContent(const nsVisualContext2d &context) {
    auto numPoints = (int)points.size();
    if (numPoints < 2) {
        return;
    }

    static IVertexBuffer *vb = nullptr;
    static int maxCachedCount = 0;

    if (!vb || maxCachedCount < numPoints) {
        if (vb) {
            _device->VerticesRelease(vb);
        }

        maxCachedCount = numPoints;
        auto numIndexes = maxCachedCount * 2;
        vb = _device->VerticesCreate(maxCachedCount, numIndexes, true, false);
        vb->SetPrimitiveMode(PM_LINES);
    }

    vb->SetValidVertices(numPoints);
    vb->SetValidIndices(numPoints * 2);

    auto indexes = vb->GetWriteIndices();
    for (int i = 0; i < numPoints; i++) {
        indexes[i * 2] = i;
        indexes[i * 2 + 1] = (i + 1) % numPoints;
    }

    auto vertexes = vb->GetWriteVertices();
    for (int i = 0; i < numPoints; i++) {
        vertexes[i].v = {points[i].x, points[i].y, 0};
    }

    _device->TextureBind(nullptr);
    _device->StateApply(nullptr);
    _device->SetColor(color);
    _device->VerticesDraw(vb);
}
