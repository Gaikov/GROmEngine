// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file ScriptSaver.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "ScriptSaver.h"
#include "nsLib/StrTools.h"
#include "nsLib/log.h"

//---------------------------------------------------------
// nsScriptSaver::nsScriptSaver: 
//---------------------------------------------------------
nsScriptSaver::nsScriptSaver(const char *fileName) : m_file(nullptr),
                                                     m_tabCount(0) {
    if (!StrCheck(fileName)) return;
    m_file = fopen(fileName, "w");
    if (!m_file)
        LogPrintf(PRN_ALL, "WARNING: can't create '%s'\n", fileName);
}

//---------------------------------------------------------
// nsScriptSaver::~nsScriptSaver:
//---------------------------------------------------------
nsScriptSaver::~nsScriptSaver() {
    if (m_file)
        fclose(m_file);
}

//---------------------------------------------------------
// nsScriptSaver::Printf:
//---------------------------------------------------------
void nsScriptSaver::Printf(const char *fmt, ...) {
    char msg[MAX_OUT_LEN];

    va_list list;
    va_start(list, fmt);
    vsprintf(msg, fmt, list);
    va_end(list);

    if (m_file)
        fprintf(m_file, "%s%s\n", StrTabs(m_tabCount), msg);
}

void nsScriptSaver::Print(const char *line) const {
    if (m_file)
        fprintf(m_file, "%s%s\n", StrTabs(m_tabCount), line);
}

void nsScriptSaver::PrintVar(const char *name, const char *fmt, ...) const {
    char msg[MAX_OUT_LEN];

    va_list list;
    va_start(list, fmt);
    vsprintf(msg, fmt, list);
    va_end(list);

    if (m_file)
        fprintf(m_file, "%s$%s %s\n", StrTabs(m_tabCount), name, msg);
}

void nsScriptSaver::VarBool(const char *name, bool value, bool defValue) const {
    if (value != defValue) {
        PrintVar(name, "%d", value ? 1 : 0);
    }
}

void nsScriptSaver::VarFloat(const char *name, float value, float defValue) const {
    if (value != defValue) {
        PrintVar(name, "%f", value);
    }
}

void nsScriptSaver::VarFloat2(const char *name, const float value[2], const float defValue[2]) const {
    if (value[0] != defValue[0] || value[1] != defValue[1]) {
        PrintVar(name, "%f %f", value[0], value[1]);
    }
}

void nsScriptSaver::VarFloat3(const char *name, const float value[3], const float defValue[3]) const {
    if (value[0] != defValue[0] || value[1] != defValue[1] || value[2] != defValue[2]) {
        PrintVar(name, "%f %f %f", value[0], value[1], value[2]);
    }
}

void nsScriptSaver::VarFloat4(const char *name, const float value[4], const float defValue[4]) const {
    if (value[0] != defValue[0] || value[1] != defValue[1] || value[2] != defValue[2] || value[3] != defValue[3]) {
        PrintVar(name, "%f %f %f %f", value[0], value[1], value[2], value[3]);
    }
}

//---------------------------------------------------------
// nsScriptSaver::BlockBegin:
//---------------------------------------------------------
bool nsScriptSaver::BlockBegin(const char *name) {
    if (!StrCheck(name)) return false;

    Printf("$%s", name);
    Printf("{");
    m_tabCount++;
    return true;
}

//---------------------------------------------------------
// nsScriptSaver::BlockEnd:
//---------------------------------------------------------
void nsScriptSaver::BlockEnd() {
    m_tabCount--;
    Printf("}\n");
}

bool nsScriptSaver::IsValid() {
    return m_file;
}
