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
    reader->Seek(0, SEEK_SET);
    auto pFile = new nsFile(size);
    if (!pFile->GetData()) {
        delete pFile;
        return nullptr;
    }

    if (size && !reader->Read(pFile->GetData(), size)) {
        delete pFile;
        return nullptr;
    }
    pFile->GetData()[size] = 0;    //for file that will be used as a string
    return pFile;
}
