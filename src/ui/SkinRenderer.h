#pragma once

#include <string>
#include <unordered_map>
#include <QPixmap>
#include <QRect>
#include <QWidget>

namespace Jonaki {

struct SkinElement {
    std::string id;
    std::string tooltip;
    std::string cmdId;
    std::string align;
    QRect geometry;
    QRect spriteRect;
    QRect altSpriteRect;
};

class SkinRenderer {
public:
    static SkinRenderer& instance();

    bool loadSkinFromArchive(const std::string& dsfPath);
    bool loadSkinFromDirectory(const std::string& dirPath);

    const SkinElement* getElement(const std::string& cmdId) const;
    QPixmap renderElement(const std::string& cmdId, bool hovered = false, bool pressed = false) const;

    bool isLoaded() const { return m_loaded; }

private:
    SkinRenderer() = default;

    bool m_loaded = false;
    QPixmap m_mainAtlas;
    QPixmap m_controlAtlas;
    QPixmap m_playlistAtlas;

    std::unordered_map<std::string, SkinElement> m_elements;
};

} // namespace Jonaki
