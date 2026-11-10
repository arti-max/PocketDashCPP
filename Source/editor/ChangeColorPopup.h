#pragma once
#include "GameObject.h"
#include "axmol.h"
#include "ui/UISlider.h"
#include <functional>
#include <algorithm>

using TriggerCallbackFade = std::function<void(float, int, int, int)>;
using TriggerCallback     = std::function<void(int, int, int)>;

enum class CallbackType : int
{
    INSTANT,
    FADE,
};

class ChangeColorPopup : public ax::Node
{
public:
    ChangeColorPopup()  = default;
    ~ChangeColorPopup() = default;

    static ChangeColorPopup* createWithCallbackFade(const TriggerCallbackFade& callback);
    static ChangeColorPopup* createWithCallback(const TriggerCallback& callback);

    bool initWithFade(const TriggerCallbackFade& callback);
    bool init(const TriggerCallback& callback);

    bool init() override;

    void createSliders();

    void setChangeColors(const ax::Color3B& color);
    void setFade(float fade) { this->fade = fade; };

    void updatePreview();

    void save();

private:
    CallbackType type = CallbackType::INSTANT;
    TriggerCallbackFade fadeCallback;
    TriggerCallback callback;

    std::string dataText;
    ax::Label* label = nullptr;

    ax::ui::Scale9Sprite* panel = nullptr;
    ax::Sprite* preview         = nullptr;

    ax::ui::Slider* rSlider    = nullptr;
    ax::ui::Slider* gSlider    = nullptr;
    ax::ui::Slider* bSlider    = nullptr;
    ax::ui::Slider* fadeSlider = nullptr;

    int c_r    = 0;
    int c_g    = 0;
    int c_b    = 0;
    float fade = 0.5F;
};