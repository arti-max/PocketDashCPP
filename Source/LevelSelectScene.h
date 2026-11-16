#pragma once
#include "axmol.h"
#include "LevelMeta.h"
#include "ui/UILayout.h"
#include "ui/UIListView.h"

class LevelSelectScene : public ax::Scene
{
public:
    LevelSelectScene()  = default;
    ~LevelSelectScene() = default;

    CREATE_FUNC(LevelSelectScene);
    bool init() override;

    ax::ui::Layout* createLevelCard(const LevelMeta& meta, float width);

    void editLevel(int levelId);
    void playLevel(int levelId);
    void deleteLevel(int levelId);
    bool publishLevel(int levelId) { return false; };

    void renameLevel(int levelId, std::string_view name);

private:
    std::vector<LevelMeta> levels;
    ax::ui::Scale9Sprite* panel = nullptr;
    ax::ui::ListView* view      = nullptr;
};