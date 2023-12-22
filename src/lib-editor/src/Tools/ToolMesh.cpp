#include "Tools/ToolMesh.hpp"
#include "Commands/ResizeCommand.hpp"
#include "Commands/SetTileAreaCommand.hpp"
#include "Commands/SetTileCommand.hpp"
#include "Commands/SetTilePropertyCommand.hpp"
#include "JsonHelper.hpp"
#include "Utilities/Utilities.hpp"
#include <LevelItemId.hpp>
#include <filesystem>
#include <ranges>

void ToolMesh::penClicked(const sf::Vector2i& position)
{
    const auto tilePos = worldToTilePos(position);
    if (isPositionValid(tilePos)
        && sidebarUser.getPenValue() != getMap().getTileValue(tilePos))
    {
        commandQueue->push<SetTileCommand>(
            getMap(),
            tilePos,
            sidebarUser.getPenValue(),
            defaultBlocks.at(sidebarUser.getPenValue()));
    }
}

void ToolMesh::penDragStarted(const sf::Vector2i& start)
{
    dragging = true;
    rectShape.setPosition(sf::Vector2f(start));
}

void ToolMesh::penDragUpdate(const sf::Vector2i& start, const sf::Vector2i& end)
{
    if (mode == DrawMode::Pencil)
    {
        penClicked(end);
    }
    else
    {
        rectShape.setSize(sf::Vector2f(end - start));
    }
}

void ToolMesh::penDragEnded(const sf::Vector2i& start, const sf::Vector2i& end)
{
    if (!dragging) return;
    dragging = false;

    const auto startTile = worldToTilePos(Utilities::clipNegativeCoords(start));
    const auto endTile = worldToTilePos(Utilities::clipNegativeCoords(end));
    if (mode == DrawMode::RectEdge)
    {
        constexpr bool DONT_FILL = false;
        commandQueue->push<SetTileAreaCommand>(
            getMap(),
            startTile,
            endTile,
            sidebarUser.getPenValue(),
            defaultBlocks.at(sidebarUser.getPenValue()),
            DONT_FILL);
    }
    else if (mode == DrawMode::RectFill)
    {
        constexpr bool FILL = true;
        commandQueue->push<SetTileAreaCommand>(
            getMap(),
            startTile,
            endTile,
            sidebarUser.getPenValue(),
            defaultBlocks.at(sidebarUser.getPenValue()),
            FILL);
    }
}

void ToolMesh::configure(nlohmann::json& config)
{
    const std::string TOOL_STR = "toolMesh";
    const auto rootPath =
        std::filesystem::path(config["configFolder"].get<std::string>());

    auto texturePath = std::filesystem::path(
        config[TOOL_STR]["texture"]["path"].get<std::string>());
    if (texturePath.is_relative()) texturePath = rootPath / texturePath;

    sf::Vector2u tileDims = JsonHelper::arrayToVector2u(
        config[TOOL_STR]["texture"]["tileDimensions"]);
    sf::Vector2u tileSpacing =
        JsonHelper::arrayToVector2u(config[TOOL_STR]["texture"]["tileSpacing"]);
    sf::IntRect bounds =
        JsonHelper::arrayToIntRect(config[TOOL_STR]["texture"]["boundaries"]);

    std::vector<bool> blocks(
        config[TOOL_STR]["defaultProperties"]["count"], false);

    unsigned i = 0;
    for (auto& item : config[TOOL_STR]["defaultProperties"]["solids"])
    {
        blocks[i++] = item.get<int>() > 0;
    }

    configure(texturePath, tileDims, tileSpacing, bounds, blocks);
}

void ToolMesh::copySourceRectToTarget(
    DrawableLeveldMesh& sourceMap,
    const sf::Vector2u& start,
    const sf::Vector2u& end,
    const sf::Vector2i& translation,
    std::vector<int>& targetTileValues,
    std::vector<int>& targetSolidValues,
    unsigned targetWidth)
{
    for (unsigned y = start.y; y < end.y; y++)
    {
        for (unsigned x = start.x; x < end.x; x++)
        {
            const unsigned targetX = x + translation.x;
            const unsigned targetY = y + translation.y;
            const unsigned targetI = targetY * targetWidth + targetX;

            targetTileValues[targetI] = sourceMap.getTileValue({ x, y });
            targetSolidValues[targetI] = sourceMap.isTileSolid({ x, y });
        }
    }
}

void ToolMesh::resize(
    unsigned width, unsigned height, bool isTranslationDisabled)
{
    const bool upscalingX = width > getMap().getMapDimensions().x;
    const bool upscalingY = height > getMap().getMapDimensions().y;
    const unsigned offsetX =
        upscalingX ? (width - getMap().getMapDimensions().x) / 2u : 0;
    const unsigned offsetY =
        upscalingY ? (height - getMap().getMapDimensions().y) / 2u : 0;

    const sf::Vector2u end(
        std::min(width, getMap().getMapDimensions().x),
        std::min(height, getMap().getMapDimensions().y));

    for (auto&& map : maps)
    {
        auto tileValues = std::vector<int>(width * height, 0);
        auto solidValues = std::vector<int>(width * height, 0);

        copySourceRectToTarget(
            map,
            { 0u, 0u },
            end,
            isTranslationDisabled ? sf::Vector2i { 0, 0 }
                                  : sf::Vector2i(offsetX, offsetY),
            tileValues,
            solidValues,
            width);

        map.build(tileValues, solidValues, { width, height });
    }
}

void ToolMesh::build(unsigned width, unsigned height)
{
    for (unsigned i = 0; i < maps.size(); i++)
    {
        maps[i].build(
            std::vector<int>(
                width * height,
                static_cast<int>(
                    i == 0 ? LevelTileId::Flat1 : LevelTileId::CeilSky)),
            std::vector<int>(width * height, 0),
            { width, height });
    }
}

void ToolMesh::shrinkTo(const TileRect& boundingBox)
{
    const auto width = boundingBox.right - boundingBox.left + 1;
    const auto height = boundingBox.bottom - boundingBox.top + 1;

    const sf::Vector2i translation(boundingBox.left, boundingBox.top);

    for (auto&& map : maps)
    {
        auto tileValues = std::vector<int>(width * height, 0);
        auto solidValues = std::vector<int>(width * height, 0);

        copySourceRectToTarget(
            map,
            { boundingBox.left, boundingBox.top },
            { boundingBox.right + 1, boundingBox.bottom + 1 },
            -translation,
            tileValues,
            solidValues,
            width);

        map.build(tileValues, solidValues, { width, height });
    }
}

void ToolMesh::saveTo(LevelD& lvd) const
{
    lvd.mesh.tileWidth = sidebarUser.getClip().getFrameSize().x;
    lvd.mesh.tileHeight = sidebarUser.getClip().getFrameSize().y;
    lvd.mesh.layerWidth = getMap().getMapDimensions().x;
    lvd.mesh.layerHeight = getMap().getMapDimensions().y;

    const std::size_t numItemsPerLayer =
        lvd.mesh.layerWidth * lvd.mesh.layerHeight;

    lvd.mesh.layers.resize(maps.size());
    for (unsigned l = 0; l < maps.size(); l++)
    {
        auto& layer = lvd.mesh.layers[l];
        layer.tiles.resize(numItemsPerLayer);
        layer.blocks.resize(numItemsPerLayer);

        for (unsigned tileY = 0, i = 0; tileY < lvd.mesh.layerHeight; tileY++)
        {
            for (unsigned tileX = 0; tileX < lvd.mesh.layerWidth; tileX++, i++)
            {
                const auto coord = sf::Vector2u(tileX, tileY);
                layer.tiles[i] = maps[l].getTileValue(coord);
                layer.blocks[i] = maps[l].isTileSolid(coord);
            }
        }
    }
}

void ToolMesh::loadFrom(const LevelD& lvd)
{
    assert(maps.size() == getLayerCount());
    if (lvd.mesh.layers.size() > getLayerCount())
        throw std::runtime_error(std::format(
            "Level has too many layers ({}), editor only supports up to {}",
            lvd.mesh.layers.size(),
            getLayerCount()));

    const auto dataSize =
        sf::Vector2u(lvd.mesh.layerWidth, lvd.mesh.layerHeight);

    for (unsigned i = 0; i < lvd.mesh.layers.size(); i++)
    {
        const auto tilemapData = std::vector<int>(
            lvd.mesh.layers[i].tiles.begin(), lvd.mesh.layers[i].tiles.end());
        const auto collisionData = std::vector<int>(
            lvd.mesh.layers[i].blocks.begin(), lvd.mesh.layers[i].blocks.end());

        maps[i].build(tilemapData, collisionData, dataSize);
    }

    for (unsigned i = lvd.mesh.layers.size(); i < maps.size(); i++)
    {
        const auto dataSizeMul = dataSize.x * dataSize.y;
        const auto emptyData = std::vector<int>(dataSizeMul, 0);
        maps[i].build(emptyData, emptyData, dataSize);
    }
}

void ToolMesh::drawTo(
    tgui::CanvasSFML::Ptr& canvas, std::size_t layerIdx, uint8_t)
{
    const bool isCurrentLayer = layerIdx == getCurrentLayerIdx();

    maps[layerIdx].drawTo(canvas, enableOverlay, isCurrentLayer);

    if (isCurrentLayer)
    {
        const bool areaDrawMode =
            mode == DrawMode::RectEdge || mode == DrawMode::RectFill;

        if (dragging && areaDrawMode)
        {
            canvas->draw(rectShape);
        }
    }
}

ExpectedPropertyPtr ToolMesh::getProperty(const sf::Vector2i& penPos) const
{
    auto tilePos = worldToTilePos(penPos);
    if (not isPositionValid(tilePos)) return std::unexpected(BaseError());

    auto tileValue = getMap().getTileValue(tilePos);

    auto&& result = mem::Box<MeshToolProperty>(
        sidebarUser.getSpriteAsTexture(tileValue),
        tilePos.x,
        tilePos.y,
        static_cast<uint32_t>(getCurrentLayerIdx()),
        tileValue,
        getMap().isTileSolid(tilePos),
        defaultBlocks[tileValue]);

    return std::move(result);
}

void ToolMesh::setProperty(const ToolPropertyInterface& prop)
{
    auto&& property = dynamic_cast<const MeshToolProperty&>(prop);

    commandQueue->push<SetTilePropertyCommand>(
        getMap(),
        sf::Vector2u(property.tileX, property.tileY),
        property.tileValue,
        property.blocking);
}

void ToolMesh::buildCtxMenuInternal(tgui::MenuBar::Ptr& menu)
{
    const std::string OPTION_PENCIL = "Pencil Mode (Shift+P)";
    const std::string OPTION_FILL = "Rect-fill Mode (Shift+F)";
    const std::string OPTION_EDGE = "Rect-edge Mode (Shift+E)";
    const std::string OPTION_OVERLAY = "Toggle overlay (Shift+O)";

    addCtxMenuItem(
        menu,
        OPTION_PENCIL,
        [this] { changeDrawingMode(DrawMode::Pencil); },
        sf::Keyboard::P);
    addCtxMenuItem(
        menu,
        OPTION_FILL,
        [this] { changeDrawingMode(DrawMode::RectFill); },
        sf::Keyboard::F);
    addCtxMenuItem(
        menu,
        OPTION_EDGE,
        [this] { changeDrawingMode(DrawMode::RectEdge); },
        sf::Keyboard::E);
    addCtxMenuItem(
        menu, OPTION_OVERLAY, [this] { toggleOverlay(); }, sf::Keyboard::O);
}

std::optional<TileRect> ToolMesh::getBoundingBox() const noexcept
{
    bool nonZeroTileDetected = false;
    TileRect result;

    auto updateBox = [&](unsigned x, unsigned y, const DrawableLeveldMesh& map)
    {
        if (map.getTileValue({ x, y }) == 0) return;

        [[likely]] if (nonZeroTileDetected)
        {
            result.left = std::min(result.left, x);
            result.top = std::min(result.top, y);
            result.right = std::max(result.right, x);
            result.bottom = std::max(result.bottom, y);
        }
        else
        {
            nonZeroTileDetected = true;
            result = { .left = x, .top = y, .right = x, .bottom = y };
        }
    };

    for (auto&& map : maps)
    {
        for (unsigned y = 0; y < map.getMapDimensions().y; y++)
        {
            for (unsigned x = 0; x < map.getMapDimensions().x; x++)
            {
                updateBox(x, y, map);
            }
        }
    }

    if (nonZeroTileDetected) return result;
    return {};
}

void ToolMesh::configure(
    const std::filesystem::path& texturePath,
    const sf::Vector2u& frameSize,
    const sf::Vector2u& frameSpacing,
    const sf::IntRect& textureBounds,
    const std::vector<bool>& defaultBlockSetting)
{
    defaultBlocks = defaultBlockSetting;

    sidebarUser.configure(
        texturePath, dgm::Clip(frameSize, textureBounds, 0, frameSpacing));

    maps.clear();
    maps.resize(getLayerCount());
    for (auto&& map : maps)
        map =
            DrawableLeveldMesh(sidebarUser.getTexture(), sidebarUser.getClip());

    rectShape.setOutlineColor(sf::Color(255, 0, 0, 128));
    rectShape.setOutlineThickness(2.f);
}

void ToolMesh::changeDrawingMode(ToolMesh::DrawMode newMode)
{
    mode = newMode;
    if (mode == DrawMode::RectEdge)
    {
        rectShape.setFillColor(sf::Color::Transparent);
    }
    else if (mode == DrawMode::RectFill)
    {
        rectShape.setFillColor(sf::Color(128, 0, 0, 128));
    }
}

void ToolMesh::sanitizeBeforeSave()
{
    for (auto&& map : maps)
    {
        for (unsigned y = 0; y < map.getMapDimensions().y; y++)
        {
            for (unsigned x = 0; x < map.getMapDimensions().x; x++)
            {
                // Skip non-border
                if (!(y == 0 || x == 0 || y == (map.getMapDimensions().y - 1)
                      || x == (map.getMapDimensions().x - 1)))
                {
                    continue;
                }

                map.setTileSolid({ x, y }, true);
            }
        }
    }
}

void ToolMesh::toggleOverlay()
{
    enableOverlay = !enableOverlay;
}

std::string std::to_string(ToolMesh::DrawMode mode)
{
    if (mode == ToolMesh::DrawMode::Pencil)
        return "Pencil";
    else if (mode == ToolMesh::DrawMode::RectFill)
        return "RectFill";
    else if (mode == ToolMesh::DrawMode::RectEdge)
        return "RectEdge";
    else if (mode == ToolMesh::DrawMode::Line)
        return "Line";
    return "Error";
}
