#include "SaveManager.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

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

bool SaveManager::saveLocalLevel(std::string_view levelString, int levelId)
{
    auto utils           = ax::FileUtils::getInstance();
    std::string filePath = utils->getWritablePath() + SaveManager::SAVE_FILE_NAME;

    if (!utils->isFileExist(filePath))
    {
        initSaveFile();
    }

    std::string content = utils->getStringFromFile(filePath);

    rapidjson::Document doc;
    doc.Parse(content.c_str());

    if (doc.HasParseError() || !doc.IsObject())
    {
        AXLOGE("Error save file parse!");
        return false;
    }

    auto& allocator = doc.GetAllocator();

    if (doc.HasMember("local") && doc["local"].IsArray())
    {
        auto& localLevels = doc["local"];
        bool found        = false;

        for (auto& item : localLevels.GetArray())
        {
            if (item.HasMember("id") && item["id"].GetInt() == levelId)
            {
                item["data"].SetString(levelString.data(), static_cast<rapidjson::SizeType>(levelString.size()),
                                       allocator);
                found = true;
                break;
            }
        }

        if (!found)
        {
            rapidjson::Value newLevel(rapidjson::kObjectType);

            newLevel.AddMember("id", levelId, allocator);
            newLevel.AddMember("song", 1, allocator);  // Ego
            rapidjson::Value levelDataVal;
            levelDataVal.SetString(levelString.data(), static_cast<rapidjson::SizeType>(levelString.size()), allocator);
            newLevel.AddMember("data", levelDataVal, allocator);

            // 0,0,255 (rbg) - Blue
            rapidjson::Value color(rapidjson::kArrayType);
            color.PushBack(0, allocator);
            color.PushBack(0, allocator);
            color.PushBack(255, allocator);

            newLevel.AddMember("color", color, allocator);

            localLevels.PushBack(newLevel, allocator);
        }
    }

    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    doc.Accept(writer);

    return utils->writeStringToFile(sb.GetString(), filePath);
}

std::string SaveManager::loadLocalLevel(int levelId)
{
    auto utils           = ax::FileUtils::getInstance();
    std::string filePath = utils->getWritablePath() + SaveManager::SAVE_FILE_NAME;

    if (!utils->isFileExist(filePath))
    {
        initSaveFile();
        return "";
    }

    std::string content = utils->getStringFromFile(filePath);

    rapidjson::Document doc;
    doc.Parse(content.c_str());

    if (doc.HasParseError() || !doc.IsObject())
    {
        AXLOGE("Error save file parse!");
        return "";
    }

    auto& allocator = doc.GetAllocator();

    if (doc.HasMember("local") && doc["local"].IsArray())
    {
        auto& localLevels = doc["local"];

        for (auto& item : localLevels.GetArray())
        {
            if (item.HasMember("id") && item["id"].GetInt() == levelId)
            {
                return std::string(item["data"].GetString());
            }
        }
    }
    return "";
}