#pragma once
#include "ObjectChunk.h"
#include "axmol.h"
#include "GridLayer.h"

class EditorScene : public ax::Scene
{
public:
    static constexpr int MAX_UNITS_BY_Y    = 16;
    static constexpr int MAX_UNITS_BY_X    = 10000;
    static constexpr float DRAG_MAX_OFFSET = 10.0F;

    EditorScene()           = default;
    ~EditorScene() override = default;
    CREATE_FUNC(EditorScene);

    bool init() override;
    void update(float dt) override;

    void initBlockPanel(ax::Vec2 visibleSize, ax::Vec2 origin);
    int snapClickToBlock(float x, float y);  // returns cell id
    void handleCellClick(const ax::Vec2& pos);
    void addObjectToCell(int cellX, int cellY);

    void onTouchMoved(ax::Touch* touch, ax::Event* event);
    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchEnded(ax::Touch* touch, ax::Event* event);

    void cullChunks();

    ObjectChunk* getChunkByCell(int cellX, int cellY);

private:
    ax::Vector<ObjectChunk*> chunks;

    ax::Node* contentNode = nullptr;
    GridLayer* gridLayer  = nullptr;

    bool isDragging = false;

    ax::EventListenerTouchOneByOne* _listener = nullptr;
};