#pragma once
#include <string>

#define MAX_SONG_ID 4

enum class SONG : int
{
    EGO       = 1,
    CLOCKWORK = 2,
    LETSGO    = 3,
    PYRO      = 4,
    MENUSONG  = -1
};

static std::string getSongNameByID(int songId)
{
    switch (static_cast<SONG>(songId))
    {
    case SONG::MENUSONG: return "menu Song";
    case SONG::EGO: return "Ego";
    case SONG::CLOCKWORK: return "Clockwork";
    case SONG::LETSGO: return "Lets Go";
    case SONG::PYRO: return "Pyro";
    default: return "HOW?";
    }
}

static std::string getSongNameWithNumber(int songId)
{
    return std::to_string(songId) + ": " + getSongNameByID(songId);
}

static std::string getSongFileNameByID(int songId)
{
    std::string str = getSongNameByID(songId);
    std::erase(str, ' ');
    return str + ".mp3";
}