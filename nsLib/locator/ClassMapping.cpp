//
// Created by Roman on 10/22/2024.
//

#include "ClassMapping.h"

nsClassMapping& nsClassMapping::AsSingleton() {
    _singleton = true;
    return *this;
}

nsLocatable *nsClassMapping::GetInstance() {
    if (_singleton) {
        if (!_instance) {
            _instance = _factoryFunc();
            _instance->OnCreated();
        }
        return _instance;
    }
    return _factoryFunc();
}
