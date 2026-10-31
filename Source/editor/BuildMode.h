#pragma once

#include <cstdint>
enum class BuildMode : int8_t
{
    BUILD,
    EDIT,
    DELETE,
    SWIPE,
    NONE
};