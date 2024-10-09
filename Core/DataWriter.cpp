//
// Created by Roman on 5/16/2024.
//
#include "DataWriter.h"
#include "nsLib/log.h"

nsFile *IDataReader::ReadBlob(IDataReader *reader) {
    if (!reader || !reader->IsValid()) {
        return nullptr;
    }

    reader->Seek(0, SEEK_END);
    uint size = reader->Tell();
    if (!size) {
        Log::Warning("reader is empty");
        return nullptr;
    }

    reader->Seek(0, SEEK_SET);
    auto pFile = new nsFile(size);
    if (!pFile->GetData()) {
        delete pFile;
        return nullptr;
    }

    if (!reader->Read(pFile->GetData(), size)) {
        return nullptr;
    }
    pFile->GetData()[size] = 0;    //for file that will be used as a string
    return pFile;
}

