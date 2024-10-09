//
// Created by Roman on 5/16/2024.
//

#include "PackStrategy.h"
#include "nsLib/log.h"
#include "Core/FileWriter.h"
#include "Core/FileReader.h"
#include "Options.h"
#include "Core/Crypt.h"

nsPackStrategy::nsPackStrategy(const Args &args) : _args(args) {

}

bool nsPackStrategy::Perform() {
    if (_args.Length() < 3) {
        printf("Not enough params to pack data!");
        return false;
    }

    _pass = _args.GetByName(OPT_PASS);
    if (!_pass.IsEmpty()) {
        Log::Info("Use encryption with: %s", _pass.AsChar());
    }

    //enumerating
    printf("Enumerating files...\n");
    _fileList.clear();
    nsFilePath inputFolder(_args.GetParam(1));
    if (!inputFolder.IsFolder()) {
        Log::Warning("Invalid input folder!");
        return false;
    }

    nsFilePath::tList filesPaths;
    inputFolder.ListingRecursive(filesPaths);

    for (auto &file: filesPaths) {
        char *path = strstr(file, inputFolder);
        if (path) {
            path += strlen(inputFolder) + 1;
            packFileDesc_t desc;
            strcpy(desc.filename, path);
            _fileList.push_back(desc);
        }
    }

    printf("Files: %i\n", (int) _fileList.size());

    nsFilePath outputFile(_args.GetParam(2));
    auto folder = outputFile.GetParent();
    if (!folder.CreateFolders()) {
        Log::Error("Can't create folders: %s", (const char *) folder);
        return false;
    }

    //packing
    nsFileWriter packWriter(outputFile, "wb");
    if (!packWriter.IsValid()) {
        printf("ERROR: create file '%s'\n", (const char *) outputFile);
        return false;
    }

    //writeheaders
    printf("Packing files...\n");
    packHeader_t ph;
    ph.version = PACK_VERSION;
    ph.id[0] = 'P';
    ph.id[1] = 'A';
    ph.id[2] = 'C';
    ph.id[3] = 'K';
    ph.dir_size = _fileList.size() * sizeof(packFileDesc_t);
    if (!packWriter.Write(&ph, sizeof(ph))) {
        printf("ERROR: write pack header!\n");
        return false;
    }

    if (!packWriter.Write(&_fileList[0], sizeof(packFileDesc_t) * _fileList.size())) {
        printf("ERROR: write file directory!\n");
        return false;
    }

    for (int i = 0; i < _fileList.size(); i++) {
        auto &filePath = filesPaths[i];
        auto &desc = _fileList[i];

        //printf("packing: [%s] -> %s", (const char *) filePath, desc.filename);

        desc.offset = packWriter.Tell();

        auto data = ReadFile(filePath);
        if (!data) return false;

        desc.size = data->GetSize();

        CodingFile(filePath, data);

        if (!packWriter.Write(data->GetData(), desc.size)) {
            printf("ERROR: write data!\n");
            return false;
        }
    }

    //rewrite file dir
    packWriter.Seek(sizeof(packHeader_t), SEEK_SET);

    if (!packWriter.Write(&_fileList[0], sizeof(packFileDesc_t) * _fileList.size())) {
        printf("ERROR: write file directory!\n");
        return false;
    }

    packWriter.Seek(0, SEEK_END);
    Log::Info("Total size: %i Mb", (int) packWriter.Tell() / 1024 / 1024);

    printf("DONE!\n");
    return true;
}

std::shared_ptr<nsFile> nsPackStrategy::ReadFile(const char *filename) {
    nsFileReader reader(filename, "rb");

    reader.Seek(0, SEEK_END);
    auto file = std::make_shared<nsFile>(reader.Tell());

    reader.Seek(0, SEEK_SET);
    if (!reader.Read(file->GetData(), file->GetSize())) {
        return nullptr;
    }

    return file;
}

//---------------------------------------------------------
// CodingText:
//---------------------------------------------------------
void nsPackStrategy::CodingText(unsigned char *data, unsigned int size) {
    unsigned char hi, lo;
    for (int i = 0; i < size; i++) {
        lo = data[i] & 0x0F;
        hi = data[i] & 0xF0;
        data[i] = (lo << 4) | (hi >> 4);
    }
}

void nsPackStrategy::CodingFile(const nsFilePath &filePath, std::shared_ptr<nsFile> &file) {
    if (!_pass.IsEmpty()) {
        nsCrypt::XorEncode(file->GetData(), (int)file->GetSize(), _pass);
    } else {
        auto ext = filePath.GetExtension();
        ext.ToLower();

        if (ext == "txt") {
            CodingText(file->GetData(), file->GetSize());
        }
    }
}

