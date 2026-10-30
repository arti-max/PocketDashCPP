#pragma once
#include "GameObject.h"
#include "axmol.h"

// Что должен делать чанк?
/*
1. массив с объектами уровня фиксированной длины.
2. чанк размером CHUNK_SIZExCHUNK_SIZE GameObject*'ов
3. добавление объектов в чанк и получение объектов из чанка
4. Преобразование глобальныхх координат в локальные и наоборот
5. Координаты самого чанка
*/

class ObjectChunk : public ax::Node
{
public:
    static constexpr int CHUNK_SIZE    = 16;
    static constexpr int TOTAL_OBJECTS = CHUNK_SIZE * CHUNK_SIZE;
    static constexpr float CELL_SIZE   = GameObject::SPRITE_SCALE * GameObject::OBJECT_SPRITE_SIZE;

    ObjectChunk()           = default;
    ~ObjectChunk() override = default;

    CREATE_FUNC(ObjectChunk);
    bool init() override;

    GameObject* getObjectAt(float x, float y);  // global coords
    GameObject* getObjectAt(const ax::Vec2& pos);

    ax::Vec2 globalToLocal(float x, float y) const;
    ax::Vec2 globalToLocal(const ax::Vec2& pos) const;

    int getChunkX() const { return this->x; };
    int getChunkY() const { return this->y; };

    void setChunkCoords(int x, int y);

    bool addObjectToChunk(GameObject* obj);
    int getIndexByPos(int x, int y) const;

private:
    const int MAX_CHUNK_SIZE = 256;

    std::array<ax::RefPtr<GameObject>, TOTAL_OBJECTS> objects{};
    int maxArraySize = 0;

    int width  = 0;
    int height = 0;

    int x = 0;
    int y = 0;
};