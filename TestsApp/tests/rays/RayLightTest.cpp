//
// Created by Roman on 8/2/2024.
//

#include "RayLightTest.h"
#include "engine/RenAux.h"
#include "engine/RenManager.h"

IVertexBuffer *nsRayLightTest::_vb = nullptr;
IRenState *nsRayLightTest::_rs = nullptr;

nsRayLightTest::nsRayLightTest(ITexture *tex, float radius) :
        _tex(tex),
        _radius(radius) {
    float step = M_2PI / NUM_RAYS;
    float angle = 0;
    for (int i = 0; i < NUM_RAYS; i++) {
        _rays.push_back({
                                  sinf(angle),
                                  cosf(angle)
                          });
        angle += step;
    }
    _castedPoints = _rays;

    EnsureBuffer();
}

void nsRayLightTest::GetLocalBounds(nsRect &bounds) {
    bounds.x = -_radius;
    bounds.y = -_radius;
    bounds.width = _radius * 2;
    bounds.height = _radius * 2;
}

void nsRayLightTest::Loop() {

}

void nsRayLightTest::DrawContent(const nsVisualContext2d &context) {
    _device->SetColor(color);
    _device->TextureBind(_tex);
    _device->StateApply(_rs);
    auto verts = _vb->GetWriteVertices();

    float k = 0.5f / _radius;

    for (int i = 0; i < NUM_RAYS; i++) {
        auto &p = _castedPoints[i];
        auto &v = verts[i];
        v.v = p;

        nsVec2  uv = p * k + 0.5f;
        v.tu = uv.x;
        v.tv = uv.y;
    }

    _device->VerticesDraw(_vb);
}

void nsRayLightTest::CastShadows(const std::vector<nsRayBoxTest *> &objects) {
    nsVec2 zero = origin.ToGlobal(nsVec2());
    for (int i = 0; i < NUM_RAYS; i++) {

        nsVec2 &localRay = _rays[i];
        nsVec2 ray = origin.GetWorld().TransformVector(localRay);
        float distance = _radius;

        for (auto obj: objects) {
            nsVec2 start = obj->origin.ToLocal(zero);
            nsVec2 dir = obj->origin.GetWorldInvert().TransformVector(ray);

            float dist = nsRayMarching::GetDistance(&obj->sdFunc, start, dir, _radius);
            if (dist >= 0 && dist < distance) {
                distance = dist;
            }
        }

        nsVec2 &targetPoint = _castedPoints[i];
        targetPoint = localRay * distance;
    }
}

void nsRayLightTest::EnsureBuffer() {
    if (!_vb) {
        int numVerts = NUM_RAYS + 1;
        int numIndexes = NUM_RAYS * 3;
        _vb = _device->VerticesCreate(numVerts, numIndexes, true, false);
        auto indexes = _vb->GetWriteIndices();

        for (int i = 0; i < NUM_RAYS; i++) {
            int index2 = (i + 1) % NUM_RAYS;
            int index3 = (i + 2) % NUM_RAYS;

            indexes[0] = NUM_RAYS;
            indexes[1] = index3;
            indexes[2] = index2;
            indexes += 3;
        }

        auto verts = _vb->GetWriteVertices();
        auto &last = verts[NUM_RAYS];
        last.tu = last.tv = 0.5f;

        _vb->SetValidIndices(numIndexes);
        _vb->SetValidVertices(numVerts);

        _rs = _device->StateLoad("scripts/rs/gui_add.txt");
    }
}

