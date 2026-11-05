#include "ParticleConfigLoader.h"
#include "rapidjson/document.h"

ax::ParticleSystemQuad* ParticleConfigLoader::createFromJson(std::string_view filename)
{
    auto fileUtils      = ax::FileUtils::getInstance();
    std::string path    = fileUtils->fullPathForFilename(filename);
    std::string jsonStr = fileUtils->getStringFromFile(path);

    if (jsonStr.empty())
    {
        AXLOGE("Failed to load particle config: {}", filename);
        return nullptr;
    }

    rapidjson::Document doc;
    doc.Parse(jsonStr.c_str());

    if (doc.HasParseError())
    {
        AXLOGE("JSON parse error: {}", filename);
        return nullptr;
    }

    int maxParticles = doc.HasMember("maxParticles") ? doc["maxParticles"].GetInt() : 100;

    auto* system = ax::ParticleSystemQuad::createWithTotalParticles(maxParticles);
    if (system == nullptr) return nullptr;

    system->setPositionType(ax::ParticleSystem::PositionType::GROUPED);

    if (doc.HasMember("textureFileName"))
    {
        std::string textureName = doc["textureFileName"].GetString();
        auto* texture           = ax::Director::getInstance()->getTextureCache()->addImage(textureName);
        if (texture)
        {
            AXLOGD("Texture {} added!", textureName);
            system->setTexture(texture);
        }
        else
        {
            AXLOGE("Particle texture not found {}", textureName);
        }
    }

    float lifespan = doc.HasMember("particleLifespan") ? doc["particleLifespan"].GetFloat() : 1.0F;
    system->setLife(lifespan);

    if (lifespan > 0.0F)
    {
        system->setEmissionRate(static_cast<float>(maxParticles) / lifespan);
    }

    if (doc.HasMember("duration")) system->setDuration(doc["duration"].GetFloat());
    if (doc.HasMember("particleLifespanVariance")) system->setLifeVar(doc["particleLifespanVariance"].GetFloat());

    if (doc.HasMember("angle")) system->setAngle(doc["angle"].GetFloat());
    if (doc.HasMember("angleVariance")) system->setAngleVar(doc["angleVariance"].GetFloat());

    if (doc.HasMember("startParticleSize")) system->setStartSize(doc["startParticleSize"].GetFloat());
    if (doc.HasMember("finishParticleSize")) system->setEndSize(doc["finishParticleSize"].GetFloat());

    float varX = doc.HasMember("sourcePositionVariancex") ? doc["sourcePositionVariancex"].GetFloat() : 0.0F;
    float varY = doc.HasMember("sourcePositionVariancey") ? doc["sourcePositionVariancey"].GetFloat() : 0.0F;
    system->setPosVar(ax::Vec2(varX, varY));

    int emitterType = doc.HasMember("emitterType") ? doc["emitterType"].GetInt() : 0;
    system->setEmitterMode(static_cast<ax::ParticleSystem::Mode>(emitterType));

    if (emitterType == 0)
    {
        if (doc.HasMember("speed")) system->setSpeed(doc["speed"].GetFloat());
        if (doc.HasMember("speedVariance")) system->setSpeedVar(doc["speedVariance"].GetFloat());
        if (doc.HasMember("radialAcceleration")) system->setRadialAccel(doc["radialAcceleration"].GetFloat());
        if (doc.HasMember("radialAccelVariance")) system->setRadialAccelVar(doc["radialAccelVariance"].GetFloat());
        if (doc.HasMember("tangentialAcceleration"))
            system->setTangentialAccel(doc["tangentialAcceleration"].GetFloat());
        if (doc.HasMember("tangentialAccelVariance"))
            system->setTangentialAccelVar(doc["tangentialAccelVariance"].GetFloat());

        system->setGravity(ax::Vec2::ZERO);
    }
    else
    {
        if (doc.HasMember("maxRadius")) system->setStartRadius(doc["maxRadius"].GetFloat());
        if (doc.HasMember("minRadius")) system->setEndRadius(doc["minRadius"].GetFloat());
        if (doc.HasMember("rotatePerSecond")) system->setRotatePerSecond(doc["rotatePerSecond"].GetFloat());
        if (doc.HasMember("rotatePerSecondVariance"))
            system->setRotatePerSecondVar(doc["rotatePerSecondVariance"].GetFloat());
    }

    ax::Color4F startColor(doc.HasMember("startColorRed") ? doc["startColorRed"].GetFloat() : 1.0F,
                           doc.HasMember("startColorGreen") ? doc["startColorGreen"].GetFloat() : 1.0F,
                           doc.HasMember("startColorBlue") ? doc["startColorBlue"].GetFloat() : 1.0F,
                           doc.HasMember("startColorAlpha") ? doc["startColorAlpha"].GetFloat() : 1.0F);
    system->setStartColor(startColor);
    ax::Color4F finishColor(doc.HasMember("finishColorRed") ? doc["finishColorRed"].GetFloat() : 1.0F,
                            doc.HasMember("finishColorGreen") ? doc["finishColorGreen"].GetFloat() : 1.0F,
                            doc.HasMember("finishColorBlue") ? doc["finishColorBlue"].GetFloat() : 1.0F,
                            doc.HasMember("finishColorAlpha") ? doc["finishColorAlpha"].GetFloat() : 1.0F);
    system->setEndColor(finishColor);

    ax::Color4F startColorVar(
        doc.HasMember("startColorVarianceRed") ? doc["startColorVarianceRed"].GetFloat() : 0.0F,
        doc.HasMember("startColorVarianceGreen") ? doc["startColorVarianceGreen"].GetFloat() : 0.0F,
        doc.HasMember("startColorVarianceBlue") ? doc["startColorVarianceBlue"].GetFloat() : 0.0F,
        doc.HasMember("startColorVarianceAlpha") ? doc["startColorVarianceAlpha"].GetFloat() : 0.0F);
    system->setStartColorVar(startColorVar);

    unsigned int srcBlend = doc.HasMember("blendFuncSource") ? doc["blendFuncSource"].GetUint() : 770;
    unsigned int dstBlend = doc.HasMember("blendFuncDestination") ? doc["blendFuncDestination"].GetUint() : 771;
    system->setBlendFunc(ax::BlendFunc{static_cast<ax::backend::BlendFactor>(srcBlend),
                                       static_cast<ax::backend::BlendFactor>(dstBlend)});

    return system;
}