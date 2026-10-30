#pragma once
#include "GameObjectType.h"
#include "axmol.h"
#include "cocos2d.h"
#include "ui/UIButton.h"

class BlockSelectLayer : public ax::Node
{
public:
    static constexpr int BLOCK_COUNT = 9;

    BlockSelectLayer()  = default;
    ~BlockSelectLayer() = default;

    CREATE_FUNC(BlockSelectLayer);
    bool init() override;

    void initObjectButtons();

    void onBlockButtonClicked(ax::Ref* sender, ax::ui::Widget::TouchEventType type);
    GameObjectType getSelectedType() const { return selected; }

private:
    GameObjectType selected = GameObjectType::Block;
};
