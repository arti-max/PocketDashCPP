#include "AlertPopup.h"
#include "ui/UIScale9Sprite.h"
#include "ui/UIButton.h"
#include "FontHelper.h"
#include "ui/UIWidget.h"
#include "utils/UIUtils.h"

AlertPopup* AlertPopup::create(std::string title, std::string desc, const AlertCallback& callback)
{
    auto* obj = new (std::nothrow) AlertPopup();
    if (obj && obj->init(title, desc, callback))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool AlertPopup::init(std::string title, std::string desc, const AlertCallback& callback)
{
    if (!Node::init()) return false;

    auto visibleSize               = _director->getVisibleSize();
    const float panel_normal_scale = 1.0F / 2.5F;
    ax::Size panelSize(visibleSize.width / 2.2, visibleSize.height / 3);

    auto titleLabel = ax::Label::createWithTTF(title, "fonts/JUA.ttf", getFontScaleWithSample(32.0F));
    titleLabel->setColor(ax::Color3B::YELLOW);
    titleLabel->setScale(FONT_DIV_SAMPLE * panel_normal_scale);

    auto descLabel = ax::Label::createWithTTF(desc, "fonts/JUA.ttf", getFontScaleWithSample(22.0F));
    descLabel->setScale(FONT_DIV_SAMPLE * panel_normal_scale);

    descLabel->setDimensions((panelSize.width - 10) / FONT_DIV_SAMPLE, 0.0F);
    descLabel->setHorizontalAlignment(ax::TextHAlignment::CENTER);
    descLabel->setVerticalAlignment(ax::TextVAlignment::TOP);
    descLabel->setAnchorPoint(ax::Vec2(0.5F, 1.0F));

    float titleH = titleLabel->getContentSize().height * FONT_DIV_SAMPLE * panel_normal_scale;
    float descH  = descLabel->getContentSize().height * FONT_DIV_SAMPLE * panel_normal_scale;

    float panelHeight = 12 + titleH + 8 + descH + 14 + 26;
    panelHeight       = std::max(panelHeight, (visibleSize.height / 3.2F) * panel_normal_scale);

    auto panel = ax::ui::Scale9Sprite::createWithSpriteFrameName("content_panel.png");
    panel->setCapInsets(ax::Rect(3, 3, 138, 154));
    panel->setContentSize(ax::Size(panelSize.width / 2.5F, panelHeight));
    panel->setScale(2.5F);
    panel->setPosition(ax::Vec2(visibleSize.width * 0.5, visibleSize.height * 0.5));
    this->addChild(panel, -10);

    titleLabel->setPosition(panel->getContentSize().width / 2, panel->getContentSize().height - 16);
    descLabel->setPosition(titleLabel->getPositionX(), titleLabel->getPositionY() - 16);

    auto okBtn = createTextButton("medium_btn.png", "OK", 2.5F, 26.0F, [this, callback]() {
        if (callback) callback();
        this->removeFromParent();
    });
    okBtn->setScale(2.5F * panel_normal_scale);
    okBtn->setPosition(ax::Vec2(panel->getContentSize().width / 2, 16));

    auto cancleBtn = ax::ui::Button::create("cancel_ico.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    cancleBtn->setPosition(
        ax::Vec2((cancleBtn->getContentSize().width / 2) + 2.0F, panel->getContentSize().height - 8));
    cancleBtn->setScale(1.2F * panel_normal_scale);
    cancleBtn->addClickEventListener([this](ax::Object* obj) -> void { this->removeFromParent(); });

    panel->addChild(cancleBtn, 10);
    panel->addChild(titleLabel, 10);
    panel->addChild(descLabel, 10);
    panel->addChild(okBtn, 10);
    return true;
}