#define GEODE_DEFINE_EVENT_EXPORTS
#include "../include/TextureLoader.hpp"

#include "PackManager.hpp"

using namespace geode;

namespace ext = geode::texture_loader;

ext::Pack convertPack(std::shared_ptr<Pack> const& pack) {
    ext::Pack res;
    res.id = pack->getID();
    res.name = pack->getDisplayName();
    res.path = pack->getOriginPath();
    res.resourcesPath = pack->getResourcesPath();
    // if the pack has a mod.json
    if (auto opt = pack->getInfo()) {
        auto info = opt.value();
        res.version = info.m_version;
        res.authors = info.m_authors;
    }
    return res;
}

std::vector<ext::Pack> ext::getAvailablePacks() {
    return utils::ranges::map<std::vector<ext::Pack>>(PackManager::get()->getAvailablePacks(), convertPack);
}

std::vector<ext::Pack> ext::getAppliedPacks() {
    return utils::ranges::map<std::vector<ext::Pack>>(PackManager::get()->getAppliedPacks(), convertPack);
}
