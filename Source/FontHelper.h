#pragma once

#define FONT_OVERSAMPLE 2.5F
#define FONT_DIV_SAMPLE (1.0F / FONT_OVERSAMPLE)

static float getFontScaleWithSample(float scale)
{
    return scale * FONT_OVERSAMPLE;
}