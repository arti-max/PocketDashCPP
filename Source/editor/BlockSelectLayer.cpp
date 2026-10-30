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
    rect->setPosition(origin.x + (visibleSize.width - panelWidth) * 0.5F, origin.y + 10.0F);
    this->addChild(rect, -10);

    this->initObjectButtons();

    return true;
}

void BlockSelectLayer::initObjectButtons()
{

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

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
        float baseXOffset = origin.x + (visibleSize.width - (visibleSize.width - 180.0F)) * 0.5F;
        auto btn = ax::ui::Button::create("dark_red_small_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
        btn->setPosition(ax::Vec2(baseXOffset + i * 60.0F, origin.y + 120.0F));

        btn->setPressedActionEnabled(true);
        btn->setScale(3.0F);

        auto icon    = ax::Sprite::createWithSpriteFrameName(frameName);
        auto btnSize = btn->getContentSize();
        icon->setPosition(btnSize.width * 0.5F, btnSize.height * 0.5F);
        icon->setScale(0.5F);
        btn->addChild(icon);
        btn->setTag(i);

        btn->addTouchEventListener(AX_CALLBACK_2(BlockSelectLayer::onBlockButtonClicked, this));

        this->addChild(btn);
    }
}

void BlockSelectLayer::onBlockButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type)
{
    if (type == ax::ui::Widget::TouchEventType::ENDED)
    {
        auto btn  = static_cast<ax::ui::Button*>(sender);
        int blkid = btn->getTag();

        this->selected = static_cast<GameObjectType>(blkid);
    }
}