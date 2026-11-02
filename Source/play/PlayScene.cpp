#include "PlayScene.h"
#include "SaveManager.h"

PlayScene* PlayScene::create(int levelId)
{
    auto* obj = new (std::nothrow) PlayScene();
    if (obj && obj->initWithLevelid(levelId))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool PlayScene::initWithLevelid(int levelId)
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    this->levelId = levelId;

    _ticker.setCallback(AX_CALLBACK_1(PlayScene::tick, this));

    this->groundLayer = ax::Node::create();
    this->addChild(this->groundLayer, 100);

    auto background = ax::LayerColor::create(ax::Color4B(0, 0, 198, 255), visibleSize.width, visibleSize.height);
    background->setPosition(ax::Vec2::ZERO);
    this->addChild(background, -100);

    auto ground = ax::LayerColor::create(ax::Color4B(0, 0, 0, 255), visibleSize.width, 90.0F);
    ground->setPosition(ax::Vec2::ZERO);
    auto drawNode = ax::DrawNode::create();
    float width   = ground->getContentSize().width;
    float height  = ground->getContentSize().height;
    drawNode->drawLine(ax::Vec2(0.0F, height + 2), ax::Vec2(width, height + 2), ax::Color4F::WHITE, 6.0F);
    this->groundLayer->addChild(ground, 0);
    this->groundLayer->addChild(drawNode, 1);

    float screenSpawnX = (visibleSize.width / 2) - 160.0F;
    this->player       = PlayerObject::create(2);
    this->player->setPosition(ax::Vec2(screenSpawnX, MIN_GROUND_Y));
    this->addChild(this->player);

    this->contentLayer = ax::Node::create();
    this->contentLayer->setPosition(screenSpawnX, MIN_GROUND_Y_CONTENT);
    this->player->setBaseWorldX(0.0F);
    this->addChild(this->contentLayer);

    // auto obj = GameObject::createFromType(GameObjectType::Block);
    // obj->setPosition(ax::Vec2::ZERO);
    // this->contentLayer->addChild(obj);

    this->_listener               = ax::EventListenerTouchOneByOne::create();
    this->_listener->onTouchBegan = AX_CALLBACK_2(PlayScene::onTouchBegan, this);
    this->_listener->onTouchEnded = AX_CALLBACK_2(PlayScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(this->_listener, this);

    this->loadLevel(this->levelId);

    this->_debugDraw = ax::DrawNode::create();
    this->addChild(this->_debugDraw, 999);

    this->scheduleUpdate();
    return true;
}

void PlayScene::resetlevel()
{
    this->deathTicks   = 0;
    auto visibleSize   = _director->getVisibleSize();
    float screenSpawnX = (visibleSize.width / 2) - 160.0F;
    this->contentLayer->setPosition(screenSpawnX, MIN_GROUND_Y_CONTENT);
    this->player->setBaseWorldX(0.0F);
    this->player->setPositionY(MIN_GROUND_Y);
    this->player->setOnGround(true);
    this->player->setIsDead(false);
}

void PlayScene::updateCamera(float dt)
{
    if (!contentLayer || !player) return;

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    float screenSpawnX = (visibleSize.width / 2) - 160.0F;

    float targetX = screenSpawnX - player->getWorldPositionX();

    contentLayer->setPositionX(targetX);
    this->cullChunks();
}

void PlayScene::update(float dt)
{
    _ticker.update(dt);
}

void PlayScene::tick(float dt)
{
    if (this->player->getIsDead())
    {
        this->deathTicks++;
        if (this->deathTicks >= DEATH_TICKS) this->resetlevel();
    }
    else
    {
        this->player->update(dt);
        this->checkCollisions();
        this->updateCamera(dt);
        this->drawHitboxes();
    }
}

void PlayScene::cullChunks()
{
    auto visibleSize     = _director->getVisibleSize();
    float chunkPixelSize = ObjectChunk::CHUNK_SIZE * ObjectChunk::CELL_SIZE;

    float viewLeft  = -this->contentLayer->getPositionX() - chunkPixelSize;
    float viewRight = -this->contentLayer->getPositionX() + visibleSize.width + chunkPixelSize;

    int minChunkX = static_cast<int>(std::floor(viewLeft / chunkPixelSize));
    int maxChunkX = static_cast<int>(std::floor(viewRight / chunkPixelSize));

    for (ObjectChunk* chunk : this->chunks)
    {
        int cx = chunk->getChunkX();

        bool isVisible = (cx >= minChunkX && cx <= maxChunkX);
        chunk->setVisible(isVisible);
    }
}
void PlayScene::checkCollisions()
{
    if (this->player->getPositionY() > PlayScene::MIN_GROUND_Y)
    {
        this->player->setOnGround(false);
    }

    auto chunk = this->getCurrentChunk();
    if (chunk == nullptr) return;

    int cellX = this->player->getCellX();
    int cellY = this->player->getCellY();

    std::vector<GameObject*> objects;
    objects.reserve(9);
    chunk->collectObjectsAround(objects, cellX, cellY);

    if (objects.empty()) return;

    ax::Rect pAABB = this->player->getAABB();

    for (GameObject* obj : objects)
    {
        // AXLOGD("Object: {}", obj->getObjectString());
        if (pAABB.intersectsRect(obj->getAABB()))
        {
            AXLOGD("pAABB: {}, {}, {}, {}", pAABB.getMinX(), pAABB.getMaxX(), pAABB.getMinY(), pAABB.getMaxY());
            AXLOGD("Collided!!! {}: {}, {}, {}, {}", static_cast<int>(obj->getObjectType()), obj->getAABB().getMinX(),
                   obj->getAABB().getMaxX(), obj->getAABB().getMinY(), obj->getAABB().getMaxY());
            switch (obj->getObjectType())
            {
            case GameObjectType::Block: this->player->collideWithObject(obj); break;
            case GameObjectType::Spike: this->player->collideWithHazard(obj); break;
            case GameObjectType::Orb: this->player->collideWithOrb(obj); break;
            case GameObjectType::Pad: this->player->collideWithPad(obj); break;
            case GameObjectType::PortalB:
            case GameObjectType::PortalY: this->player->collideWithPortal(obj); break;
            default: break;
            }
        }
    }
}

bool PlayScene::onTouchBegan(ax::Touch* touch, ax::Event* event)
{
    if (this->player)
    {
        this->player->pushTouch();
    }
    return true;
}

void PlayScene::onTouchEnded(ax::Touch* touch, ax::Event* event)
{
    if (this->player)
    {
        this->player->popTouch();
    }
}

void PlayScene::loadLevel(int levelId)
{
    std::string levelData = SaveManager::loadLocalLevel(levelId);
    if (levelData.empty()) return;

    size_t start = 0;

    while (start < levelData.size())
    {
        size_t end = levelData.find(';', start);

        std::string_view blockStr;
        if (end == std::string_view::npos)  // last obj
        {
            blockStr = std::string_view(levelData).substr(start);
            start    = levelData.size();
        }
        else
        {
            blockStr = std::string_view(levelData).substr(start, end - start);
            start    = end + 1;
        }

        if (blockStr.empty()) continue;

        auto obj = GameObject::createFromObj(blockStr);
        if (obj != nullptr)
        {
            auto pos  = obj->getPosition();
            int cellX = std::floor(pos.x / ObjectChunk::CELL_SIZE);
            int cellY = std::floor(pos.y / ObjectChunk::CELL_SIZE);
            obj->setPosition(static_cast<float>(cellX), static_cast<float>(cellY));
            auto chunk = this->getChunkByCell(cellX, cellY);
            if (chunk == nullptr)
            {
                chunk = ObjectChunk::create();
                AXLOGD("chunk coords: {}, {}", std::floor(cellX / ObjectChunk::CELL_SIZE),
                       std::floor(cellY / ObjectChunk::CHUNK_SIZE));
                chunk->setChunkCoords(std::floor(cellX / ObjectChunk::CHUNK_SIZE),
                                      std::floor(cellY / ObjectChunk::CHUNK_SIZE));
                this->contentLayer->addChild(chunk);
                this->chunks.pushBack(chunk);
            }
            chunk->addObjectToChunk(obj);
        }
    }
}

ObjectChunk* PlayScene::getChunkByCell(int cellX, int cellY)
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

ObjectChunk* PlayScene::getCurrentChunk()
{
    int chunkX = (static_cast<int>(std::floor(this->player->getWorldPositionX())) / ObjectChunk::CELL_SIZE) /
                 ObjectChunk::CHUNK_SIZE;
    int chunkY =
        (static_cast<int>(std::floor(this->player->getPositionY())) / ObjectChunk::CELL_SIZE) / ObjectChunk::CHUNK_SIZE;
    // AXLOGD("CHUNKX: {}", chunkX);
    if (chunkX < 0) return nullptr;

    for (ObjectChunk* chunk : this->chunks)
    {
        if (chunk->getChunkX() == chunkX && chunk->getChunkY() == chunkY)
        {
            return chunk;
        }
    }

    return nullptr;
}

void PlayScene::drawHitboxes()
{
    if (!_debugDraw || !player || !contentLayer) return;

    _debugDraw->clear();

    ax::Rect pBox = this->player->getAABB();
    ax::Vec2 pPos = this->player->getPosition();
    ax::Rect playerScreenRect(pPos.x - pBox.size.width * 0.5f, pPos.y - pBox.size.height * 0.5f, pBox.size.width,
                              pBox.size.height);
    _debugDraw->drawRect(playerScreenRect.origin, playerScreenRect.origin + playerScreenRect.size, ax::Color4F::GREEN);

    auto chunk = this->getCurrentChunk();
    if (!chunk) return;

    std::vector<GameObject*> objects;
    objects.reserve(9);
    chunk->collectObjectsAround(objects, player->getCellX(), player->getCellY());

    float offsetX = this->contentLayer->getPositionX();

    for (GameObject* obj : objects)
    {
        ax::Rect box = obj->getAABB();
        ax::Rect screenBox(box.origin.x + offsetX, box.origin.y, box.size.width, box.size.height);
        ax::Color4F color = (obj->getObjectType() == GameObjectType::Spike) ? ax::Color4F::RED : ax::Color4F::BLUE;
        _debugDraw->drawRect(screenBox.origin, screenBox.origin + screenBox.size, color);
    }
}
