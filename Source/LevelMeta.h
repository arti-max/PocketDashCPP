#pragma once

#include <string>
#include "math/Color.h"
struct LevelMeta
{
    int id;
    int onlineId = -1;
    int songId;
    ax::Color3B color;
    std::string name;
};