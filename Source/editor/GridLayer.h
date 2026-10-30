#pragma once
#include "axmol.h"

class GridLayer : public ax::Node
{
public:
    // width and height in units (1 unit = 1 block)
    static GridLayer* create(int width, int height);
    bool init(int width, int height);

    GridLayer()  = default;
    ~GridLayer() = default;

    ax::Vec2 snapToGrid(float x, float y);
};