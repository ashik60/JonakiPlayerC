#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <QRect>
#include <QString>

namespace Jonaki {

enum class SkinElementType {
    Unknown,
    Static,
    Button,
    Slider,
    Label,
    Layout,
    Image
};

struct SkinElementNode {
    SkinElementType type = SkinElementType::Unknown;
    std::string tag;
    std::string cmdId;
    std::string tooltip;
    std::string align;     // "Top", "Bottom", "Left", "Right", "Client"
    std::string text;
    std::string fontName;
    int fontSize = 11;

    int width = 0;
    int height = 0;
    int leftMargin = 0;
    int rightMargin = 0;
    int topMargin = 0;
    int bottomMargin = 0;

    // Spritesheet slice coordinates
    int imageLeft = 0;
    int imageTop = 0;
    int imageWidth = 0;
    int imageHeight = 0;

    int altImageLeft = 0;
    int altImageTop = 0;

    int backImageLeft = 0;
    int backImageTop = 0;

    int thumbImageLeft = 0;
    int thumbImageTop = 0;
    int thumbWidth = 0;
    int thumbHeight = 0;

    bool isTitleBar = false;
    bool autoSize = false;
    bool autoVisible = false;

    std::vector<std::shared_ptr<SkinElementNode>> children;
};

class SkinParser {
public:
    SkinParser() = default;

    /**
     * @brief Parses a PotPlayer XML skin file (e.g. TopCommon.txt, BottomVideo.txt),
     * evaluating macro guards like #if<...>, #else, #endif and building an element tree.
     */
    bool parseFile(const std::string& filePath, std::shared_ptr<SkinElementNode>& rootOut);
    bool parseXmlString(const std::string& xmlContent, std::shared_ptr<SkinElementNode>& rootOut);

    // Context macro flags
    void setMacro(const std::string& key, const std::string& value);

private:
    std::string preprocessMacros(const std::string& input);

    std::unordered_map<std::string, std::string> m_macros{
        {"CodePage", "1252"},
        {"SkinName", "VideoSkin"},
        {"PlayerType", "0"}
    };
};

} // namespace Jonaki
