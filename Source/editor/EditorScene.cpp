#include "EditorScene.h"

bool EditorScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    this->contentNode = ax::Node::create();
    this->contentNode->setPosition(200.0F, 150.0F);

    this->gridLayer = GridLayer::create(MAX_UNITS_BY_X, MAX_UNITS_BY_Y);
    this->contentNode->addChild(this->gridLayer, -1);

    this->addChild(this->contentNode);

    auto background = ax::LayerColor::create(ax::Color4B(120, 120, 120, 255), visibleSize.width, visibleSize.height);
    background->setPosition(origin);

    this->addChild(background, -100);

    this->initBlockPanel(visibleSize, origin);

    // for (int i = 0; i < 1; i++)
    // {
    //     auto c = ObjectChunk::create();
    //     for (int x = 0; x < ObjectChunk::CHUNK_SIZE; x++)
    //     {
    //         for (int y = 0; y < ObjectChunk::CHUNK_SIZE; y++)
    //         {
    //             auto b2 = GameObject::createFromType((i == 0)   ? GameObjectType::Block
    //                                                  : (i == 1) ? GameObjectType::Spike
    //                                                  : (i == 2) ? GameObjectType::Orb
    //                                                  : (i == 3) ? GameObjectType::Pad
    //                                                  : (i == 4) ? GameObjectType::PortalB
    //                                                  : (i == 5) ? GameObjectType::PortalY
    //                                                             : GameObjectType::Trigger);
    //             b2->setPosition(x * ObjectChunk::CELL_SIZE, y * ObjectChunk::CELL_SIZE);

    //             c->addObjectToChunk(b2);
    //         }
    //     }

    //     c->setChunkCoords(i, 0);
    //     this->contentNode->addChild(c);
    //     this->chunks.pushBack(c);
    // }
    this->cullChunks();

    _listener               = ax::EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = AX_CALLBACK_2(EditorScene::onTouchBegan, this);
    _listener->onTouchMoved = AX_CALLBACK_2(EditorScene::onTouchMoved, this);
    _listener->onTouchEnded = AX_CALLBACK_2(EditorScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

    return true;
}

void EditorScene::update(float dt) {}
void EditorScene::initBlockPanel(ax::Vec2 visibleSize, ax::Vec2 origin)
{
    float panelWidth  = visibleSize.width - 180.0F;
    float panelHeight = 120.0F;

    auto rect = ax::LayerColor::create(ax::Color4B(0, 0, 0, 120), panelWidth, panelHeight);
    rect->setPosition(origin.x + (visibleSize.width - panelWidth) * 0.5F, origin.y + 10.0F);
    this->addChild(rect, 10);
}
int EditorScene::snapClickToBlock(float x, float y)
{
    return 0;
}

bool EditorScene::onTouchBegan(ax::Touch* touch, ax::Event* event)
{
    this->isDragging = false;
    return true;
}

void EditorScene::onTouchMoved(ax::Touch* touch, ax::Event* event)
{
    if (!this->isDragging)
    {
        float dist = touch->getStartLocation().distance(touch->getLocation());
        if (dist > DRAG_MAX_OFFSET)
        {
            this->isDragging = true;
        }
    }
    if (this->isDragging)
    {
        ax::Vec2 delta = touch->getDelta();
        this->contentNode->setPosition(this->contentNode->getPosition() + delta);
        if (this->contentNode->getPositionX() > 200.0F)
        {
            this->contentNode->setPositionX(200.0F);
        }
        if (this->contentNode->getPositionY() > 150.0F)
        {
            this->contentNode->setPositionY(150.0F);
        }
        this->cullChunks();
    }
}

void EditorScene::onTouchEnded(ax::Touch* touch, ax::Event* event)
{
    if (!this->isDragging)
    {
        ax::Vec2 screenClickPos = touch->getLocation();
        this->handleCellClick(screenClickPos);
    }

    this->isDragging = false;
}

void EditorScene::handleCellClick(const ax::Vec2& pos)
{
    // AXLOGD("click x: {}, y: {}", pos.x, pos.y);
    // AXLOGD("click+levelPos x: {}, y: {}", std::floor(pos.x + -this->contentNode->getPositionX()),
    //    std::floor(pos.y + -this->contentNode->getPositionY()));

    int clickX = std::floor(pos.x - this->contentNode->getPositionX());
    int clickY = std::floor(pos.y - this->contentNode->getPositionY());
    if (clickX < 0 || clickY < 0)
    {
        return;
    }

    int cellX = clickX / static_cast<int>(ObjectChunk::CELL_SIZE);
    int cellY = clickY / static_cast<int>(ObjectChunk::CELL_SIZE);

    if (cellX >= MAX_UNITS_BY_X || cellY >= MAX_UNITS_BY_Y)
    {
        return;
    }

    AXLOGD("click to cell: x: {}, y: {}", cellX, cellY);

    this->addObjectToCell(cellX, cellY);
}

void EditorScene::addObjectToCell(int cellX, int cellY)
{
    auto object = GameObject::createFromType(GameObjectType::Block);
    object->setPosition(static_cast<float>(cellX), static_cast<float>(cellY));
    auto chunk = this->getChunkByCell(cellX, cellY);
    if (chunk == nullptr)
    {
        chunk = ObjectChunk::create();
        AXLOGD("chunk coords: {}, {}", std::floor(cellX / ObjectChunk::CELL_SIZE),
               std::floor(cellY / ObjectChunk::CHUNK_SIZE));
        chunk->setChunkCoords(std::floor(cellX / ObjectChunk::CHUNK_SIZE), std::floor(cellY / ObjectChunk::CHUNK_SIZE));
        this->contentNode->addChild(chunk);
        this->chunks.pushBack(chunk);
        this->cullChunks();
    }
    chunk->addObjectToChunk(object);
}

ObjectChunk* EditorScene::getChunkByCell(int cellX, int cellY)
{
    int chunkX = cellX / ObjectChunk::CHUNK_SIZE;
    int chunkY = cellY / ObjectChunk::CHUNK_SIZE;

    for (ObjectChunk* chunk : this->chunks)
    {
        if (chunk->getChunkX() == chunkX && chunk->getChunkY() == chunkY)
        {
            return chunk;
        }
    }
    return nullptr;
}

void EditorScene::cullChunks()
{
    auto visibleSize     = _director->getVisibleSize();
    float chunkPixelSize = ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE;

    float viewLeft  = -this->contentNode->getPositionX() - chunkPixelSize;
    float viewRight = -this->contentNode->getPositionX() + visibleSize.width + chunkPixelSize;

    int minChunkX = static_cast<int>(std::floor(viewLeft / chunkPixelSize));
    int maxChunkX = static_cast<int>(std::floor(viewRight / chunkPixelSize));

    for (ObjectChunk* chunk : this->chunks)
    {
        int cx = chunk->getChunkX();

        bool isVisible = (cx >= minChunkX && cx <= maxChunkX);
        chunk->setVisible(isVisible);
    }
}