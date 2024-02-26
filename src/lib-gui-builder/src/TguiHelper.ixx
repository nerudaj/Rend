module;

#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

export module TguiHelper;

export class TguiHelper final
{
public:
    [[nodiscard]] static tgui::Texture
    convertTexture(const sf::Texture& texture)
    {
        auto result = tgui::Texture();
        result.loadFromPixelData(
            texture.getSize(), texture.copyToImage().getPixelsPtr());
        return result;
    }

    [[nodiscard]] static tgui::Texture
    convertTexture(const sf::Texture& texture, const sf::IntRect& clipping)
    {
        auto result = tgui::Texture();
        result.loadFromPixelData(
            texture.getSize(),
            texture.copyToImage().getPixelsPtr(),
            tgui::UIntRect(
                clipping.left, clipping.top, clipping.width, clipping.height));
        return result;
    }
};
