#include "SaveManager.h"
#include "SongData.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

rapidjson::Value makeJsonString(std::string_view str, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value val;
    val.SetString(str.data(), static_cast<rapidjson::SizeType>(str.size()), allocator);
    return val;
}

std::string SaveManager::getSaveFilePath()
{
    return ax::FileUtils::getInstance()->getWritablePath() + SaveManager::SAVE_FILE_NAME;
}

bool SaveManager::loadDocument(rapidjson::Document& doc)
{
    auto utils           = ax::FileUtils::getInstance();
    std::string filePath = getSaveFilePath();

    if (!utils->isFileExist(filePath))
    {
        initSaveFile();
    }

    std::string content = utils->getStringFromFile(filePath);
    doc.Parse(content.c_str());

    if (doc.HasParseError() || !doc.IsObject())
    {
        AXLOGE("Error save file parse!");
        return false;
    }

    return true;
}

bool SaveManager::saveDocument(const rapidjson::Document& doc)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);

    return ax::FileUtils::getInstance()->writeStringToFile(sb.GetString(), getSaveFilePath());
}

rapidjson::Value* SaveManager::findLocalLevel(rapidjson::Document& doc, int levelId)
{
    if (!doc.HasMember("local") || !doc["local"].IsArray())
    {
        return nullptr;
    }

    for (auto& item : doc["local"].GetArray())
    {
        if (item.HasMember("id") && item["id"].IsInt() && item["id"].GetInt() == levelId)
        {
            return &item;
        }
    }

    return nullptr;
}

void SaveManager::initSaveFile()
{
    auto utils           = ax::FileUtils::getInstance();
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + SaveManager::SAVE_FILE_NAME;

    AXLOGD("Path: {}", filePath);

    if (utils->isFileExist(filePath)) return;

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

    writer.StartObject();

    writer.Key("playername");
    writer.String("player");

    writer.Key("skin");
    writer.Int(1);

    writer.Key("jumps");
    writer.Int(0);

    writer.Key("attempts");
    writer.Int(0);

    writer.Key("local");
    writer.StartArray();
    writer.EndArray();

    writer.Key("online");
    writer.StartArray();
    writer.EndArray();

    writer.EndObject();

    std::string jsonString = sb.GetString();
    utils->writeStringToFile(jsonString, filePath);
}

int SaveManager::initLocalLevel(std::string_view levelName)
{
    rapidjson::Document doc;
    if (!loadDocument(doc)) return -1;

    if (!doc.HasMember("local") || !doc["local"].IsArray()) return -1;

    auto& allocator   = doc.GetAllocator();
    auto& localLevels = doc["local"];

    int maxId = 0;
    for (const auto& item : localLevels.GetArray())
    {
        if (item.IsObject() && item.HasMember("id") && item["id"].IsInt())
        {
            maxId = std::max(maxId, item["id"].GetInt());
        }
    }

    int newId = maxId + 1;

    std::string name = levelName.empty() ? fmt::format("level {}", newId) : std::string(levelName);

    rapidjson::Value newLevel(rapidjson::kObjectType);
    newLevel.AddMember("id", newId, allocator);
    newLevel.AddMember("oid", -1, allocator);
    newLevel.AddMember("song", static_cast<int>(SONG::EGO), allocator);  // Ego
    newLevel.AddMember("name", makeJsonString(name, allocator), allocator);
    newLevel.AddMember("data", makeJsonString("", allocator), allocator);

    // 0,0,255 (rbg) - Blue
    rapidjson::Value color(rapidjson::kArrayType);
    color.PushBack(0, allocator);
    color.PushBack(0, allocator);
    color.PushBack(255, allocator);
    newLevel.AddMember("color", color, allocator);

    localLevels.PushBack(newLevel, allocator);

    if (!saveDocument(doc)) return -1;

    return newId;
}

bool SaveManager::saveLocalLevel(std::string_view levelString, int levelId)
{
    if (levelId < 0)
    {
        levelId = initLocalLevel("");
        if (levelId < 0) return false;
    }

    rapidjson::Document doc;
    if (!loadDocument(doc)) return false;

    auto* level = findLocalLevel(doc, levelId);
    if (level == nullptr)
    {
        AXLOGE("saveLocalLevel: level with id {} not fond!", levelId);
        return false;
    }

    (*level)["data"] = makeJsonString(levelString, doc.GetAllocator());
    return saveDocument(doc);
}

std::string SaveManager::loadLocalLevel(int levelId)
{

    rapidjson::Document doc;
    if (!loadDocument(doc)) return "";

    if (auto* level = findLocalLevel(doc, levelId); level && level->HasMember("data") && (*level)["data"].IsString())
    {
        return (*level)["data"].GetString();
    }
    return "";
}

void SaveManager::setLocalLevelSongId(int levelId, int songId)
{
    rapidjson::Document doc;
    if (!loadDocument(doc)) return;

    auto& allocator = doc.GetAllocator();

    if (auto* level = findLocalLevel(doc, levelId))
    {
        if (level->HasMember("song"))
        {
            (*level)["song"].SetInt(songId);
        }
        else
        {
            level->AddMember("song", songId, doc.GetAllocator());
        }

        saveDocument(doc);
    }
}

int SaveManager::getLocalLevelSongId(int levelId)
{
    rapidjson::Document doc;
    if (!loadDocument(doc)) return 1;

    if (auto* level = findLocalLevel(doc, levelId); level && level->HasMember("song") && (*level)["song"].IsInt())
    {
        return (*level)["song"].GetInt();
    }
    return 1;
}

void SaveManager::setLocalLevelBGColor(int levelId, const ax::Color3B& color)
{
    rapidjson::Document doc;
    if (!loadDocument(doc)) return;

    if (auto* level = findLocalLevel(doc, levelId); level && level->HasMember("color") && (*level)["color"].IsArray())
    {
        auto& allocator = doc.GetAllocator();
        auto& colors    = (*level)["color"];
        colors.Clear();
        colors.PushBack(color.r, allocator);
        colors.PushBack(color.g, allocator);
        colors.PushBack(color.b, allocator);

        saveDocument(doc);
    }
}

ax::Color3B SaveManager::getLocalLevelBGColor(int levelId)
{
    rapidjson::Document doc;
    if (!loadDocument(doc)) return ax::Color3B::BLUE;

    if (auto* level = findLocalLevel(doc, levelId); level && level->HasMember("color") && (*level)["color"].IsArray())
    {
        auto& colors = (*level)["color"];
        if (colors.Size() >= 3)
        {
            return ax::Color3B(colors[0].GetInt(), colors[1].GetInt(), colors[2].GetInt());
        }
    }
    return ax::Color3B::BLUE;
}

std::vector<LevelMeta> SaveManager::getLocalLevels()
{
    std::vector<LevelMeta> result;

    rapidjson::Document doc;
    if (!loadDocument(doc)) return result;

    if (doc.HasMember("local") && doc["local"].IsArray())
    {
        const auto& localLevels = doc["local"].GetArray();
        result.reserve(localLevels.Size());

        for (const auto& item : localLevels)
        {
            if (!item.IsObject() || !item.HasMember("id")) continue;
            if (!item["id"].IsInt()) continue;

            LevelMeta meta;
            meta.id       = item["id"].GetInt();
            meta.onlineId = item.HasMember("oid") ? item["oid"].GetInt() : -1;
            meta.name   = (item.HasMember("name") && item["name"].IsString()) ? item["name"].GetString() : "undefined";
            meta.color  = ax::Color3B(item["color"][0].GetInt(), item["color"][1].GetInt(), item["color"][2].GetInt());
            meta.songId = (item.HasMember("song")) ? item["song"].GetInt() : static_cast<int>(SONG::EGO);

            result.push_back(std::move(meta));
        }
    }

    return result;
}