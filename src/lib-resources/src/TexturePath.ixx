module;

#include <LevelTheme.hpp>
#include <stdexcept>
#include <string>

export module TexturePath;

export class TexturePath final
{
public:
    static [[nodiscard]] std::string getTilesetName(TexturePack packId)
    {
        switch (packId)
        {
            using enum TexturePack;

        case SpaceStation:
            return "space_station_tileset.png";

        case CountrySide:
            return "countryside_tileset.png";

        case Neon:
            return "neon_tileset.png";
        }

        throw std::runtime_error(
            "Programmer error: TexturePack id without assigned texture");
    }
};