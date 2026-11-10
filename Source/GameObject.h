#pragma once
#include "GameObjectType.h"
#include "VisualLayer.h"
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

    virtual ax::Rect getAABB() const;

    bool getCanChangeColor() const { return canChangeColor; }
    bool getIsActivated() const { return activated; }
    bool getIsTrigger() const { return isTrigger; }
    GameObjectType getObjectType() const { return type; }
    void setVisualLayer(VisualLayer layer);

    void setChunkId(int id) { chunkId = id; }

    std::string getObjectString();

    void setFadeTime(float fade) { fadeTime = fade; }
    void setTriggerR(int r) { c_r = r; }
    void setTriggerG(int g) { c_g = g; }
    void setTriggerB(int b) { c_b = b; }

    float getFadeTime() const { return fadeTime; }
    ax::Color3B getTriggerColor() const { return ax::Color3B(c_r, c_g, c_b); }

    float getWorldPositionX();

    ax::Node* chunk  = nullptr;
    ax::Sprite* ring = nullptr;

private:
    ax::Rect bbox;

    GameObjectType type = GameObjectType::Block;
    bool canChangeColor = false;
    bool activated      = false;
    bool isTrigger      = false;

    float fadeTime = 0.5F;
    int c_r        = 0;
    int c_b        = 0;
    int c_g        = 0;

    int chunkId = 0;
};