#include "GameObject.h"

GameObject* GameObject::create(int spriteId)
{
    return nullptr;
}
GameObject* GameObject::createFromType(GameObjectType type)
{
    auto* obj = new (std::nothrow) GameObject();
    if (obj && obj->initWithType(type))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}
GameObject* GameObject::createFromObj(std::string_view objString)
{
    return nullptr;
}

bool GameObject::initWithType(GameObjectType type)
{

    std::string_view frameName;
    switch (type)
    {
    case GameObjectType::Block:
        frameName = "block_01.png";
        break;
    case GameObjectType::Spike:
        frameName = "spike_01.png";
        break;
    case GameObjectType::Orb:
        frameName = "orb_01.png";
        break;
    case GameObjectType::Pad:
        frameName = "pad_01.png";
        break;
    case GameObjectType::PortalB:
        frameName = "portal_01.png";
        break;
    case GameObjectType::PortalY:
        frameName = "portal_02.png";
        break;
    case GameObjectType::ChainLarge0:
        frameName = "chain_large_01.png";
        break;
    case GameObjectType::ChainLarge1:
        frameName = "chain_large_02.png";
        break;
    case GameObjectType::Trigger:
        frameName = "trigger_01.png";
        break;
    default:
        frameName = "player_0_01.png";
        break;
    }

    if (!Sprite::initWithSpriteFrameName(frameName))
    {
        return false;
    }

    this->type = type;
    this->setScale(SPRITE_SCALE);

    this->setAnchorPoint(ax::Vec2::ZERO);

    return true;
}

ax::Rect GameObject::getAABB() const
{
    return ax::Rect::ZERO;
}

std::string GameObject::getObjectString()
{
    return "";
}