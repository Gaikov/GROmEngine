// Copyright (c) 2003-2025, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file StructDef.h
// author Roman Gaikov
//--------------------------------------------------------------------------------------------------
#pragma once

#define DEFINE_STATIC_MAP(Name, Type, ...) \
class Name { \
public: \
    static Type Get(const char* str) { \
        static const struct { \
            const char* name; \
            Type value; \
        } data[] = __VA_ARGS__; \
        for (const auto& item : data) { \
            if (strcmp(item.name, str) == 0) return item.value; \
        } \
        return data[0].value; \
    } \
};
