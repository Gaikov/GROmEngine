//
// Created by Roman on 10/12/2024.
//

#include "FontsCache.h"
#include "nsLib/log.h"

nsFont *nsFontsCache::AllocateResource(const char *resourceName, int param) {
    auto font = new nsFont();
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

    _sysFont = LoadFont("default/sysfont.txt");
    if (!_sysFont) {
        Log::Error("Can't load default font!");
        return false;
    }
    return true;
}

void nsFontsCache::OnRelease() {
    ReleaseAll();
    nsSubSystem::OnRelease();
}

nsFont *nsFontsCache::LoadFont(const char *fileName) {
    return GetResource(fileName, 0);
}
