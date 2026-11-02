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
    this->player->setPosition(ax::Vec2(visibleSize.width / 2, MIN_GROUND_Y));
    this->addChild(this->player);

    this->contentLayer = ax::Node::create();
    this->contentLayer->setPosition(visibleSize.width, MIN_GROUND_Y);
    this->player->setBaseWorldX(-this->contentLayer->getPositionX());
    this->addChild(this->contentLayer);

    auto obj = GameObject::createFromType(GameObjectType::Block);
    obj->setPosition(ax::Vec2::ZERO);
    this->contentLayer->addChild(obj);

    this->_listener               = ax::EventListenerTouchOneByOne::create();
    this->_listener->onTouchBegan = AX_CALLBACK_2(PlayScene::onTouchBegan, this);
    this->_listener->onTouchEnded = AX_CALLBACK_2(PlayScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(this->_listener, this);

    this->scheduleUpdate();
    return true;
}

void PlayScene::updateCamera(float dt)
{
    if (!contentLayer || !player) return;

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    float worldX = player->getWorldPositionX();

    contentLayer->setPositionX(-worldX);
}
void PlayScene::update(float dt)
{
    _ticker.update(dt);
}
void PlayScene::checkCollisions() {}
void PlayScene::loadLevel(int levelId) {}
void PlayScene::tick(float dt)
{
    this->player->update(dt);
    this->updateCamera(dt);
}

void PlayScene::cullChunks() {}

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