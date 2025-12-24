//
// Created by Roman on 12/24/2025.
//

#include "UndoFileCreate.h"

#include "FileWriter.h"
#include "nsLib/log.h"

nsUndoFileCreate::nsUndoFileCreate(const nsFilePath &path, const char *content)
    : _path(path), _content(content) {
}

void nsUndoFileCreate::Init() {
    Redo();
}

void nsUndoFileCreate::Redo() {
    nsFileWriter writer(_path);

    if (!writer.Write(_content.c_str(), _content.size())) {
        Log::Error("Can't write file: %s", _path.AsChar());
    }
}

void nsUndoFileCreate::Undo() {
    if (!_path.Remove()) {
        Log::Error("Can't remove file: %s", _path.AsChar());
    }
}
