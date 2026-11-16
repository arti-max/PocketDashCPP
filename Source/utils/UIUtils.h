#pragma once
#include "FontHelper.h"
#include "axmol.h"
#include "cocos2d.h"
#include "ui/UIButton.h"

ax::ui::Button* createTextButton(std::string_view spriteName,
                                 std::string_view text,
                                 float spriteScale,
                                 float fontSize,
                                 std::function<void()> onClick = nullptr)
{
    auto btn = ax::ui::Button::create(spriteName, "", "", ax::ui::Widget::TextureResType::PLIST);
    btn->setScale(spriteScale);
    btn->setPressedActionEnabled(false);

    auto label = ax::Label::createWithTTF(text, "fonts/JUA.ttf", getFontScaleWithSample(fontSize));
    label->setScale(FONT_DIV_SAMPLE / spriteScale);
    label->setPosition(btn->getContentSize() * 0.5F);
    btn->addChild(label, 1);

    auto baseScale = std::make_shared<float>(0.0F);

    btn->addTouchEventListener([baseScale, onClick](ax::Ref* sender, ax::ui::Widget::TouchEventType type) {
        auto node = static_cast<ax::Node*>(sender);
        if (type == ax::ui::Widget::TouchEventType::BEGAN)
        {
            if (*baseScale == 0.0F)
            {
                *baseScale = node->getScale();
            }
            node->stopAllActions();
            node->runAction(ax::ScaleTo::create(0.05F, (*baseScale) * 1.15F));
        }
        else if (type == ax::ui::Widget::TouchEventType::ENDED)
        {
            node->stopAllActions();
            node->runAction(ax::ScaleTo::create(0.05F, *baseScale));
            if (onClick) onClick();
        }
        else if (type == ax::ui::Widget::TouchEventType::CANCELED)
        {
            node->stopAllActions();
            node->runAction(ax::ScaleTo::create(0.05F, *baseScale));
        }
    });

    return btn;
}