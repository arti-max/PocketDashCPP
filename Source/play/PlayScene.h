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
    static constexpr int MIN_GROUND_Y         = 115.0F;
    static constexpr int MIN_GROUND_Y_CONTENT = 95.0F;
    static PlayScene* create(int levelId);
    bool initWithLevelid(int levelId);

    PlayScene()  = default;
    ~PlayScene() = default;

    void update(float dt) override;
    void tick(float dt);
    void checkCollisions();
    void updateCamera(float dt);
    void cullChunks();

    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchEnded(ax::Touch* touch, ax::Event* event);

    void loadLevel(int levelId);
    void resetlevel();

    ObjectChunk* getChunkByCell(int cellX, int cellY);
    ObjectChunk* getCurrentChunk();

private:
    const int DEATH_TICKS = 60;
    ax::Vector<ObjectChunk*> chunks;
    Ticker _ticker{60};

    int deathTicks = 0;

    int levelId            = 0;
    PlayerObject* player   = nullptr;
    ax::Node* contentLayer = nullptr;
    ax::Node* groundLayer  = nullptr;
    ax::Node* uiLayer      = nullptr;

    ax::EventListenerTouchOneByOne* _listener = nullptr;

    // debug:

    ax::DrawNode* _debugDraw = nullptr;
    void drawHitboxes();
};