/*
 * PocketDash - Rhythm-based 2D Platformer
 * Copyright (C) 2026 [arti-max]
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "LevelSelectScene.h"
#include "ui/UIScale9Sprite.h"
#include "ui/UIButton.h"
#include "SaveManager.h"
#include "FontHelper.h"
#include "SongData.h"

#include "AlertPopup.h"

// scenes
#include "play/PlayScene.h"
#include "editor/EditorScene.h"

bool LevelSelectScene::init()
{
    if (!Scene::init()) return false;

    auto visibleSize = this->_director->getVisibleSize();

    auto bg = ax::LayerColor::create(ax::Color4F(0.4, 0.4, 0.4, 1.0), visibleSize.width, visibleSize.height);
    bg->setPosition(ax::Vec2::ZERO);
    this->addChild(bg, -100);

    ax::Size panelSize(visibleSize.width / 2, visibleSize.height - 20);
    this->panel = ax::ui::Scale9Sprite::createWithSpriteFrameName("content_panel.png");
    panel->setCapInsets(ax::Rect(3, 3, 138, 154));
    panel->setContentSize(panelSize / 2.5F);
    panel->setScale(2.5F);
    panel->setPosition(ax::Vec2(visibleSize.width * 0.5, visibleSize.height * 0.5));
    this->addChild(panel, -10);

    this->view = ax::ui::ListView::create();
    view->setDirection(ax::ui::ScrollView::Direction::VERTICAL);
    view->setContentSize(ax::Size(panelSize.width, panelSize.height - 20));
    view->setPosition(ax::Vec2::ZERO);
    view->setClippingEnabled(true);
    view->setBounceEnabled(true);
    view->setItemsMargin(10.0F);
    view->setScrollBarEnabled(false);
    view->setGravity(ax::ui::ListView::Gravity::CENTER_HORIZONTAL);
    view->setScale(1.0F / 2.5F);

    this->levels = SaveManager::getLocalLevels();

    for (const auto& lvl : this->levels)
    {
        view->pushBackCustomItem(createLevelCard(lvl, view->getContentSize().width - 20));
    }

    auto createBtn = ax::ui::Button::create("plus_btn.png", "", "", ax::ui::Widget::TextureResType::PLIST);
    createBtn->setScale(4.5F);
    createBtn->setPosition(
        ax::Vec2(visibleSize.width - createBtn->getContentSize().width - 56, createBtn->getContentSize().height + 56));
    createBtn->addClickEventListener([this](ax::Object* obj) -> void {
        this->levels.clear();
        this->view->removeAllItems();
        SaveManager::initLocalLevel("");
        this->levels = SaveManager::getLocalLevels();
        for (const auto& lvl : this->levels)
        {
            this->view->pushBackCustomItem(createLevelCard(lvl, this->view->getContentSize().width - 20));
        }
    });

    this->addChild(createBtn, 10);
    this->panel->addChild(view);
    return true;
}

ax::ui::Layout* LevelSelectScene::createLevelCard(const LevelMeta& meta, float width)
{
    const float height = 120.0F;

    auto card = ax::ui::Layout::create();
    card->setContentSize(ax::Size(width, height));
    card->setTouchEnabled(false);

    card->setBackGroundColorType(ax::ui::Layout::BackGroundColorType::SOLID);
    card->setBackGroundColor(ax::Color3B(0, 0, 0));
    card->setBackGroundColorOpacity(120);

    auto nameBtn = ax::ui::Button::create();
    nameBtn->setTitleText(meta.name);
    nameBtn->setTitleFontName("fonts/JUA.ttf");
    nameBtn->setTitleFontSize(getFontScaleWithSample(26.0F));
    nameBtn->setTitleColor(ax::Color3B::WHITE);
    nameBtn->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE_LEFT);
    nameBtn->setPosition(ax::Vec2(14.0F, height - 26.0F));
    nameBtn->setSwallowTouches(false);
    nameBtn->setScale(FONT_DIV_SAMPLE);
    nameBtn->addClickEventListener(
        [levelId = meta.id](ax::Object* obj) -> void { AXLOGD("TODO: rename level ({})", levelId); });

    auto infoLabel =
        ax::Label::createWithTTF(fmt::format("levelId: {}       song: {}", meta.onlineId, getSongNameByID(meta.songId)),
                                 "fonts/JUA.ttf", getFontScaleWithSample(12.0F));
    infoLabel->setColor(ax::Color3B::YELLOW);
    infoLabel->setAnchorPoint(ax::Vec2::ANCHOR_MIDDLE_LEFT);
    infoLabel->setPosition(ax::Vec2(22.0F, 16.0F));
    infoLabel->setScale(FONT_DIV_SAMPLE);

    // lambda for create action btns
    auto makeActionBtn = [card](std::string_view spriteName, float xPos, auto callback) -> ax::ui::Button* {
        auto btn = ax::ui::Button::create(spriteName, "", "", ax::ui::Widget::TextureResType::PLIST);
        btn->setPosition(ax::Vec2(xPos, card->getContentSize().height * 0.5));
        btn->setSwallowTouches(false);
        btn->addClickEventListener(std::move(callback));
        btn->setScale(2.5F);
        card->addChild(btn);
        return btn;
    };

    const float spacing = 56.0F;
    const int localId   = meta.id;
    const int onlineId  = meta.onlineId;

    makeActionBtn("delete_btn.png", width - 34 - (spacing * 0),
                  [localId, this](ax::Object* obj) { this->deleteLevel(localId); });

    makeActionBtn("edit_btn.png", width - 34 - (spacing * 1),
                  [localId, this](ax::Object* obj) { this->editLevel(localId); });

    makeActionBtn("play_btn.png", width - 34 - (spacing * 2),
                  [localId, this](ax::Object* obj) { this->playLevel(localId); });

    card->addChild(nameBtn);
    card->addChild(infoLabel);
    return card;
}

void LevelSelectScene::editLevel(int levelId)
{
    auto newScene   = EditorScene::create(levelId);
    auto transition = ax::TransitionFade::create(0.5F, newScene, ax::Color3B::BLACK);
    _director->pushScene(transition);
}
void LevelSelectScene::playLevel(int levelId)
{
    auto newScene   = PlayScene::create(levelId);
    auto transition = ax::TransitionFade::create(0.5F, newScene, ax::Color3B::BLACK);
    _director->pushScene(transition);
}
void LevelSelectScene::deleteLevel(int levelId)
{
    auto alert = AlertPopup::create("WRANING!", "Do you want to delete this level?", [this, levelId]() {
        bool deleted = SaveManager::deleteLocalLevel(levelId);
        if (deleted)
        {
            this->view->removeAllItems();
            this->levels.clear();
            this->levels = SaveManager::getLocalLevels();
            for (const auto& lvl : this->levels)
            {
                view->pushBackCustomItem(createLevelCard(lvl, view->getContentSize().width - 20));
            }
        }
    });
    alert->setPosition(ax::Vec2::ZERO);
    this->addChild(alert, 100);
}