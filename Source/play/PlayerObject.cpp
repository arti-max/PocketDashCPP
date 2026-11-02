#include "PlayerObject.h"
#include "PlayScene.h"
#include <fmt/format.h>

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

    this->skin0  = ax::Sprite::createWithSpriteFrameName(fmt::format("player_0_0{}.png", skinId));
    this->skin1  = ax::Sprite::createWithSpriteFrameName(fmt::format("player_1_0{}.png", skinId));
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
    float ny = this->getPositionY() + ((this->vy * dt) * this->getGravityMul());

    if (ny <= PlayScene::MIN_GROUND_Y)
    {
        ny             = PlayScene::MIN_GROUND_Y;
        this->vy       = 0.0F;
        this->onGround = true;
        this->snapRotation();
    }

    if (isInAir() && this->getActionByTag(ROTATE_ACTION_TAG) == nullptr)
    {
        auto rotateAction = ax::RotateBy::create(0.4F, 180.0F);
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
void PlayerObject::hitGround()
{
    this->snapRotation();
    this->onGround = true;
    this->vy       = 0.0F;
}
void PlayerObject::jump()
{
    if (isInAir()) return;
    this->vy       = JUMP_VELOCITY;
    this->onGround = false;
}

int PlayerObject::getCellX() const
{
    return static_cast<int>(std::floor(this->worldX / ObjectChunk::CELL_SIZE));
}

int PlayerObject::getCellY() const
{
    return static_cast<int>(
        std::floor((this->getPositionY() - PlayScene::MIN_GROUND_Y_CONTENT) / ObjectChunk::CELL_SIZE));
}

ax::Rect PlayerObject::getAABB() const
{
    float w = OBJECT_SPRITE_SIZE * SPRITE_SCALE;
    float h = OBJECT_SPRITE_SIZE * SPRITE_SCALE;

    float cx = this->getWorldPositionX();
    float cy = this->getPositionY();

    return ax::Rect(cx - (w * 0.5F), cy - (h * 0.5F), w, h);
}

void PlayerObject::drop()
{
    if (this->getPositionY() > PlayScene::MIN_GROUND_Y && this->isOnGround())
    {
        this->onGround = false;
    }
}

void PlayerObject::collideWithObject(GameObject* obj)
{

    auto pAABB = this->getAABB();
    auto oAABB = obj->getAABB();

    float intersectsByX = std::min(pAABB.getMaxX(), oAABB.getMaxX()) - std::max(pAABB.getMinX(), oAABB.getMinX());
    float intersectsByY = std::min(pAABB.getMaxY(), oAABB.getMaxY()) - std::max(pAABB.getMinY(), oAABB.getMinY());

    if (intersectsByX <= 0.0F || intersectsByY <= 0.0F) return;

    bool isFalling    = (this->vy <= 0.0F);
    bool isAboveBlock = (pAABB.getMinY() >= oAABB.getMaxY() - 14.0F);
    if (isFalling && isAboveBlock && intersectsByX >= 4.0F)
    {
        this->setPositionY(oAABB.getMaxY() + (pAABB.size.height * 0.5F));
        this->hitGround();
        return;
    }

    if (intersectsByY > 14.0F)
    {
        if (pAABB.getMaxX() > oAABB.getMinX() + 6.0F && pAABB.getMinX() < oAABB.getMinX() + 14.0F)
        {
            this->isDead = true;
        }
    }
}

void PlayerObject::collideWithHazard(GameObject* obj)
{
    this->isDead = true;
}
void PlayerObject::collideWithOrb(GameObject* obj) {}
void PlayerObject::collideWithPad(GameObject* obj) {}
void PlayerObject::collideWithPortal(GameObject* obj) {}