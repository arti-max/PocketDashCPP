#include "BlockSelectLayer.h"
#include "ui/UIButton.h"

bool BlockSelectLayer::init()
{
    if (!Node::init())
    {
        return false;
    }

    ax::Vec2 visibleSize = _director->getVisibleSize();
    ax::Vec2 origin      = _director->getVisibleOrigin();

    float panelWidth  = visibleSize.width - 180.0F;
    float panelHeight = 120.0F;

    auto rect = ax::LayerColor::create(ax::Color4B(0, 0, 0, 120), panelWidth, panelHeight);
    rect->setPosition(origin.x + ((visibleSize.width - panelWidth) * 0.5F), origin.y + 10.0F);
    this->addChild(rect, -10);

    this->initObjectButtons();
    this->initModeButtons();

    return true;
}

void BlockSelectLayer::initObjectButtons()
{

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    float baseXOffset = (origin.x + ((visibleSize.width - (visibleSize.width - 180.0F)) * 0.5F)) + 110.0F;
    float xOffset     = baseXOffset;
    for (int i = 0; i < BLOCK_COUNT; ++i)
    {
        std::string_view frameName;
        // clang-format off
        switch (static_cast<GameObjectType>(i)) {
            case GameObjectType::Block: frameName = "block_01.png"; break;
            case GameObjectType::Spike: frameName = "spike_01.png"; break;
            case GameObjectType::Orb: frameName = "orb_01.png"; break;
            case GameObjectType::Pad: frameName = "pad_01.png"; break;
            case GameObjectType::PortalB: frameName = "portal_01.png"; break;
            case GameObjectType::PortalY: frameName = "portal_02.png"; break;
            case GameObjectType::ChainLarge0: frameName = "chain_large_01.png"; break;
            case GameObjectType::ChainLarge1: frameName = "chain_large_02.png"; break;
            case GameObjectType::Trigger: frameName = "trigger_01.png"; break;
            default: frameName = "player_0_01.png"; break;
        }
        // clang-format on
        if (i == 6)
        {
            xOffset = baseXOffset;
        }
        auto btn = ax::ui::Button::create("dark_red_small_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
        btn->setPosition(ax::Vec2(baseXOffset + xOffset, origin.y + ((i < 6) ? 100.0F : 40.0F)));
        xOffset += 85.0F;

        btn->setPressedActionEnabled(true);
        btn->setScale(3.0F);

        auto icon = ax::Sprite::createWithSpriteFrameName(frameName);
        icon->setName("btn_icon");
        auto btnSize = btn->getContentSize();
        icon->setPosition(btnSize.width * 0.5F, btnSize.height * 0.5F);
        icon->setScale(0.5F);
        btn->addChild(icon);
        btn->setTag(i);

        btn->addTouchEventListener(AX_CALLBACK_2(BlockSelectLayer::onBlockButtonClicked, this));

        this->addChild(btn);
    }
}

void BlockSelectLayer::initModeButtons()
{
    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    float baseXOffset = (origin.x + ((visibleSize.width - (visibleSize.width - 180.0F)) * 0.5F)) - 10.0F;
    float xOffset     = baseXOffset;

    for (int i = 0; i < MODE_COUNT; ++i)
    {
        std::string_view text;
        switch (static_cast<BuildMode>(i))
        {
        case BuildMode::BUILD: text = "Build"; break;
        case BuildMode::EDIT: text = "Edit"; break;
        case BuildMode::DELETE: text = "Delete"; break;
        case BuildMode::SWIPE: text = "Swipe"; break;
        default: text = "err"; break;
        }
        if (i == 2)
        {
            xOffset = baseXOffset;
        }
        auto btn = ax::ui::Button::create("medium_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
        btn->setPosition(ax::Vec2(baseXOffset + xOffset, origin.y + ((i < 2) ? 100.0F : 40.0F)));
        btn->setPressedActionEnabled(true);
        btn->setScale(2.5F);
        btn->addTouchEventListener(AX_CALLBACK_2(BlockSelectLayer::onModeButtonClicked, this));
        btn->setTag(i);
        this->addChild(btn);
        auto label = ax::Label::createWithTTF(text, "fonts/JUA.ttf", 30.0F);
        label->setScale(0.4F);
        label->enableOutline(ax::Color4B::BLACK, 1);
        auto btnSize = btn->getContentSize();
        label->setPosition(btnSize.width * 0.5F, btnSize.height * 0.5F);
        btn->addChild(label);
        xOffset += (btn->getContentSize().width * 2.5F) + 10.0F;
        if (static_cast<BuildMode>(i) == BuildMode::BUILD) this->selectButton(btn, 1);
    }
}

void BlockSelectLayer::onBlockButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type)
{
    if (type == ax::ui::Widget::TouchEventType::ENDED)
    {
        auto btn  = static_cast<ax::ui::Button*>(sender);
        int blkid = btn->getTag();

        this->selectButton(btn, 0);

        this->selected = static_cast<GameObjectType>(blkid);
    }
}

void BlockSelectLayer::onModeButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type)
{
    if (type == ax::ui::Widget::TouchEventType::ENDED)
    {
        auto btn = static_cast<ax::ui::Button*>(sender);
        int md   = btn->getTag();

        this->selectButton(btn, 1);

        if (static_cast<BuildMode>(md) == BuildMode::EDIT && this->activeModeButton != nullptr)
        {
            this->activeModeButton->setColor(ax::Color3B::WHITE);
            this->activeModeButton = nullptr;
            this->swipeMode        = false;
        }
        if (static_cast<BuildMode>(md) == BuildMode::SWIPE)
        {
            this->swipeMode = this->activeModeButton != nullptr;
        }
        else
        {
            this->mode = static_cast<BuildMode>(md);
        }
    }
}

void BlockSelectLayer::selectButton(ax::ui::Button* btn, int type)
{
    if (type == 0)
    {
        if (this->activeButton == btn) return;

        if (this->activeButton != nullptr)
        {
            this->activeButton->setColor(ax::Color3B::WHITE);
            this->activeButton->getChildByName("btn_icon")->setColor(ax::Color3B::WHITE);
        }

        this->activeButton = btn;
        if (this->activeButton != nullptr)
        {
            ax::Color3B col(180, 180, 180);
            this->activeButton->setColor(col);
            this->activeButton->getChildByName("btn_icon")->setColor(col);
        }
    }
    else if (type == 1)
    {
        BuildMode mode = static_cast<BuildMode>(btn->getTag());
        if (mode != BuildMode::SWIPE)
        {
            if (this->activeBuildButton == btn) return;

            if (this->activeBuildButton != nullptr) this->activeBuildButton->setColor(ax::Color3B::WHITE);

            this->activeBuildButton = btn;
            if (this->activeBuildButton != nullptr) this->activeBuildButton->setColor(ax::Color3B(180, 180, 180));
        }
        else
        {
            if (this->activeModeButton == btn)
            {
                this->activeModeButton->setColor(ax::Color3B::WHITE);
                this->activeModeButton = nullptr;
                return;
            }

            this->activeModeButton = btn;
            if (this->activeModeButton != nullptr) this->activeModeButton->setColor(ax::Color3B(180, 180, 180));
        }
    }
}