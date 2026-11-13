#pragma once
#include "axmol.h"
#include "SongData.h"
#include "cocos2d.h"
#include "ui/UILayout.h"
#include "ui/UIWidget.h"
#include <functional>
#include <algorithm>
#include "SongData.h"

using LevelSettingsCallback = std::function<void(int, const ax::Color3B&)>;

class LevelSettingsLayer : public ax::Node
{
public:
    LevelSettingsLayer()  = default;
    ~LevelSettingsLayer() = default;

    CREATE_FUNC(LevelSettingsLayer);

    bool init() override;

    void initSongsSelector();
    void initBGChangeButton();

    void setCallback(const LevelSettingsCallback& c) { this->callback = c; }
    void setSelectedSong(int songId)
    {
        this->selectedSongId = songId;
        if (this->songName != nullptr) this->songName->setString(getSongNameWithNumber(songId));
    }
    void setBackgroundColor(const ax::Color3B& color);

    void onChangeSongBtnTouched(ax::Ref* sender, ax::ui::Widget::TouchEventType type);

private:
    ax::ui::Scale9Sprite* panel = nullptr;

    ax::Label* songName     = nullptr;
    ax::ui::Layout* preview = nullptr;

    LevelSettingsCallback callback;

    int selectedSongId = 1;

    int b_r = 0;
    int b_g = 0;
    int b_b = 0;
};