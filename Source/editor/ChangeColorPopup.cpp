#include "ChangeColorPopup.h"
#include "cocos2d.h"
#include "ui/UIScale9Sprite.h"
#include "ui/UISlider.h"
#include "ui/UIButton.h"
#include "fmt/format.h"
#include "FontHelper.h"

ChangeColorPopup* ChangeColorPopup::createWithCallback(const TriggerCallback& callback)
{
    auto* obj = new (std::nothrow) ChangeColorPopup();
    if (obj && obj->init(callback))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

ChangeColorPopup* ChangeColorPopup::createWithCallbackFade(const TriggerCallbackFade& callback)
{
    auto* obj = new (std::nothrow) ChangeColorPopup();
    if (obj && obj->initWithFade(callback))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool ChangeColorPopup::init(const TriggerCallback& callback)
{
    this->type     = CallbackType::INSTANT;
    this->callback = callback;
    return this->init();
}

bool ChangeColorPopup::initWithFade(const TriggerCallbackFade& callback)
{
    this->type         = CallbackType::FADE;
    this->fadeCallback = callback;
    return this->init();
}

bool ChangeColorPopup::init()
{
    if (!Node::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();

    this->panel = ax::ui::Scale9Sprite::createWithSpriteFrameName("content_panel.png");
    panel->setCapInsets(ax::Rect(3, 3, 138, 154));
    panel->setContentSize(ax::Size(400.0F, 350.0F));
    panel->setPosition(ax::Vec2::ZERO);
    this->addChild(panel, -10);

    this->createSliders();

    this->dataText = fmt::format("r: {}, g: {}, b: {}", this->c_r, this->c_g, this->c_b);
    this->label    = ax::Label::createWithTTF(this->dataText, "fonts/JUA.ttf", getFontScaleWithSample(24.0F));
    this->label->setScale(FONT_DIV_SAMPLE);
    this->label->setPosition(std::round(this->panel->getContentSize().width / 2.0F), std::round(330.0F));

    auto ok = ax::ui::Button::create("medium_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    ok->setPosition(ax::Vec2(this->panel->getContentSize().width / 2.0F, 35.0F));
    ok->setPressedActionEnabled(true);
    ok->setScale(2.5F);
    ok->addTouchEventListener([this](ax::Ref* sender, ax::ui::Widget::TouchEventType type) {
        if (type == ax::ui::Widget::TouchEventType::ENDED)
        {
            auto btn = static_cast<ax::ui::Button*>(sender);
            this->save();
        }
    });
    auto oklabel = ax::Label::createWithTTF("OK", "fonts/JUA.ttf", 30.0F);
    oklabel->setScale(0.4F);
    oklabel->enableOutline(ax::Color4B::BLACK, 1);
    auto btnSize = ok->getContentSize();
    oklabel->setPosition(btnSize.width * 0.5F, btnSize.height * 0.5F);
    ok->addChild(oklabel);

    float previewSize = 50;
    this->preview = ax::LayerColor::create(ax::Color4B(this->c_r, this->c_g, this->c_b, 255), previewSize, previewSize);
    this->preview->setPosition((this->panel->getContentSize().width / 2.0F) - (previewSize / 2), 250.0F);

    this->panel->addChild(this->preview, 10);
    this->panel->addChild(this->label, 10);
    this->panel->addChild(ok, 10);
    return true;
}

void ChangeColorPopup::createSliders()
{

    this->rSlider = ax::ui::Slider::create();
    rSlider->setScale9Enabled(true);

    rSlider->loadBarTexture("slider_line_red_off.png", ax::ui::Widget::TextureResType::PLIST);
    rSlider->loadProgressBarTexture("slider_line_red.png", ax::ui::Widget::TextureResType::PLIST);
    rSlider->loadSlidBallTextures("slider_ico_red.png", "slider_ico_red.png", "",
                                  ax::ui::Widget::TextureResType::PLIST);

    rSlider->setPercent((this->c_r * 100 / 255));
    rSlider->setPosition(ax::Vec2(this->panel->getContentSize().width / 2.0F, 220.0F));
    rSlider->setScale(4.0F);
    rSlider->setContentSize(ax::Size(60.0F, 16.0F));

    this->gSlider = ax::ui::Slider::create();
    gSlider->setScale9Enabled(true);

    gSlider->loadBarTexture("slider_line_green_off.png", ax::ui::Widget::TextureResType::PLIST);
    gSlider->loadProgressBarTexture("slider_line_green.png", ax::ui::Widget::TextureResType::PLIST);
    gSlider->loadSlidBallTextures("slider_ico_green.png", "slider_ico_green.png", "",
                                  ax::ui::Widget::TextureResType::PLIST);

    gSlider->setPercent((this->c_g * 100 / 255));
    gSlider->setPosition(ax::Vec2(this->panel->getContentSize().width / 2.0F, 180.0F));
    gSlider->setScale(4.0F);
    gSlider->setContentSize(ax::Size(60.0F, 16.0F));

    this->bSlider = ax::ui::Slider::create();
    bSlider->setScale9Enabled(true);

    bSlider->loadBarTexture("slider_line_blue_off.png", ax::ui::Widget::TextureResType::PLIST);
    bSlider->loadProgressBarTexture("slider_line_blue.png", ax::ui::Widget::TextureResType::PLIST);
    bSlider->loadSlidBallTextures("slider_ico_blue.png", "slider_ico_blue.png", "",
                                  ax::ui::Widget::TextureResType::PLIST);

    bSlider->setPercent((this->c_g * 100 / 255));
    bSlider->setPosition(ax::Vec2(this->panel->getContentSize().width / 2.0F, 140.0F));
    bSlider->setScale(4.0F);
    bSlider->setContentSize(ax::Size(60.0F, 16.0F));

    if (this->type == CallbackType::FADE)
    {
        this->fadeSlider = ax::ui::Slider::create();
        fadeSlider->setScale9Enabled(true);

        fadeSlider->loadBarTexture("slider_line_base_off.png", ax::ui::Widget::TextureResType::PLIST);
        fadeSlider->loadProgressBarTexture("slider_line_base.png", ax::ui::Widget::TextureResType::PLIST);
        fadeSlider->loadSlidBallTextures("slider_ico_base.png", "slider_ico_base.png", "",
                                         ax::ui::Widget::TextureResType::PLIST);

        fadeSlider->setPercent(std::round(this->fade * 100 / 10));
        fadeSlider->setPosition(ax::Vec2(this->panel->getContentSize().width / 2.0F, 80.0F));
        fadeSlider->setScale(4.0F);
        fadeSlider->setContentSize(ax::Size(60.0F, 16.0F));

        auto sliderLabel = ax::Label::createWithTTF(fmt::format("fade: {:.1f}s", this->fade), "fonts/JUA.ttf",
                                                    getFontScaleWithSample(24.0F));
        sliderLabel->setScale(FONT_DIV_SAMPLE);
        sliderLabel->setPosition(this->panel->getContentSize().width / 2.0F, 110.0F);

        fadeSlider->addEventListener([this, sliderLabel](ax::Ref* sender, ax::ui::Slider::EventType type) {
            if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
            {
                auto* s     = static_cast<ax::ui::Slider*>(sender);
                int percent = s->getPercent();

                this->fade = (static_cast<float>(percent) * 10.0F) / 100.0F;

                sliderLabel->setString(fmt::format("fade: {:.1f}s", this->fade));
            }
        });

        this->panel->addChild(fadeSlider, 10);
        this->panel->addChild(sliderLabel, 10);
    }

    rSlider->addEventListener([this](ax::Ref* sender, ax::ui::Slider::EventType type) {
        if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
        {
            auto* s     = static_cast<ax::ui::Slider*>(sender);
            int percent = s->getPercent();

            this->c_r = (percent * 255) / 100;
            this->updatePreview();
        }
    });

    gSlider->addEventListener([this](ax::Ref* sender, ax::ui::Slider::EventType type) {
        if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
        {
            auto* s     = static_cast<ax::ui::Slider*>(sender);
            int percent = s->getPercent();

            this->c_g = (percent * 255) / 100;
            this->updatePreview();
        }
    });

    bSlider->addEventListener([this](ax::Ref* sender, ax::ui::Slider::EventType type) {
        if (type == ax::ui::Slider::EventType::ON_PERCENTAGE_CHANGED)
        {
            auto* s     = static_cast<ax::ui::Slider*>(sender);
            int percent = s->getPercent();

            this->c_b = (percent * 255) / 100;
            this->updatePreview();
        }
    });

    this->panel->addChild(rSlider, 10);
    this->panel->addChild(gSlider, 10);
    this->panel->addChild(bSlider, 10);
}

void ChangeColorPopup::updatePreview()
{
    this->dataText = fmt::format("r: {}, g: {}, b: {}", this->c_r, this->c_g, this->c_b);
    this->label->setString(this->dataText);
    this->preview->setColor(ax::Color3B(this->c_r, this->c_g, this->c_b));
}

void ChangeColorPopup::save()
{
    if (this->type == CallbackType::INSTANT)
    {
        this->callback(this->c_r, this->c_g, this->c_b);
    }
    else
    {
        this->fadeCallback(this->fade, this->c_r, this->c_g, this->c_b);
    }

    this->removeFromParent();
}

void ChangeColorPopup::setChangeColors(const ax::Color3B& color)
{
    this->c_r = color.r;
    this->c_g = color.g;
    this->c_b = color.b;
    this->updatePreview();

    this->rSlider->setPercent((this->c_r * 100 / 255));
    this->gSlider->setPercent((this->c_g * 100 / 255));
    this->bSlider->setPercent((this->c_b * 100 / 255));
    if (this->fadeSlider != nullptr) this->fadeSlider->setPercent(std::round(this->fade * 100 / 10));
}