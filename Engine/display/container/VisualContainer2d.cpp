//
// Created by Roman on 6/28/2024.
//

#include "VisualContainer2d.h"
#include "Engine/display/context/VisualMaskManager.h"

nsVisualContainer2d::nsVisualContainer2d() {
    _children.reserve(20);
}

void nsVisualContainer2d::Destroy() {
    DestroyChildren();
    nsVisualObject2d::Destroy();
}

void nsVisualContainer2d::DestroyChildren() {
    auto children = _children; //copy to not break iterator
    for (auto child: children) {
        RemoveChild(child);
        child->Destroy();
    }
    _children.clear();
}

void nsVisualContainer2d::AddChild(nsVisualObject2d *obj) {
    assert(obj != this && obj);

    if (obj->_parent) {
        obj->_parent->RemoveChild(obj);
    }

    _children.push_back(obj);
    origin.AddChild(&obj->origin);
    obj->_parent = this;
    if (IsOnStage()) {
        obj->OnAddedToStage();
    }
}

void nsVisualContainer2d::RemoveChild(nsVisualObject2d *obj) {
    auto it = std::find(_children.begin(), _children.end(), obj);
    if (it != _children.end()) {
        _children.erase(it);
        obj->_parent = nullptr;
        origin.RemoveChild(&obj->origin);
        if (_childOver == dynamic_cast<IUserInput*>(obj)) {
            _childOver = nullptr;
        }
        if (IsOnStage()) {
            obj->OnRemovedFromStage();
        }
    }
}

void nsVisualContainer2d::RemoveChildren() {
    for (auto child: _children) {
        child->_parent = nullptr;
        origin.RemoveChild(&child->origin);
        if (IsOnStage()) {
            child->OnRemovedFromStage();
        }
    }
    _children.clear();
}

void nsVisualContainer2d::DrawContent(const nsVisualContext2d &context) {
    for (auto child: _children) {
        if (child->visible) {
            child->DrawNode(context);
        }
    }
}

void nsVisualContainer2d::GetLocalBounds(nsRect &bounds) {
    nsRect childBounds;

    int visibleChildren = 0;
    for (auto child: _children) {
        if (child->visible) {
            child->GetBounds(childBounds, this);
            if (!visibleChildren) {
                visibleChildren++;
                bounds = childBounds;
            } else {
                bounds += childBounds;
            }
        }
    }

    if (!visibleChildren) {
        bounds = {0, 0, 0, 0};
    }
}

void nsVisualContainer2d::Loop() {
    for (auto child: _children) {
        if (child->visible) {
            child->Loop();
        }
    }
}

bool nsVisualContainer2d::HitTest(float x, float y) {
    for (auto it = _children.rbegin(); it != _children.rend(); it++) {
        if ((*it)->visible && (*it)->HitTest(x, y)) {
            return true;
        }
    }
    return false;
}

#define ITER_INPUT_CHILDREN(expr) \
    if (interactiveChildren) \
    { \
        for (auto it = _children.rbegin(); it != _children.rend(); ++it) { \
            auto input = dynamic_cast<IUserInput*>(*it); \
            if (input && (*it)->visible) { \
                expr; \
            } \
        } \
    }

bool nsVisualContainer2d::OnPointerUp(float x, float y, int pointerId) {
    ITER_INPUT_CHILDREN(if (input->OnPointerUp(x, y, pointerId)) return true)
    return false;
}

bool nsVisualContainer2d::OnPointerDown(float x, float y, int pointerId) {
    ITER_INPUT_CHILDREN(if (input->OnPointerDown(x, y, pointerId)) return true);
    return false;
}

bool nsVisualContainer2d::OnPointerMove(float x, float y, int pointerId) {
    if (!interactiveChildren) {
        return false;
    }

    for (auto it = _children.rbegin(); it != _children.rend(); ++it) {
        auto input = dynamic_cast<IUserInput *>(*it);
        if (input && (*it)->visible) {
            if (input->OnPointerMove(x, y, pointerId)) {
                if (_childOver && _childOver != input) {
                    _childOver->OnPointerCancel(pointerId);
                }
                _childOver = input;
                return true;
            }
        }
    }
    return false;
}

void nsVisualContainer2d::OnKeyUp(int key) {
    ITER_INPUT_CHILDREN(input->OnKeyUp(key))
}

void nsVisualContainer2d::OnKeyDown(int key, bool rept) {
    ITER_INPUT_CHILDREN(input->OnKeyDown(key, rept))
}

void nsVisualContainer2d::OnChar(char ch) {
    ITER_INPUT_CHILDREN(input->OnChar(ch))
}

bool nsVisualContainer2d::OnMouseWheel(float delta) {
    ITER_INPUT_CHILDREN(if (input->OnMouseWheel(delta)) return true);
    return false;
}
void nsVisualContainer2d::OnAddedToStage() {
    nsVisualObject2d::OnAddedToStage();
    for (const auto c : _children) {
        c->OnAddedToStage();
    }
}
void nsVisualContainer2d::OnRemovedFromStage() {
    nsVisualObject2d::OnRemovedFromStage();
    for (const auto c : _children) {
        c->OnRemovedFromStage();
    }
}

void nsVisualContainer2d::OnPointerCancel(int pointerId) {
    ITER_INPUT_CHILDREN(input->OnPointerCancel(pointerId))
}

void nsVisualContainer2d::DrawNode(const nsVisualContext2d &context) {
    auto mm = context.GetMaskManager();
    mm->PushMasks(&_masks, context);

    ApplyWorldMatrix();
    DrawContent(context);

    mm->PopMasks(&_masks, context);
}

nsVisualObject2d *nsVisualContainer2d::GetChildById(const char *id) {
    for (auto child : _children) {
        if (child->id == id) {
            return child;
        }
    }
    return nullptr;
}

nsVisualObject2d *nsVisualContainer2d::GetChildByIdRecursive(const char *id) {
    for (auto child : _children) {
        if (child->id == id) {
            return child;
        }

        auto container = dynamic_cast<nsVisualContainer2d*>(child);
        if (container) {
            auto found = container->GetChildByIdRecursive(id);
            if (found) {
                return found;
            }
        }
    }
    return nullptr;
}

bool nsVisualContainer2d::IterateRecursive(const tChildCallback &callback) {
    for (auto child : _children) {
        if (!callback(child)) {
            return false;
        }

        auto container = dynamic_cast<nsVisualContainer2d*>(child);
        if (container) {
            if (!container->IterateRecursive(callback)) {
                return false;
            }
        }
    }
    return true;
}

nsVisualContainer2d * nsVisualContainer2d::CreateStage() {
    const auto stage = new nsVisualContainer2d();
    stage->id = "stage";
    stage->OnAddedToStage();
    return stage;
}

