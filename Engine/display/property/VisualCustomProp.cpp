//
// Created by Roman on 1/27/2026.
//

#include "VisualCustomProp.h"
#include "Core/serialization/SerializeUtils.h"

nsVisualCustomProp::nsVisualCustomProp(const Type type, nsSerializable *value) : _type(type), _value(value) {
}


