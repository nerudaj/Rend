#pragma once

#include <DGM/dgm.hpp>
#include <core/Types.hpp>

struct DrawableLevel
{
    dgm::Mesh bottomTextures;
    dgm::Mesh upperTextures;
    LightmapType lightmap;
};
