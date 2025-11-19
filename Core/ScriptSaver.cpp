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
