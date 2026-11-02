#pragma once
#include "GameObject.h"

class PlayerObject : public GameObject
{
public:
    static constexpr int ROTATE_ACTION_TAG = 0;
    static PlayerObject* create(int skinId);
    bool initWitchSkinId(int skinId);

    PlayerObject()  = default;
    ~PlayerObject() = default;

    void update(float dt);
    void jump();

    void pushTouch();
    void popTouch();

    void setOnGround(bool f) { onGround = f; }
    void setIsDead(bool f) { isDead = f; }
    bool isOnGround() const { return onGround; }
    bool getIsDead() const { return isDead; }
    bool isAlive() const { return !isDead; }
    bool isInAir() const { return !onGround; }
    float getWorldPositionX() const { return this->worldX; }
    void setBaseWorldX(float x) { worldX = x; };
    int getGravityMul() const { return isUpside ? -1 : 1; }

    void collideWithObject(GameObject* obj);
    void collideWithHazard(GameObject* obj);
    void collideWithPortal(GameObject* obj);
    void collideWithPad(GameObject* obj);
    void collideWithOrb(GameObject* obj);

    int getCellX() const;
    int getCellY() const;

    void snapRotation();

    void hitGround();
    void drop();

    ax::Rect getAABB() const override;

private:
    const float BASE_X_VELOCITY = 421.0F;
    const float GRAVITY         = -3920.0F;
    const float JUMP_VELOCITY   = 940.0F;

    ax::Sprite* skin0 = nullptr;
    ax::Sprite* skin1 = nullptr;
    ax::Color3B color1;
    ax::Color3B color2;

    float vy = 0.0F;
    float vx = BASE_X_VELOCITY;

    float worldX = 0.0F;

    bool isUpside = false;

    bool onGround  = false;
    bool isHolding = false;
    bool doJump    = false;
    bool isDead    = false;
    bool doRotate  = false;
};