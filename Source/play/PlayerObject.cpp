#include "PlayerObject.h"
#include "PlayScene.h"

PlayerObject* PlayerObject::create(int skinId)
{
    auto* obj = new (std::nothrow) PlayerObject();
    if (obj && obj->initWitchSkinId(skinId))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool PlayerObject::initWitchSkinId(int skinId)
{
    if (!GameObject::init())
    {
        return false;
    }

    this->skin0  = ax::Sprite::createWithSpriteFrameName(std::format("player_0_0{}.png", skinId));
    this->skin1  = ax::Sprite::createWithSpriteFrameName(std::format("player_1_0{}.png", skinId));
    this->color1 = ax::Color3B::GREEN;
    this->color2 = ax::Color3B(0, 120, 230);
    this->skin0->setColor(this->color1);
    this->skin1->setColor(this->color2);
    this->addChild(this->skin0);
    this->addChild(this->skin1);
    this->setScale(2.5F);

    return true;
}

void PlayerObject::update(float dt)
{
    float nx = this->getPositionX();
    float ny = this->getPositionY();
    if (!isOnGround())
    {
        ny += vy;
        if (ny < PlayScene::MIN_GROUND_Y)
        {
            ny             = PlayScene::MIN_GROUND_Y;
            this->onGround = true;
        }
    }

    this->setPosition(nx, ny);
}

void PlayerObject::popTouch() {}
void PlayerObject::pushTouch() {}
void PlayerObject::hitGround() {}
void PlayerObject::jump() {}