#include "GridLayer.h"
#include "ObjectChunk.h"
#include "EditorScene.h"

GridLayer* GridLayer::create(int width, int height)
{
    auto* obj = new (std::nothrow) GridLayer();
    if (obj && obj->init(width, height))
    {
        obj->autorelease();
        return obj;
    }
    delete obj;
    return nullptr;
}

bool GridLayer::init(int width, int height)
{
    if (!Node::init())
    {
        return false;
    }

    auto lines = ax::DrawNode::create();

    for (int x = 0; x <= width; x++)
    {
        lines->drawSegment(ax::Vec2(x * ObjectChunk::CELL_SIZE, 0.0F),
                           ax::Vec2(x * ObjectChunk::CELL_SIZE, height * ObjectChunk::CELL_SIZE), 1.0F,
                           ax::Color4F(0.0F, 0.0F, 0.0F, 1.0F));
    }

    for (int y = 0; y <= height; y++)
    {
        lines->drawSegment(ax::Vec2(0.0F, y * ObjectChunk::CELL_SIZE),
                           ax::Vec2(width * ObjectChunk::CELL_SIZE, y * ObjectChunk::CELL_SIZE), 1.0F,
                           ax::Color4F(0.0F, 0.0F, 0.0F, 1.0F));
    }

    this->addChild(lines);

    return true;
}