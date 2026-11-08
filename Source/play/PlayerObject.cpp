#include "PlayerObject.h"
#include "PlayScene.h"
#include "audio/AudioEngine.h"
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

    this->deathParticles = ParticleConfigLoader::createFromJson("particleB.json");
    this->deathParticles->setScale(1.2F / this->getScale());
    this->addChild(this->deathParticles);

    this->deathParticles->setStartColor(ax::Color4F(this->color1));
    this->deathParticles->setEndColor(ax::Color4F(this->color2));
    this->deathParticles->stopSystem();

    ax::AudioEngine::preload("explode.wav");

    return true;
}

void PlayerObject::update(float dt)
{
    if (isInAir())
    {
        this->vy += (GRAVITY * this->getGravityMul()) * dt;
        if (this->vy < -MAX_VELOCITY_Y || this->vy > MAX_VELOCITY_Y)
            this->vy = -(MAX_VELOCITY_Y * this->getGravityMul());
    }

    float nx = this->getWorldPositionX() + (this->vx * dt);
    float ny = this->worldY + ((this->vy * dt));

    if (ny <= 0.0F && !this->isUpside)
    {
        ny             = 0.0F;
        this->vy       = 0.0F;
        this->onGround = true;
        this->snapRotation();
    }

    if (isInAir() && this->getActionByTag(ROTATE_ACTION_TAG) == nullptr)
    {
        auto rotateAction = ax::RotateBy::create(0.4F, 180.0F * this->getGravityMul());
        rotateAction->setTag(ROTATE_ACTION_TAG);
        this->runAction(rotateAction);
    }

    if (isOnGround() && this->isHolding)
    {
        this->jump();
    }

    this->worldY = ny;
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
    this->doJump    = true;
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
    this->doJump   = false;
    this->vy       = JUMP_VELOCITY * this->getGravityMul();
    this->onGround = false;
}

int PlayerObject::getCellX() const
{
    return static_cast<int>(std::floor(this->worldX / ObjectChunk::CELL_SIZE));
}

int PlayerObject::getCellY() const
{
    return static_cast<int>(std::floor(this->worldY / ObjectChunk::CELL_SIZE));
}

ax::Rect PlayerObject::getAABB() const
{
    float w = OBJECT_SPRITE_SIZE * SPRITE_SCALE;
    float h = OBJECT_SPRITE_SIZE * SPRITE_SCALE;

    float cx = this->getWorldPositionX();
    float cy = this->worldY + (h * 0.5F);

    return ax::Rect(cx - (w * 0.5F), cy - (h * 0.5F), w, h);
}

void PlayerObject::drop()
{
    if (this->worldY > 0.0F && this->isOnGround())
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

    bool isFalling = (this->vy * this->getGravityMul() <= 0.0F);
    bool isAboveBlock =
        this->isUpside ? (pAABB.getMaxY() <= oAABB.getMinX() + 14.0F) : (pAABB.getMinY() >= oAABB.getMaxY() - 14.0F);
    if (isFalling && isAboveBlock && intersectsByX >= 4.0F)
    {
        this->worldY = this->isUpside ? oAABB.getMinY() - (pAABB.size.height) : oAABB.getMaxY();
        this->hitGround();
        return;
    }

    if (intersectsByY > 14.0F)
    {
        if (pAABB.getMaxX() > oAABB.getMinX() + 6.0F && pAABB.getMinX() < oAABB.getMinX() + 14.0F)
        {
            this->death();
        }
    }
}

void PlayerObject::collideWithHazard(GameObject* obj)
{
    this->death();
}
void PlayerObject::collideWithOrb(GameObject* obj)
{
    if (this->doJump)
    {
        this->vy       = JUMP_VELOCITY * this->getGravityMul();
        this->onGround = false;
        this->doJump   = false;
    }
}
void PlayerObject::collideWithPad(GameObject* obj)
{
    this->vy       = (JUMP_VELOCITY * 1.2F) * this->getGravityMul();
    this->onGround = false;
}
void PlayerObject::collideWithPortal(GameObject* obj)
{
    if (obj->getObjectType() == GameObjectType::PortalY && !this->isUpside)
    {
        this->vy *= 0.6F;
        this->isUpside = true;
        this->onGround = false;
    }
    else if (this->isUpside && obj->getObjectType() == GameObjectType::PortalB)
    {
        this->vy *= 0.6F;
        this->isUpside = false;
        this->onGround = false;
    }
    this->stopActionByTag(ROTATE_ACTION_TAG);
}

void PlayerObject::showSkin()
{
    this->skin0->setVisible(true);
    this->skin1->setVisible(true);
}

void PlayerObject::death()
{
    if (this->getActionByTag(ROTATE_ACTION_TAG) != nullptr) this->stopActionByTag(ROTATE_ACTION_TAG);
    this->isDead = true;
    this->skin0->setVisible(false);
    this->skin1->setVisible(false);
    this->deathParticles->resetSystem();
    int soundId = ax::AudioEngine::play2d("explode.wav", false, 1.0F);
}