#pragma once

#include <string>
#include "math/Color.h"
struct LevelMeta
{
    int id;
    int songId;
    ax::Color3B color;
    std::string name;
};