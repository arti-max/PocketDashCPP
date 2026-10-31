#include "GameObject.h"
#include "ObjectChunk.h"

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
    // id,x,y,rot;

    size_t p1 = objString.find(',');
    if (p1 == std::string_view::npos) return nullptr;

    size_t p2 = objString.find(',', p1 + 1);
    if (p2 == std::string_view::npos) return nullptr;

    size_t p3 = objString.find(',', p2 + 1);
    if (p3 == std::string_view::npos) return nullptr;

    auto idStr   = objString.substr(0, p1);
    auto posXStr = objString.substr(p1 + 1, p2 - p1 - 1);
    auto posYStr = objString.substr(p2 + 1, p3 - p2 - 1);
    auto rotStr  = objString.substr(p3 + 1);

    int typeId = 0;
    int objX   = 0;
    int objY   = 0;
    float rot  = 0.0F;

    std::from_chars(idStr.data(), idStr.data() + idStr.size(), typeId);
    std::from_chars(posXStr.data(), posXStr.data() + posXStr.size(), objX);
    std::from_chars(posYStr.data(), posYStr.data() + posYStr.size(), objY);

    rot = std::stof(std::string(rotStr));

    auto* obj = GameObject::createFromType(static_cast<GameObjectType>(typeId));
    if (obj)
    {
        obj->setRotation(rot);
        obj->setPosition(static_cast<float>(objX), static_cast<float>(objY));
    }
    return obj;
}

bool GameObject::initWithType(GameObjectType type)
{

    std::string_view frameName;
    switch (type)
    {
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

    if (!Sprite::initWithSpriteFrameName(frameName))
    {
        return false;
    }

    this->type = type;
    this->setScale(SPRITE_SCALE);

    this->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE);
    // if (type == GameObjectType::PortalY || type == GameObjectType::PortalB)
    //     this->setAnchorPoint(ax::Vec2(0.0F, 1.0F / 3.0F));

    if (type == GameObjectType::Trigger) this->isTrigger = true;

    return true;
}

ax::Rect GameObject::getAABB() const
{
    return ax::Rect::ZERO;
}

std::string GameObject::getObjectString()
{
    std::string blockStr;
    ObjectChunk* objChunk = static_cast<ObjectChunk*>(this->chunk);

    blockStr += std::to_string(static_cast<int>(this->type));
    blockStr += ',';
    blockStr += std::to_string(static_cast<int>(
        (objChunk->getChunkX() * ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE) + this->getPositionX()));
    blockStr += ',';
    blockStr += std::to_string(static_cast<int>(
        (objChunk->getChunkY() * ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE) + this->getPositionY()));
    blockStr += ',';
    blockStr += std::to_string(static_cast<int>(std::floor(this->getRotation())));
    if (this->isTrigger)
    {
        blockStr += "5,0,255,0";  // fadetime,r,g,b (fade time in ms - 0.fadetime 100 = 1, 5 = 0.05, 50 = 0.5)
    }
    blockStr += ';';

    return blockStr;
}