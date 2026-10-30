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

    for (int i = 0; i < 1; i++)
    {
        auto c = ObjectChunk::create();
        // for (int x = 0; x < ObjectChunk::CHUNK_SIZE; x++)
        // {
        //     for (int y = 0; y < ObjectChunk::CHUNK_SIZE; y++)
        //     {
        //         auto b2 = GameObject::createFromType((i == 0)   ? GameObjectType::Block
        //                                              : (i == 1) ? GameObjectType::Spike
        //                                              : (i == 2) ? GameObjectType::Orb
        //                                              : (i == 3) ? GameObjectType::Pad
        //                                              : (i == 4) ? GameObjectType::PortalB
        //                                              : (i == 5) ? GameObjectType::PortalY
        //                                                         : GameObjectType::Trigger);
        //         b2->setPosition(x * ObjectChunk::CELL_SIZE, y * ObjectChunk::CELL_SIZE);

        //         c->addObjectToChunk(b2);
        //     }
        // }

        c->setChunkCoords(i, 0);
        this->contentNode->addChild(c);
        this->chunks.pushBack(c);
    }
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
    return true;
}

void EditorScene::onTouchMoved(ax::Touch* touch, ax::Event* event)
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

void EditorScene::onTouchEnded(ax::Touch* touch, ax::Event* event) {}

void EditorScene::cullChunks()
{
    auto visibleSize     = _director->getVisibleSize();
    float chunkPixelSize = ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE;

    float viewLeft  = -this->contentNode->getPositionX() - chunkPixelSize;
    float viewRight = -this->contentNode->getPositionX() + visibleSize.width + chunkPixelSize;

    int minIdx = std::max(0, static_cast<int>(viewLeft / chunkPixelSize));
    int maxIdx = static_cast<int>(viewRight / chunkPixelSize);

    for (int i = 0; i < this->chunks.size(); ++i)
    {
        bool isVisible = (i >= minIdx && i <= maxIdx);
        this->chunks.at(i)->setVisible(isVisible);
    }
}