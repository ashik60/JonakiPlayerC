#include <QTest>
#include "ui/SkinParser.h"

using namespace Jonaki;

class TestSkinParser : public QObject {
    Q_OBJECT

private slots:
    void testMacroPreprocessor() {
        SkinParser parser;
        parser.setMacro("SkinName", "VideoSkin");
        parser.setMacro("CodePage", "1252");

        std::string sample = R"(
            #if<CodePage==949>
                <Button CmdID='KOREAN_BTN' />
            #else
                <Button CmdID='ENGLISH_BTN' />
            #endif
        )";

        std::shared_ptr<SkinElementNode> root;
        QVERIFY(parser.parseXmlString(sample, root));
        QCOMPARE(root->children.size(), static_cast<size_t>(1));
        QCOMPARE(root->children[0]->cmdId, std::string("ENGLISH_BTN"));
    }

    void testSkinDirectives() {
        SkinParser parser;
        std::string sample = R"(
            <Static Align='Top' Height='31' IsTitleBar='1' ImageLeft='3' ImageTop='490' ImageWidth='347' ImageHeight='31'>
                <Button CmdID='CMD_MAIN_MENU' Align='Left' Width='89' Height='30' ImageLeft='0' ImageTop='43' />
                <Slider CmdID='CMD_VOLUME_CTRL' Align='Right' Width='70' Height='10' ThumbWidth='11' ThumbHeight='10' />
                <Label CmdID='CMD_CURRENTTIME_MESSAGE' Align='Left' Text='00:00' FontSize='11' />
            </Static>
        )";

        std::shared_ptr<SkinElementNode> root;
        QVERIFY(parser.parseXmlString(sample, root));
        QCOMPARE(root->children.size(), static_cast<size_t>(1));

        auto staticNode = root->children[0];
        QCOMPARE(staticNode->type, SkinElementType::Static);
        QCOMPARE(staticNode->height, 31);
        QCOMPARE(staticNode->isTitleBar, true);
        QCOMPARE(staticNode->children.size(), static_cast<size_t>(3));

        auto btn = staticNode->children[0];
        QCOMPARE(btn->type, SkinElementType::Button);
        QCOMPARE(btn->cmdId, std::string("CMD_MAIN_MENU"));
        QCOMPARE(btn->width, 89);

        auto slider = staticNode->children[1];
        QCOMPARE(slider->type, SkinElementType::Slider);
        QCOMPARE(slider->cmdId, std::string("CMD_VOLUME_CTRL"));
        QCOMPARE(slider->thumbWidth, 11);

        auto label = staticNode->children[2];
        QCOMPARE(label->type, SkinElementType::Label);
        QCOMPARE(label->text, std::string("00:00"));
    }

    void testTopCommonFile() {
        SkinParser parser;
        std::shared_ptr<SkinElementNode> root;
        QString path = "resources/reference/skin_unpacked/TopCommon.txt";
        if (!QFile::exists(path)) {
            path = "F:/Projects/JonakiPlayer/resources/reference/skin_unpacked/TopCommon.txt";
        }
        QVERIFY(parser.parseFile(path.toStdString(), root));
        QVERIFY(root->children.size() > 0);
        QCOMPARE(root->children[0]->height, 31);
        QCOMPARE(root->children[0]->isTitleBar, true);
    }
};

QTEST_MAIN(TestSkinParser)
#include "test_skin_parser.moc"
