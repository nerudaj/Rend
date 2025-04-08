#include "Tools/SidebarUserItem.hpp"
#include "TguiHelper.hpp"

void SidebarUserItem::configure(
    const std::vector<PathRectPair>& textureClipPairs,
    MapCompatibility newMapCompat)
{
    mapCompat = newMapCompat;

    renderData.clear();
    penValue = mapCompat == MapCompatibility::Deathmatch
                   ? std::to_underlying(LevelItemId::PlayerSpawn)
                   : std::to_underlying(LevelItemId::RedSpawn);

    // The only way to make sure penHistory contains one item
    // with correct value. `clear()` inserts zero value after clearing
    penHistory.insert(penValue);
    penHistory.prune(1);

    renderData.reserve(textureClipPairs.size());
    for (auto&& [path, clip] : textureClipPairs)
    {
        renderData.push_back({});
        if (!renderData.back().texture.loadFromFile(path.string()))
        {
            throw std::runtime_error(
                "Could not load texture: " + path.string());
        }

        renderData.back().clip = clip;
    }

    for (auto&& rd : renderData)
    {
        rd.sprite.setTexture(rd.texture);
        rd.sprite.setTextureRect(rd.clip);

        if (rd.clip.width == rd.clip.height)
        {
            rd.guiTexture = TguiHelper::convertTexture(rd.texture, rd.clip);
        }
        else
        {
            const int size = std::max(rd.clip.width, rd.clip.height);

            sf::Image txImg = rd.texture.copyToImage();
            sf::Image sqImg;
            sqImg.create(size, size, sf::Color::Transparent);

            // Copy txImg to sqImg, centered
            unsigned xOffset = (size - rd.clip.width) / 2;
            unsigned yOffset = (size - rd.clip.height) / 2;
            for (int y = 0; y < rd.clip.height; y++)
            {
                for (int x = 0; x < rd.clip.width; x++)
                {
                    sqImg.setPixel(
                        x + xOffset,
                        y + yOffset,
                        txImg.getPixel(x + rd.clip.left, y + rd.clip.top));
                }
            }

            sf::Texture texture;
            texture.loadFromImage(sqImg);
            rd.guiTexture = TguiHelper::convertTexture(texture);
        }
    }
}

void SidebarUserItem::drawSprite(
    tgui::CanvasSFML::Ptr& canvas,
    unsigned spriteId,
    const sf::Vector2i& pos,
    SpawnRotation rotation,
    bool selected,
    float opacity)
{
    auto&& rd = renderData.at(spriteId);

    auto position = sf::Vector2f(pos);
    rd.sprite.setColor(
        sf::Color(255, 255, 255, static_cast<sf::Uint8>(opacity)));
    rd.sprite.setPosition(position);
    rd.sprite.setRotation(std::to_underlying(rotation) * 45.f);
    rd.sprite.setOrigin(rd.clip.width / 2.f, rd.clip.height / 2.f);

    canvas->draw(rd.sprite);

    // Outline for selected items
    outline.setOrigin(rd.clip.width / 2.f, rd.clip.height / 2.f);
    if (selected)
    {
        outline.setPosition(position);
        outline.setSize({ float(rd.clip.width), float(rd.clip.height) });
        canvas->draw(outline);
    }
}
