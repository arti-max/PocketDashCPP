#include "PlayScene.h"

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
    this->loadLevel(levelId);

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

    this->player = PlayerObject::create(2);
    this->player->setPosition(ax::Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(this->player);

    this->scheduleUpdate();
    return true;
}

void PlayScene::updateCamera() {}
void PlayScene::update(float dt)
{
    _ticker.update(dt);
}
void PlayScene::checkCollisions() {}
void PlayScene::loadLevel(int levelId) {}
void PlayScene::tick(float dt)
{
    this->player->update(dt);
}