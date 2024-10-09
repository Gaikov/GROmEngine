//
// Created by Roman on 5/16/2024.
//

#pragma once
#include "StdAfx.h"
#include "Args.h"
#include "Core/PackArch.h"
#include "Core/Blob.h"

class nsPackStrategy {
public:
    explicit nsPackStrategy(const Args &args);
    bool Perform();

private:
    typedef std::vector<packFileDesc_t> file_list_t;

    file_list_t _fileList;
    const Args &_args;
    nsString _pass;

private:
    std::shared_ptr<nsFile> ReadFile(const char *filename);

    void CodingText(unsigned char *data, unsigned int size);
    void CodingFile(const nsFilePath &filePath, std::shared_ptr<nsFile> &file);

};
