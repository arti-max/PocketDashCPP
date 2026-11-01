#pragma once
#include "GameObject.h"

class PlayerObject : public GameObject
{
public:
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
    void setInAir(bool f) { inAir = f; }
    bool isInAir() const { return inAir; }

    void hitGround();

private:
    const float BASE_X_VELOCITY = 311.0F;

    ax::Sprite* skin0 = nullptr;
    ax::Sprite* skin1 = nullptr;
    ax::Color3B color1;
    ax::Color3B color2;

    float vy = -5.0F;
    float vx = BASE_X_VELOCITY;

    bool onGround  = false;
    bool isHolding = false;
    bool doJump    = false;
    bool isDead    = false;
    bool doRotate  = false;
    bool inAir     = false;
};