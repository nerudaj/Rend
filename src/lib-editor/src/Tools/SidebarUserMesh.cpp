#include "Tools/SidebarUserMesh.hpp"

void SidebarUserMesh::configure(
    const std::filesystem::path& texturePath, dgm::Clip&& _clip)
{
    penHistory.clear();
    penValue = 0;

    if (!texture.loadFromFile(texturePath.string()))
        throw std::runtime_error(
            "Cannot load texture file: " + texturePath.string());
    clip = _clip;
}
