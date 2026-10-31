#pragma once
#include "GameObjectType.h"
#include "BuildMode.h"
#include "axmol.h"
#include "cocos2d.h"
#include "ui/UIButton.h"

class BlockSelectLayer : public ax::Node
{
public:
    static constexpr int BLOCK_COUNT = 9;
    static constexpr int MODE_COUNT  = 4;

    BlockSelectLayer()  = default;
    ~BlockSelectLayer() = default;

    CREATE_FUNC(BlockSelectLayer);
    bool init() override;

    void initObjectButtons();
    void initModeButtons();

    void onBlockButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type);
    void onModeButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type);

    GameObjectType getSelectedType() const { return selected; }
    BuildMode getSelectedMode() const { return mode; }
    bool getSwipeMode() const { return swipeMode; }
    void selectButton(ax::ui::Button* btn, int type = 0);

private:
    GameObjectType selected = GameObjectType::Block;
    bool swipeMode          = false;
    BuildMode mode          = BuildMode::BUILD;

    ax::ui::Button* activeButton      = nullptr;
    ax::ui::Button* activeModeButton  = nullptr;
    ax::ui::Button* activeBuildButton = nullptr;
};
