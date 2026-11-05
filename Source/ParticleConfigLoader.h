#pragma
#include "axmol.h"

class ParticleConfigLoader
{
public:
    static ax::ParticleSystemQuad* createFromJson(std::string_view filename);
};