//
// Created by Roman on 10/12/2024.
//

#include "FontsCache.h"

#include "RenManager.h"
#include "nsLib/log.h"

nsFont *nsFontsCache::AllocateResource(const char *resourceName, int param) {
    const auto font = new nsFont(_renBuffer);
    if (!font->Load(resourceName)) {
        delete font;
        return nullptr;
    }
    return font;
}

void nsFontsCache::FreeResource(nsFont *item) {
    Log::Info("...releasing font: %s", item->m_fileName.AsChar());
    delete item;
}

bool nsFontsCache::OnInit() {
    if (!nsSubSystem::OnInit()) {
        return false;
    }

    _renBuffer = new nsQuadsBuffer(nsRenDevice::Shared()->Device(), 10000, true);

    _sysFont = LoadFont("default/sysfont.txt");
    if (!_sysFont) {
        Log::Error("Can't load default font!");
        return false;
    }
    return true;
}

void nsFontsCache::OnRelease() {
    ReleaseAll();
    delete _renBuffer;
    nsSubSystem::OnRelease();
}

nsFont *nsFontsCache::LoadFont(const char *fileName) {
    return GetResource(fileName, 0);
}
