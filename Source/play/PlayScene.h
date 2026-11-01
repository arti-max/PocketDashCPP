#pragma once
#include "axmol.h"
#include "PlayerObject.h"
#include "ObjectChunk.h"
#include "GameObject.h"
#include "Ticker.h"

// что должно быть в PlayScene?
/*
1. цикл обновления камеры
*/
class PlayScene : public ax::Scene
{
public:
    static constexpr int MIN_GROUND_Y = 115.0F;
    static PlayScene* create(int levelId);
    bool initWithLevelid(int levelId);

    PlayScene()  = default;
    ~PlayScene() = default;

    void update(float dt) override;
    void tick(float dt);
    void checkCollisions();
    void updateCamera();

    void loadLevel(int levelId);

private:
    ax::Vector<ObjectChunk*> chunks;
    Ticker _ticker{60};

    int levelId            = 0;
    PlayerObject* player   = nullptr;
    ax::Node* contentLayer = nullptr;
    ax::Node* groundLayer  = nullptr;
};