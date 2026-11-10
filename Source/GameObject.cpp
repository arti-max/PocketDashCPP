#include "GameObject.h"
#include "ObjectChunk.h"
#include "play/PlayScene.h"

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

    size_t p1 = objString.find(',');  // id
    if (p1 == std::string_view::npos) return nullptr;

    size_t p2 = objString.find(',', p1 + 1);  // x
    if (p2 == std::string_view::npos) return nullptr;

    size_t p3 = objString.find(',', p2 + 1);  // y
    if (p3 == std::string_view::npos) return nullptr;

    int typeId = 0;

    auto idStr   = objString.substr(0, p1);
    auto posXStr = objString.substr(p1 + 1, p2 - p1 - 1);
    auto posYStr = objString.substr(p2 + 1, p3 - p2 - 1);

    std::from_chars(idStr.data(), idStr.data() + idStr.size(), typeId);
    GameObjectType type  = static_cast<GameObjectType>(typeId);
    bool process_trigger = false;
    std::basic_string_view<char> rotStr;
    std::basic_string_view<char> fadeStr;
    std::basic_string_view<char> rStr;
    std::basic_string_view<char> gStr;
    std::basic_string_view<char> bStr;

    if (type != GameObjectType::Trigger)
    {
        rotStr = objString.substr(p3 + 1);
    }
    else
    {
        process_trigger = true;

        size_t p4 = objString.find(',', p3 + 1);  // rot
        if (p4 == std::string_view::npos) return nullptr;

        size_t p5 = objString.find(',', p4 + 1);  // fade
        if (p5 == std::string_view::npos) return nullptr;

        size_t p6 = objString.find(',', p5 + 1);  // r
        if (p6 == std::string_view::npos) return nullptr;

        size_t p7 = objString.find(',', p6 + 1);  // g
        if (p7 == std::string_view::npos) return nullptr;

        rotStr  = objString.substr(p3 + 1, p4 - p3 - 1);
        fadeStr = objString.substr(p4 + 1, p5 - p4 - 1);
        rStr    = objString.substr(p5 + 1, p6 - p5 - 1);
        gStr    = objString.substr(p6 + 1, p7 - p6 - 1);
        bStr    = objString.substr(p7 + 1);
    }

    int objX        = 0;
    int objY        = 0;
    float rot       = 0.0F;
    int i_fade      = 0;
    float _fadeTime = 0.0F;
    int r           = 0;
    int g           = 0;
    int b           = 0;

    std::from_chars(posXStr.data(), posXStr.data() + posXStr.size(), objX);
    std::from_chars(posYStr.data(), posYStr.data() + posYStr.size(), objY);
    rot = std::stof(std::string(rotStr));
    if (process_trigger)
    {
        std::from_chars(fadeStr.data(), fadeStr.data() + fadeStr.size(), i_fade);
        std::from_chars(rStr.data(), rStr.data() + rStr.size(), r);
        std::from_chars(gStr.data(), gStr.data() + gStr.size(), g);
        std::from_chars(bStr.data(), bStr.data() + bStr.size(), b);
        _fadeTime = (float)i_fade * 0.1F;  // 5 = 0.5, 10 = 1.0
    }

    auto* obj = GameObject::createFromType(type);
    if (obj)
    {
        obj->setRotation(rot);
        obj->setPosition(static_cast<float>(objX), static_cast<float>(objY));
        if (process_trigger)
        {
            obj->setFadeTime(_fadeTime);
            obj->setTriggerR(r);
            obj->setTriggerG(g);
            obj->setTriggerB(b);
        }
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
    float w = OBJECT_SPRITE_SIZE * SPRITE_SCALE;
    float h = OBJECT_SPRITE_SIZE * SPRITE_SCALE;

    float worldPositionX = this->getPositionX();
    float worldPositionY = this->getPositionY();

    if (this->chunk)
    {
        const auto* objChunk = static_cast<const ObjectChunk*>(this->chunk);
        worldPositionX += static_cast<float>(objChunk->getChunkX() * ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE);
        worldPositionY += static_cast<float>(objChunk->getChunkY() * ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE);
    }

    float minX = worldPositionX - (w * 0.5f);
    float minY = worldPositionY - (h * 0.5f);

    if (this->type == GameObjectType::Spike)
    {
        int rot = static_cast<int>(std::round(this->getRotation())) % 360;
        if (rot < 0) rot += 360;

        float padSide   = 12.0f;
        float padTip    = 8.0f;
        float padBottom = 8.0F;

        if (rot == 0)  // up
        {
            return ax::Rect(minX + padSide, minY + padBottom, w - (padSide * 2.0f), h - padTip - padBottom);
        }
        else if (rot == 180)  // down
        {
            return ax::Rect(minX + padSide, minY + padTip, w - (padSide * 2.0f), h - padTip - padBottom);
        }
        else if (rot == 90)  // right
        {
            return ax::Rect(minX + padBottom, minY + padSide, w - padTip - padBottom, h - (padSide * 2.0f));
        }
        else if (rot == 270)  // left
        {
            return ax::Rect(minX + padTip, minY + padSide, w - padTip - padBottom, h - (padSide * 2.0f));
        }
    }

    return ax::Rect(minX, minY, w, h);
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
        blockStr += ',';
        blockStr += std::to_string(static_cast<int>(this->fadeTime * 10));
        blockStr += ',';
        blockStr += std::to_string(this->c_r);
        blockStr += ',';
        blockStr += std::to_string(this->c_g);
        blockStr += ',';
        blockStr += std::to_string(this->c_b);
        // blockStr += ",5,0,255,0";  // fadetime,r,g,b (fade time in ms - 0.fadetime 100 = 1, 5 = 0.05, 50 = 0.5)
    }
    blockStr += ';';

    return blockStr;
}

void GameObject::setVisualLayer(VisualLayer layer)
{
    this->setGlobalZOrder(static_cast<float>(layer));
}

float GameObject::getWorldPositionX()
{

    float worldX = this->getPositionX();

    if (this->chunk)
    {
        const auto* objChunk = static_cast<const ObjectChunk*>(this->chunk);
        worldX += (static_cast<float>(objChunk->getChunkX()) * ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE);
    }

    return worldX;
}