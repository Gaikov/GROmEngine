// Copyright (c) 2003-2007, Roman Gaikov. All rights reserved.
//--------------------------------------------------------------------------------------------------
// file FileReader.cpp
// author Roman Gaikov 
//--------------------------------------------------------------------------------------------------
#include "FileReader.h"
#include "nsLib/log.h"
#include "nsLib/StrTools.h"

//---------------------------------------------------------
// nsFileReader::nsFileReader: 
//---------------------------------------------------------
nsFileReader::nsFileReader(const char *fileName, const char *mode) :
        m_fp(nullptr) {
    if (!StrCheck(fileName) || !StrCheck(mode)) return;

    //Log::Debug("Reading disk file: %s", fileName);
    m_fp = fopen(fileName, mode);
    if (!m_fp)
        LogPrintf(PRN_ALL, "WARNING: can't open file '%s'\n", fileName);
}

//---------------------------------------------------------
// nsFileReader::~nsFileReader: 
//---------------------------------------------------------
nsFileReader::~nsFileReader() {
    IDataReader::~IDataReader();
    if (m_fp)
        fclose(m_fp);
}

//---------------------------------------------------------
// nsFileReader::Read: 
//---------------------------------------------------------
bool nsFileReader::Read(void *data, uint size) {
    if (!m_fp) {
        Log::Warning("Can't read file. Invalid file handler!");
        return false;
    }
    return fread(data, size, 1, m_fp) == 1;
}

//---------------------------------------------------------
// nsFileReader::Seek: 
//---------------------------------------------------------
bool nsFileReader::Seek(long offset, int origin) {
    if (!m_fp) return false;
    return fseek(m_fp, offset, origin) == 0;
}

//---------------------------------------------------------
// nsFileReader::Tell: 
//---------------------------------------------------------
long nsFileReader::Tell() {
    if (!m_fp) return 0;
    return ftell(m_fp);
}

//---------------------------------------------------------
// nsFileReader::IsValid: 
//---------------------------------------------------------
bool nsFileReader::IsValid() {
    return m_fp != nullptr;
}