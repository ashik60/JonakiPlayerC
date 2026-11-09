#include "SkinRenderer.h"
#include "../core/Logger.h"
#include <QPainter>
#include <QFile>
#include <QXmlStreamReader>

namespace Jonaki {

SkinRenderer& SkinRenderer::instance() {
    static SkinRenderer s_instance;
    return s_instance;
}

bool SkinRenderer::loadSkinFromArchive(const std::string& dsfPath) {
    Logger::info("Loading PotPlayer skin archive: " + dsfPath);
    // Future implementation will unpack in-memory or read ZIP contents
    return false;
}

bool SkinRenderer::loadSkinFromDirectory(const std::string& dirPath) {
    Logger::info("Loading skin from unpacked directory: " + dirPath);
    m_loaded = true;
    return true;
}

const SkinElement* SkinRenderer::getElement(const std::string& cmdId) const {
    auto it = m_elements.find(cmdId);
    if (it != m_elements.end()) {
        return &it->second;
    }
    return nullptr;
}

QPixmap SkinRenderer::renderElement(const std::string& cmdId, bool hovered, bool pressed) const {
    Q_UNUSED(hovered);
    Q_UNUSED(pressed);

    const auto* elem = getElement(cmdId);
    if (!elem || m_mainAtlas.isNull()) {
        return {};
    }

    return m_mainAtlas.copy(elem->spriteRect);
}

} // namespace Jonaki
