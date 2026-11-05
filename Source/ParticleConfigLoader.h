#pragma
#include "axmol.h"

// ParticleA -
// ParticleB -
// ParticleC - Pad
// ParticleD - Level End
// Particle E,F - Blue/Yellow Portals

class ParticleConfigLoader
{
public:
    static ax::ParticleSystemQuad* createFromJson(std::string_view filename);
};