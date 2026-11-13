#include "LevelSettingsLayer.h"
#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIScale9Sprite.h"
#include "FontHelper.h"
#include "ui/UIWidget.h"
#include "ui/UIButton.h"
#include "ChangeColorPopup.h"

bool LevelSettingsLayer::init()
{
    if (!Node::init()) return false;

    this->panel = ax::ui::Scale9Sprite::createWithSpriteFrameName("content_panel.png");
    panel->setCapInsets(ax::Rect(3, 3, 138, 154));
    panel->setContentSize(ax::Size(550.0F, 340.0F));
    panel->setPosition(ax::Vec2::ZERO);
    this->addChild(panel, -10);

    this->initBGChangeButton();
    this->initSongsSelector();

    auto ok = ax::ui::Button::create("large_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    ok->setPosition(ax::Vec2(this->panel->getContentSize().width / 2.0F, 35.0F));
    ok->setPressedActionEnabled(true);
    ok->setScale(2.5F);
    ok->addTouchEventListener([this](ax::Ref* sender, ax::ui::Widget::TouchEventType type) {
        if (type == ax::ui::Widget::TouchEventType::ENDED)
        {
            auto btn = static_cast<ax::ui::Button*>(sender);
            this->removeFromParent();
            if (this->callback) this->callback(this->selectedSongId, ax::Color3B(this->b_r, this->b_g, this->b_b));
        }
    });
    auto oklabel = ax::Label::createWithTTF("OK", "fonts/JUA.ttf", getFontScaleWithSample(30.0F));
    oklabel->setScale(FONT_DIV_SAMPLE * 0.4F);
    oklabel->enableOutline(ax::Color4B::BLACK, 1);
    auto btnSize = ok->getContentSize();
    oklabel->setPosition(btnSize.width * 0.5F, btnSize.height * 0.5F);
    ok->addChild(oklabel);

    this->panel->addChild(ok, 10);

    return true;
}

void LevelSettingsLayer::initBGChangeButton()
{

    auto label = ax::Label::createWithTTF("Background color", "fonts/JUA.ttf", getFontScaleWithSample(30.0F));
    label->setScale(FONT_DIV_SAMPLE);
    label->setAnchorPoint(ax::Vec2(0.5F, 0.0F));
    label->setPosition((this->panel->getContentSize().width / 2.0F), 125.0F);

    float previewSize = 50.0f;

    auto preview = ax::ui::Layout::create();
    preview->setContentSize(ax::Size(previewSize, previewSize));
    preview->setBackGroundColorType(ax::ui::Layout::BackGroundColorType::SOLID);
    preview->setBackGroundColor(ax::Color3B(this->b_r, this->b_g, this->b_b));
    preview->setBackGroundColorOpacity(255);
    preview->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE);
    preview->setPosition(
        ax::Vec2(((this->panel->getContentSize().width / 2.0F) - (label->getPositionX() / 2.0F)) - previewSize * 0.5f,
                 label->getPositionY() + previewSize * 0.5f));

    preview->setTouchEnabled(true);
    preview->addTouchEventListener([this](ax::Object* sender, ax::ui::Widget::TouchEventType type) {
        auto widget               = static_cast<ax::ui::Widget*>(sender);
        constexpr float zoomScale = 1.1f;

        switch (type)
        {
        case ax::ui::Widget::TouchEventType::BEGAN:
            widget->stopAllActions();
            widget->runAction(ax::ScaleTo::create(0.05f, zoomScale));
            break;

        case ax::ui::Widget::TouchEventType::ENDED: {
            widget->stopAllActions();
            widget->runAction(ax::ScaleTo::create(0.05f, 1.0f));
            auto popup = ChangeColorPopup::createWithCallback(
                [this](int r, int g, int b) -> void { this->setBackgroundColor(ax::Color3B(r, g, b)); });
            popup->setPosition(this->panel->getContentSize().width / 2, this->panel->getContentSize().height / 2);
            popup->setChangeColors(ax::Color3B(this->b_r, this->b_g, this->b_b));
            this->panel->addChild(popup, 50);
            AXLOGD("Preview clicked!");
            break;
        }
        case ax::ui::Widget::TouchEventType::CANCELED:
            widget->stopAllActions();
            widget->runAction(ax::ScaleTo::create(0.05f, 1.0f));
            break;

        default: break;
        }
    });

    this->preview = preview;

    this->panel->addChild(this->preview, 10);
    this->panel->addChild(label, 10);
}

void LevelSettingsLayer::initSongsSelector()
{

    auto drawNode = ax::DrawNode::create();

    float radius = 16.0F;
    int seg      = 8;
    std::vector<ax::Vec2> vertices;
    vertices.reserve((seg + 1) * 4);

    ax::Vec2 origin(0, 0);
    ax::Vec2 dest(400, 125);

    const ax::Vec2 centers[4] = {{dest.x - radius, dest.y - radius},
                                 {origin.x + radius, dest.y - radius},
                                 {origin.x + radius, origin.y + radius},
                                 {dest.x - radius, origin.y + radius}};

    for (int i = 0; i < 4; i++)
    {
        float sAngle = i * M_PI_2;
        for (int j = 0; j <= seg; ++j)
        {
            float angle = sAngle + (M_PI_2 * j / seg);
            vertices.emplace_back(centers[i].x + (std::cos(angle) * radius), centers[i].y + (std::sin(angle) * radius));
        }
    }

    drawNode->drawSolidPoly(vertices.data(), static_cast<unsigned int>(vertices.size()),
                            ax::Color4F(0.0F, 0.0F, 0.0F, 0.3F));
    drawNode->setContentSize(ax::Vec2(400, 125));
    drawNode->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE);
    drawNode->setPosition(this->panel->getContentSize().width / 2, 260);
    this->panel->addChild(drawNode, 10);

    std::string songText = getSongNameWithNumber(this->selectedSongId);
    this->songName       = ax::Label::createWithTTF(songText, "fonts/JUA.ttf", getFontScaleWithSample(36.0F));
    this->songName->setScale(FONT_DIV_SAMPLE);
    this->songName->setPosition(this->panel->getContentSize().width / 2, drawNode->getPositionY());
    this->panel->addChild(this->songName, 15);

    auto rigthBtn = ax::ui::Button::create("arrow_right.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    rigthBtn->setPosition(ax::Vec2((this->panel->getContentSize().width / 2) + (drawNode->getContentSize().width / 2),
                                   drawNode->getPositionY()));
    rigthBtn->setScale(5.0F, 4.0F);
    rigthBtn->setTag(1);

    auto leftBtn = ax::ui::Button::create("arrow_right.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    leftBtn->setPosition(ax::Vec2((this->panel->getContentSize().width / 2) - (drawNode->getContentSize().width / 2),
                                  drawNode->getPositionY()));
    leftBtn->setScale(-5.0F, 4.0F);
    leftBtn->setTag(-1);

    rigthBtn->setPressedActionEnabled(true);
    leftBtn->setPressedActionEnabled(true);
    rigthBtn->addTouchEventListener(AX_CALLBACK_2(LevelSettingsLayer::onChangeSongBtnTouched, this));
    leftBtn->addTouchEventListener(AX_CALLBACK_2(LevelSettingsLayer::onChangeSongBtnTouched, this));

    this->panel->addChild(rigthBtn, 10);
    this->panel->addChild(leftBtn, 10);
}

void LevelSettingsLayer::onChangeSongBtnTouched(ax::Ref* sender, ax::ui::Widget::TouchEventType type)
{
    if (type == ax::ui::Widget::TouchEventType::ENDED)
    {
        auto btn = static_cast<ax::ui::Button*>(sender);
        int ch   = btn->getTag();

        this->selectedSongId = std::max(1, std::min(this->selectedSongId + ch, MAX_SONG_ID));
        this->songName->setString(getSongNameWithNumber(this->selectedSongId));
    }
}

void LevelSettingsLayer::setBackgroundColor(const ax::Color3B& color)
{
    this->b_r = color.r;
    this->b_g = color.g;
    this->b_b = color.b;

    this->preview->setBackGroundColor(color);
}