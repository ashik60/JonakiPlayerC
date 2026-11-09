#include "SkinParser.h"
#include "../core/Logger.h"

#include <QFile>
#include <QXmlStreamReader>
#include <sstream>
#include <regex>

namespace Jonaki {

void SkinParser::setMacro(const std::string& key, const std::string& value) {
    m_macros[key] = value;
}

std::string SkinParser::preprocessMacros(const std::string& input) {
    std::istringstream stream(input);
    std::string line;
    std::ostringstream output;

    std::vector<bool> conditionStack;
    conditionStack.push_back(true);

    std::regex ifRegex(R"(#if\s*<\s*([A-Za-z0-9_]+)\s*==\s*([A-Za-z0-9_]+)\s*>)");
    std::smatch match;

    while (std::getline(stream, line)) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
        trimmed.erase(trimmed.find_last_not_of(" \t\r\n") + 1);

        if (std::regex_search(trimmed, match, ifRegex)) {
            std::string varName = match[1].str();
            std::string expectedVal = match[2].str();
            auto it = m_macros.find(varName);
            bool cond = (it != m_macros.end() && it->second == expectedVal);
            bool parentActive = conditionStack.back();
            conditionStack.push_back(parentActive && cond);
            continue;
        } else if (trimmed.rfind("#else", 0) == 0) {
            if (conditionStack.size() > 1) {
                bool current = conditionStack.back();
                conditionStack.pop_back();
                bool parentActive = conditionStack.back();
                conditionStack.push_back(parentActive && !current);
            }
            continue;
        } else if (trimmed.rfind("#endif", 0) == 0) {
            if (conditionStack.size() > 1) {
                conditionStack.pop_back();
            }
            continue;
        }

        if (conditionStack.back()) {
            output << line << "\n";
        }
    }

    return output.str();
}

static SkinElementType parseType(const QString& tag) {
    if (tag == "Static") return SkinElementType::Static;
    if (tag == "Button") return SkinElementType::Button;
    if (tag == "Slider") return SkinElementType::Slider;
    if (tag == "Label") return SkinElementType::Label;
    if (tag == "Layout") return SkinElementType::Layout;
    if (tag == "Image") return SkinElementType::Image;
    return SkinElementType::Unknown;
}

bool SkinParser::parseFile(const std::string& filePath, std::shared_ptr<SkinElementNode>& rootOut) {
    QFile file(QString::fromStdString(filePath));
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        Logger::error("Failed to open skin file: " + filePath);
        return false;
    }
    QByteArray data = file.readAll();
    file.close();

    return parseXmlString(data.toStdString(), rootOut);
}

bool SkinParser::parseXmlString(const std::string& xmlContent, std::shared_ptr<SkinElementNode>& rootOut) {
    std::string processed = preprocessMacros(xmlContent);
    // Wrap with root tag to ensure valid XML stream
    std::string wrappedXml = "<SkinRoot>" + processed + "</SkinRoot>";

    QXmlStreamReader xml(QString::fromUtf8(wrappedXml.c_str()));
    std::vector<std::shared_ptr<SkinElementNode>> nodeStack;

    rootOut = std::make_shared<SkinElementNode>();
    rootOut->tag = "SkinRoot";
    nodeStack.push_back(rootOut);

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();
            if (name == "SkinRoot") continue;

            auto node = std::make_shared<SkinElementNode>();
            node->tag = name.toStdString();
            node->type = parseType(name);

            QXmlStreamAttributes attrs = xml.attributes();
            if (attrs.hasAttribute("CmdID")) node->cmdId = attrs.value("CmdID").toString().toStdString();
            if (attrs.hasAttribute("Tooltip")) node->tooltip = attrs.value("Tooltip").toString().toStdString();
            if (attrs.hasAttribute("Align")) node->align = attrs.value("Align").toString().toStdString();
            if (attrs.hasAttribute("Text")) node->text = attrs.value("Text").toString().toStdString();
            if (attrs.hasAttribute("FontName")) node->fontName = attrs.value("FontName").toString().toStdString();
            if (attrs.hasAttribute("FontSize")) node->fontSize = attrs.value("FontSize").toInt();

            if (attrs.hasAttribute("Width")) node->width = attrs.value("Width").toInt();
            if (attrs.hasAttribute("Height")) node->height = attrs.value("Height").toInt();
            if (attrs.hasAttribute("LeftMargin")) node->leftMargin = attrs.value("LeftMargin").toInt();
            if (attrs.hasAttribute("RightMargin")) node->rightMargin = attrs.value("RightMargin").toInt();
            if (attrs.hasAttribute("TopMargin")) node->topMargin = attrs.value("TopMargin").toInt();
            if (attrs.hasAttribute("BottomMargin")) node->bottomMargin = attrs.value("BottomMargin").toInt();

            if (attrs.hasAttribute("ImageLeft")) node->imageLeft = attrs.value("ImageLeft").toInt();
            if (attrs.hasAttribute("ImageTop")) node->imageTop = attrs.value("ImageTop").toInt();
            if (attrs.hasAttribute("ImageWidth")) node->imageWidth = attrs.value("ImageWidth").toInt();
            if (attrs.hasAttribute("ImageHeight")) node->imageHeight = attrs.value("ImageHeight").toInt();

            if (attrs.hasAttribute("AltImageLeft")) node->altImageLeft = attrs.value("AltImageLeft").toInt();
            if (attrs.hasAttribute("AltImageTop")) node->altImageTop = attrs.value("AltImageTop").toInt();

            if (attrs.hasAttribute("BackImageLeft")) node->backImageLeft = attrs.value("BackImageLeft").toInt();
            if (attrs.hasAttribute("BackImageTop")) node->backImageTop = attrs.value("BackImageTop").toInt();

            if (attrs.hasAttribute("ThumbImageLeft")) node->thumbImageLeft = attrs.value("ThumbImageLeft").toInt();
            if (attrs.hasAttribute("ThumbImageTop")) node->thumbImageTop = attrs.value("ThumbImageTop").toInt();
            if (attrs.hasAttribute("ThumbWidth")) node->thumbWidth = attrs.value("ThumbWidth").toInt();
            if (attrs.hasAttribute("ThumbHeight")) node->thumbHeight = attrs.value("ThumbHeight").toInt();

            if (attrs.hasAttribute("IsTitleBar")) node->isTitleBar = (attrs.value("IsTitleBar").toInt() == 1);
            if (attrs.hasAttribute("AutoSize")) node->autoSize = (attrs.value("AutoSize").toInt() == 1);
            if (attrs.hasAttribute("AutoVisible")) node->autoVisible = true;

            nodeStack.back()->children.push_back(node);
            nodeStack.push_back(node);
        } else if (token == QXmlStreamReader::EndElement) {
            QString name = xml.name().toString();
            if (name == "SkinRoot") continue;
            if (!nodeStack.empty()) {
                nodeStack.pop_back();
            }
        }
    }

    if (xml.hasError()) {
        Logger::error("XML parsing error in skin: " + xml.errorString().toStdString());
        return false;
    }

    return true;
}

} // namespace Jonaki
