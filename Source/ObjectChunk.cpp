#include "ObjectChunk.h"

bool ObjectChunk::init()
{
    if (CHUNK_SIZE > MAX_CHUNK_SIZE)
    {
        return false;
    }

    if (!Node::init())
    {
        return false;
    }

    this->width  = CHUNK_SIZE;
    this->height = CHUNK_SIZE;

    float pixelWidth  = this->width * CELL_SIZE;
    float pixelHeight = this->height * CELL_SIZE;
    this->setContentSize(ax::Size(pixelWidth, pixelHeight));

    this->maxArraySize = this->width * this->height;

    return true;
}

void ObjectChunk::setChunkCoords(int x, int y)
{
    this->x = x;
    this->y = y;

    this->setPosition(x * this->getContentSize().width, y * this->getContentSize().height);
}

int ObjectChunk::getIndexByPos(int x, int y) const
{
    return (y * this->width) + x;
}

bool ObjectChunk::addObjectToChunk(GameObject* obj)
{
    auto pos   = obj->getPosition();
    int localX = (static_cast<int>(pos.x) - (this->x * this->width));
    int localY = (static_cast<int>(pos.y) - (this->y * this->height));

    if (localX < 0 || localY < 0 || localX >= this->width || localY >= this->height)
    {
        return false;
    }

    int idx = getIndexByPos(localX, localY);

    if (this->objects[idx] != nullptr)
    {
        return false;
    }

    this->objects[idx] = obj;
    this->addChild(obj);
    obj->setPosition((localX + 0.5F) * CELL_SIZE, (localY + 0.5F) * CELL_SIZE);
    obj->chunk = this;
    objectCount++;

    if (obj->getObjectType() == GameObjectType::Orb && this->playMode)
    {
        auto ring = ax::Sprite::createWithSpriteFrameName("orb_01_1.png");
        ring->setScale(GameObject::SPRITE_SCALE);
        ring->setPosition(obj->getPosition());
        this->addChild(ring);
        this->rings.pushBack(ring);
        obj->ring = ring;
    }
    return true;
}

bool ObjectChunk::removeObjectFromChunk(GameObject* obj)
{
    auto pos   = obj->getPosition();
    int localX = (static_cast<int>(pos.x / CELL_SIZE));
    int localY = (static_cast<int>(pos.y / CELL_SIZE));

    AXLOGD("delete Pos: {}, {}", localX, localY);

    if (localX < 0 || localY < 0 || localX >= this->width || localY >= this->height)
    {
        return false;
    }

    int idx = getIndexByPos(localX, localY);

    if (this->objects[idx] == nullptr)
    {
        return false;
    }
    this->removeChild(obj);
    obj->chunk         = nullptr;
    this->objects[idx] = nullptr;
    objectCount--;
    return true;
}

GameObject* ObjectChunk::getObjectAt(int cellX, int cellY)
{
    int localX = (static_cast<int>(cellX) - (this->x * this->width));
    int localY = (static_cast<int>(cellY) - (this->y * this->height));

    if (localX < 0 || localY < 0 || localX >= this->width || localY >= this->height)
    {
        return nullptr;
    }
    return this->objects[getIndexByPos(localX, localY)].get();
}

GameObject* ObjectChunk::getObjectAt(const ax::Vec2& pos)
{
    return getObjectAt(std::floor(pos.x), std::floor(pos.y));
}

ax::Vec2 ObjectChunk::globalToLocal(int cellX, int cellY) const
{
    int localX = (cellX - (this->x * this->width));
    int localY = (cellY - (this->y * this->height));
    return ax::Vec2(static_cast<float>(localX), static_cast<float>(localY));
}

ax::Vec2 ObjectChunk::globalToLocal(const ax::Vec2& pos) const
{
    return globalToLocal(std::floor(pos.x), std::floor(pos.y));
}

void ObjectChunk::collectObjects(std::vector<GameObject*>& list) const
{
    if (this->objectCount == 0) return;

    for (const auto& ref : this->objects)
    {
        if (auto* obj = ref.get())
        {
            list.push_back(obj);
        }
    }
}

void ObjectChunk::collectObjectsAround(std::vector<GameObject*>& list, int cellX, int cellY) const
{
    if (this->objectCount == 0) return;

    int centerLX = cellX - (this->x * this->width);
    int centerLY = cellY - (this->y * this->height);

    for (int dy = -1; dy <= 1; ++dy)
    {
        int ly = centerLY + dy;
        if (ly < 0 || ly >= this->height) continue;

        for (int dx = -1; dx <= 1; ++dx)
        {
            int lx = centerLX + dx;
            if (lx < 0 || lx >= this->width) continue;

            int idx = this->getIndexByPos(lx, ly);
            if (auto* obj = this->objects[idx].get())
            {
                list.push_back(obj);
            }
        }
    }
}

void ObjectChunk::getRingNodes(std::vector<ax::Sprite*>& list)
{
    if (this->objectCount == 0) return;

    for (auto* ring : this->rings)
    {
        list.push_back(ring);
    }
}

void ObjectChunk::getOrbNodes(std::vector<GameObject*>& list)
{
    if (this->objectCount == 0) return;

    for (const auto& ref : this->objects)
    {
        if (auto* obj = ref.get())
        {
            if (obj->getObjectType() != GameObjectType::Orb) continue;
            list.push_back(obj);
        }
    }
}