#pragma once
#include "axmol.h"
#include "LevelMeta.h"

// Что должен делать SaveManager?
/*
1. static класс со static методами
2. Сохранение статистики (username, иконка, прыжки, смерти и т.д.)
3. Сохранение уровней (редактор и онлайн).
4. Всё сохранять в json файл AXGameData.dat
*/
class SaveManager
{
public:
    static constexpr std::string SAVE_FILE_NAME = "AXGameData.dat";

    SaveManager()                              = delete;
    ~SaveManager()                             = delete;
    SaveManager(const SaveManager&)            = delete;
    SaveManager& operator=(const SaveManager&) = delete;

    static bool saveLocalLevel(std::string_view levelString, int levelId);
    static std::string loadLocalLevel(int levelId);
    static int initLocalLevel(std::string_view levelName);

    static std::vector<LevelMeta> getLocalLevels();
    static bool deleteLevel(int levelId);

    static void initSaveFile();

    static bool setUserName(std::string_view username);
    static std::string getUserName();

    static bool setPlayerSkin(int skinId);
    static int getPlayerSkin();
};
