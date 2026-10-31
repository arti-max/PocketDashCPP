#include "EditorScene.h"
#include "SaveManager.h"

bool EditorScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    SaveManager::initSaveFile();  // remove in future

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

    this->selectLayer = BlockSelectLayer::create();
    this->addChild(this->selectLayer, 10);

    auto exitBtn = ax::ui::Button::create("pause_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    exitBtn->setPosition(ax::Vec2(visibleSize.width - (exitBtn->getContentSize().width + 10),
                                  visibleSize.height - (exitBtn->getContentSize().height + 10)));
    exitBtn->setPressedActionEnabled(true);
    exitBtn->setScale(2.5F);
    exitBtn->addTouchEventListener(AX_CALLBACK_2(EditorScene::onExitButtonClicked, this));
    this->addChild(exitBtn, 10);

    // TODO: load level data from save
    this->loadLevelFromSave(0);
    this->cullChunks();

    _listener               = ax::EventListenerTouchOneByOne::create();
    _listener->onTouchBegan = AX_CALLBACK_2(EditorScene::onTouchBegan, this);
    _listener->onTouchMoved = AX_CALLBACK_2(EditorScene::onTouchMoved, this);
    _listener->onTouchEnded = AX_CALLBACK_2(EditorScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_listener, this);

    return true;
}

void EditorScene::update(float dt) {}
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
    if (this->isDragging && !this->selectLayer->getSwipeMode())
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
        if (this->contentNode->getPositionY() < -((MAX_UNITS_BY_Y - 10) * ObjectChunk::CELL_SIZE))
        {
            this->contentNode->setPositionY(-((MAX_UNITS_BY_Y - 10) * ObjectChunk::CELL_SIZE));
        }
        this->cullChunks();
    }
    else if (this->selectLayer->getSwipeMode())
    {
        this->handleCellClick(touch->getLocation());
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

    switch (this->selectLayer->getSelectedMode())
    {
    case BuildMode::BUILD: this->addObjectToCell(cellX, cellY); break;
    case BuildMode::DELETE: this->removeObjectFromCell(cellX, cellY); break;
    case BuildMode::EDIT: this->editObjectFromCell(cellX, cellY);
    default: break;
    }
}

void EditorScene::addObjectToCell(int cellX, int cellY)
{
    auto object = GameObject::createFromType(this->selectLayer->getSelectedType());
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

void EditorScene::removeObjectFromCell(int cellX, int cellY)
{
    auto chunk = this->getChunkByCell(cellX, cellY);
    if (chunk == nullptr) return;
    auto obj = chunk->getObjectAt(cellX, cellY);
    if (obj != nullptr)
    {
        AXLOGD("Removed: {}, {}", cellX, cellY);
        chunk->removeObjectFromChunk(obj);
    }
    if (chunk->getObjectCount() <= 0)
    {
        this->chunks.eraseObject(chunk);
        this->contentNode->removeChild(chunk);
        this->cullChunks();
    }
}

void EditorScene::editObjectFromCell(int cellX, int cellY)
{
    auto chunk = this->getChunkByCell(cellX, cellY);
    if (chunk == nullptr) return;
    auto obj = chunk->getObjectAt(cellX, cellY);
    if (obj != nullptr)
    {
        if (obj->getIsTrigger()) return;
        AXLOGD("Rotate Obj!: {}", obj->getRotation());
        obj->setRotation(obj->getRotation() + 90.0F);
    }
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

// BlockID,posX,posY,rot;
void EditorScene::loadLevelFromSave(int levelId)
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
                this->contentNode->addChild(chunk);
                this->chunks.pushBack(chunk);
            }
            chunk->addObjectToChunk(obj);
        }
    }
}

bool EditorScene::saveLevelToSave(int levelId)
{
    std::string dataString;

    std::vector<GameObject*> objects;
    objects.reserve(ObjectChunk::TOTAL_OBJECTS);

    for (auto* chunk : this->chunks)
    {
        objects.clear();
        chunk->collectObjects(objects);
        for (GameObject* obj : objects)
        {
            if (obj == nullptr) continue;
            dataString += obj->getObjectString();
        }
    }

    return SaveManager::saveLocalLevel(dataString, levelId);
}

void EditorScene::onExitButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type)
{
    if (type == ax::ui::Widget::TouchEventType::ENDED)
    {
        bool saved = this->saveLevelToSave(0);
        if (!saved) AXLOGE("Failed to save level");
    }
}