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
    if (isInAir())
    {
        this->vy += GRAVITY * dt;
    }

    float nx = this->getWorldPositionX() + (this->vx * dt);
    float ny = this->getPositionY() + (this->vy * dt);
    if (ny < PlayScene::MIN_GROUND_Y)
    {
        ny             = PlayScene::MIN_GROUND_Y;
        this->vy       = 0.0F;
        this->onGround = true;
        this->snapRotation();
    }

    if (isInAir() && this->getActionByTag(ROTATE_ACTION_TAG) == nullptr)
    {
        auto rotateAction = ax::RotateBy::create(0.55F, 180.0F);
        rotateAction->setTag(ROTATE_ACTION_TAG);
        this->runAction(rotateAction);
    }

    if (isOnGround() && this->isHolding)
    {
        this->jump();
    }

    this->setPositionY(ny);
    this->worldX = nx;
}

void PlayerObject::snapRotation()
{
    if (this->getActionByTag(ROTATE_ACTION_TAG) != nullptr) this->stopActionByTag(ROTATE_ACTION_TAG);
    float currentRot = this->getRotation();
    int newRot       = static_cast<int>(std::round(currentRot / 90)) * 90;
    newRot           = ((newRot % 360) + 360) % 360;
    this->setRotation(static_cast<float>(newRot));
}

void PlayerObject::popTouch()
{
    this->isHolding = false;
}
void PlayerObject::pushTouch()
{
    this->isHolding = true;
    this->jump();
}
void PlayerObject::hitGround() {}
void PlayerObject::jump()
{
    this->vy       = JUMP_VELOCITY;
    this->onGround = false;
}