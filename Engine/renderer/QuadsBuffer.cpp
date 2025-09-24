//
// Created by Roman on 5/30/2024.
//

#include "QuadsBuffer.h"
#include "RenDevice.h"

nsQuadsBuffer::nsQuadsBuffer(IRenDevice *dev, const uint maxQuads, const bool useColors) :
        _dev(dev),
        _maxQuads(maxQuads) {
    const uint numVerts = maxQuads * 4;
    const uint numIndexes = maxQuads * 6;

    _vb = dev->VerticesCreate(numVerts, numIndexes, true, useColors);
    _numQuads = 0;
}

bool nsQuadsBuffer::AddQuad(const nsVec2 &p,
                            const nsVec2 &pivot,
                            const nsVec2 &size,
                            const nsColor &c,
                            const nsVec2 &uvStart,
                            const nsVec2 &uvEnd,
                            const float angle) {
    if (_numQuads >= _maxQuads) {
        //Log::Warning("Can't add another quad: %i", _numQuads);
        return false;
    }

    auto vtx = _vb->GetWriteVertices();
    auto vi = _numQuads * 4;
    auto color = c.ToARGB();

    auto _cos = 1.0f;
    auto _sin = 0.0f;
    if (angle != 0.0f) {
        _cos = cosf(-angle);
        _sin = sinf(-angle);
    }

    nsVec2 xAxis = {
            _cos,
            _sin,
    };

    auto yAxis = xAxis.GetLeft();

    nsVec2 pos = p;

    if (pivot.x != 0.0f) {
        pos -= xAxis * pivot.x;
    }

    if (pivot.y != 0.0f) {
        pos -= yAxis * pivot.y;
    }

    yAxis *= size.y;
    xAxis *= size.x;

    nsVec2 verts[] = {
            pos,
            pos + xAxis,
            pos + xAxis + yAxis,
            pos + yAxis
    };

    nsVec2 uv[] = {
            {uvStart.x, uvEnd.y},
            uvEnd,
            {uvEnd.x, uvStart.y},
            uvStart
    };

    for (int i = 0; i < 4; i++) {
        auto v = &vtx[vi + i];
        v->v = verts[i];
        v->c = color;
        v->tu = uv[i].x;
        v->tv = uv[i].y;
    }

    auto idx = _vb->GetWriteIndices();
    auto ii = _numQuads * 6;

    word indexes[] = {
            0, 1, 2, 0, 2, 3
    };

    for (int i = 0; i < 6; i++) {
        idx[ii + i] = indexes[i] + vi;
    }

    _numQuads++;
    return true;
}

void nsQuadsBuffer::Draw() {
    if (_numQuads > 0) {
        _vb->SetValidVertices(_numQuads * 4);
        _vb->SetValidIndices(_numQuads * 6);
        _dev->VerticesDraw(_vb);
    }
}

void nsQuadsBuffer::Clear() {
    _numQuads = 0;
    _vb->SetValidVertices(0);
    _vb->SetValidIndices(0);
}
