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