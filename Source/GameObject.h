#pragma once
#include "GameObjectType.h"
#include "axmol.h"

// GameObject должен:
/*
1. Объект в уровне - игрок, блок. (блок как в уровне, так и в редакторе)
2. Иметь ax::Rect (AABB)
3. Возможность менять цвет (и флаг, если объект может менять цвет)
4. Флаг, триггер ли
5. получить позицию по x, y.
6. флаг, активен ли GameObject в данный момент (проверка на границу экрана)
*/

class GameObject : public ax::Sprite
{
public:
    static constexpr float SPRITE_SCALE       = 2.5F;
    static constexpr float OBJECT_SPRITE_SIZE = 16.0F;

    static GameObject* create(int spriteId);
    static GameObject* createFromType(GameObjectType type);
    static GameObject* createFromObj(std::string_view objString);

    bool initWithType(GameObjectType type);

    GameObject()           = default;
    ~GameObject() override = default;

    void setCanChangeColor(bool flag) { canChangeColor = flag; }
    void activate() { activated = true; }
    void deacticate() { activated = false; }
    void setIsTrigger(bool flag) { isTrigger = flag; }

    ax::Rect getAABB() const;

    bool getCanChangeColor() const { return canChangeColor; }
    bool getIsActivated() const { return activated; }
    bool getIsTrigger() const { return isTrigger; }

    void setChunkId(int id) { chunkId = id; }

    std::string getObjectString();

    ax::Node* chunk = nullptr;

private:
    ax::Rect bbox;

    GameObjectType type = GameObjectType::Block;
    bool canChangeColor = false;
    bool activated      = false;
    bool isTrigger      = false;

    int chunkId = 0;
};