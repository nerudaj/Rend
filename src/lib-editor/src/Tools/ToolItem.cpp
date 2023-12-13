#include "Tools/ToolItem.hpp"
#include "Commands/CommandHelper.hpp"
#include "Commands/CreateDeleteObjectCommand.hpp"
#include "Commands/MoveObjectCommand.hpp"
#include "Commands/SetObjectPropertyCommand.hpp"
#include "JsonHelper.hpp"
#include <filesystem>

/* Implementing ToolWithDragAndSelect */
std::optional<std::size_t>
ToolItem::getObjectIndexFromMousePos(const sf::Vector2i& pos) const
{
    for (std::size_t i = 0; i < items.size(); i++)
    {
        auto&& clip = sidebarUser.getClip(items[i].id);
        const float x = float(items[i].x);
        const float y = float(items[i].y);
        const float wh = clip.width / 2.f;
        const float hh = clip.height / 2.f;
        if ((x - wh) < pos.x && pos.x < (x + wh) && (y - hh) < pos.y
            && pos.y < (y + hh))
            return i;
    }

    return {};
}

sf::Vector2i ToolItem::getPositionOfObjectWithIndex(std::size_t index) const
{
    return sf::Vector2i(items.at(index).x, items.at(index).y);
}

void ToolItem::selectObjectsInArea(const sf::IntRect& selectedArea)
{
    for (std::size_t i = 0; i < items.size(); i++)
    {
        if (selectedArea.contains(int(items[i].x), int(items[i].y)))
        {
            selectedObjects.insert(i);
        }
    }
}

void ToolItem::moveSelectedObjectsTo(const sf::Vector2i& point)
{
    CommandHelper::moveSelectedObjectsTo(
        items, selectedObjects, dragContext, point, sf::Vector2u(levelSize));
}

void ToolItem::createMoveCommand(
    const sf::Vector2i& src, const sf::Vector2i& dest)
{
    commandQueue->push<MoveItemCommand>(
        items,
        selectedObjects,
        dragContext,
        src,
        dest,
        sf::Vector2u(levelSize));
}

void ToolItem::createDeleteCommand()
{
    commandQueue->push<DeleteItemCommand>(
        items,
        std::vector<std::size_t>(
            selectedObjects.begin(), selectedObjects.end()));
}

/* Rest of ToolItem */
void ToolItem::configure(nlohmann::json& config)
{
    const std::string TOOL_STR = "toolItem";
    const auto rootPath =
        std::filesystem::path(config["configFolder"].get<std::string>());

    auto texturePath = std::filesystem::path(
        config[TOOL_STR]["texturePath"].get<std::string>());
    if (texturePath.is_relative()) texturePath = rootPath / texturePath;
    auto clipPath =
        std::filesystem::path(config[TOOL_STR]["clipPath"].get<std::string>());
    if (clipPath.is_relative()) clipPath = rootPath / clipPath;
    const auto clip = dgm::JsonLoader {}.loadClipFromFile(clipPath);

    configure(
        JsonHelper::arrayToVector2u(
            config["toolMesh"]["texture"]["tileDimensions"]),
        texturePath,
        clip);
}

void ToolItem::configure(
    const sf::Vector2u& tileDimensions,
    const std::filesystem::path& texturePath,
    const dgm::Clip& clip)
{
    items.clear();

    coordConverter = CoordConverter(tileDimensions);

    std::vector<SidebarUserItem::PathRectPair> pathRectPairs;

    for (unsigned i = 0; i < clip.getFrameCount(); i++)
    {
        pathRectPairs.push_back(SidebarUserItem::PathRectPair {
            .texturePath = texturePath, .clip = clip.getFrame(i) });
    }

    sidebarUser.configure(pathRectPairs);

    selectMarker.setOutlineColor(sf::Color::Red);
    selectMarker.setOutlineThickness(2);
    selectMarker.setFillColor(sf::Color::Transparent);
}

void ToolItem::resize(
    unsigned width, unsigned height, bool isTranslationDisabled)
{
    auto&& tileSize = coordConverter.getTileSize();

    const auto newLevelSize = sf::Vector2u(width, height);
    const auto oldLevelSize =
        sf::Vector2u(levelSize.x / tileSize.x, levelSize.y / tileSize.y);

    levelSize.x = int(tileSize.x * width);
    levelSize.y = int(tileSize.y * height);

    if (oldLevelSize.x > newLevelSize.x || oldLevelSize.y > newLevelSize.y
        || isTranslationDisabled)
        return;

    const auto tileOffset = (newLevelSize - oldLevelSize) / 2u;
    const auto offset =
        sf::Vector2u(tileOffset.x * tileSize.x, tileOffset.y * tileSize.y);

    for (auto& item : items)
    {
        item.x += offset.x;
        item.y += offset.y;
    }
}

void ToolItem::shrinkTo(const TileRect& boundingBox)
{
    const auto coordBox = coordConverter.convertTileToCoordRect(boundingBox);

    for (auto&& item : items)
    {
        item.x -= coordBox.left;
        item.y -= coordBox.top;
    }
}

void ToolItem::saveTo(LevelD& lvd) const
{
    validateBeforeSave();

    lvd.things = items;
}

void ToolItem::validateBeforeSave() const
{
    unsigned numSpawns = 0;
    for (auto&& item : items)
    {
        if (static_cast<LevelItemId>(item.id) == LevelItemId::PlayerSpawn)
            ++numSpawns;
    }

    if (numSpawns < 4)
    {
        throw std::runtime_error(
            "Cannot save level! There have to be at least four player spawns "
            "in the level.");
    }
}

void ToolItem::loadFrom(const LevelD& lvd)
{
    levelSize.x = lvd.mesh.layerWidth * coordConverter.getTileSize().x;
    levelSize.y = lvd.mesh.layerHeight * coordConverter.getTileSize().y;
    items = lvd.things;

    for (auto& item : items)
    {
        PropertyTag::get().updateTag(item.tag);
    }
}

void ToolItem::drawTo(tgui::CanvasSFML::Ptr& canvas, uint8_t opacity)
{
    unsigned index = 0;
    sf::RectangleShape outline;

    for (auto& item : items)
    {
        if (item.layerId != getCurrentLayerId()) continue;

        sidebarUser.drawSprite(
            canvas,
            item.id,
            sf::Vector2i(item.x, item.y),
            selectedObjects.contains(index),
            opacity);

        index++;
    }

    if (selecting)
    {
        canvas->draw(selectMarker);
    }
}

void ToolItem::penClicked(const sf::Vector2i& position)
{
    /*
     * If pen is outside level bounds, exit
     * If pen is over item, select it, exit
     * Else create new item and clear selected
     */

    if (not isValidPenPosForDrawing(position)) return;

    const auto itemId = getObjectIndexFromMousePos(position);
    if (itemId.has_value())
    {
        selectedObjects.insert(*itemId);
        return;
    }

    const auto tilePos =
        (sf::Vector2u(position) / coordConverter.getTileSize().x)
            * coordConverter.getTileSize().x
        + coordConverter.getTileSize() / 2u;

    const auto itemToCreate =
        LevelD::Thing { .id = sidebarUser.getPenValue(),
                        .tag = 0,
                        .x = static_cast<uint32_t>(tilePos.x),
                        .y = static_cast<uint32_t>(tilePos.y),
                        .layerId = static_cast<uint32_t>(getCurrentLayerId()),
                        .flags = 0,
                        .metadata = "" };

    commandQueue->push<CreateItemCommand>(items, itemToCreate);

    selectedObjects.clear();
}

ExpectedPropertyPtr ToolItem::getProperty(const sf::Vector2i& penPos) const
{
    const auto itemId = getObjectIndexFromMousePos(penPos);
    if (!itemId) return std::unexpected(BaseError());

    if (items[*itemId].layerId != getCurrentLayerId())
        return std::unexpected(BaseError());

    auto&& result = mem::Box<ItemToolProperty>(
        sidebarUser.getSpriteAsTexture(items.at(*itemId).id),
        *itemId,
        items.at(*itemId));

    return std::move(result);
}

void ToolItem::setProperty(const ToolPropertyInterface& prop)
{
    auto&& property = dynamic_cast<const ItemToolProperty&>(prop);

    PropertyTag::get().updateTag(property.data.tag);

    commandQueue->push<SetObjectPropertyCommand<LevelD::Thing>>(
        items, property.itemId, property.data, sf::Vector2u(levelSize));
}

void ToolItem::buildCtxMenuInternal(tgui::MenuBar::Ptr&) {}

std::optional<GenericObject>
ToolItem::getHighlightedObject(const sf::Vector2i& penPos) const
{
    auto&& prop = getProperty(penPos);
    if (!prop) return {};

    auto&& itemProp = dynamic_cast<ItemToolProperty&>(*(prop.value()));
    return GenericObject { .position = { itemProp.data.x, itemProp.data.y },
                           .tag = itemProp.data.tag };
}

std::vector<sf::Vector2u>
ToolItem::getPositionsOfObjectsWithTag(unsigned tag) const
{
    std::vector<sf::Vector2u> result;
    for (auto&& item : items)
    {
        if (item.tag == tag) result.push_back({ item.x, item.y });
    }

    return result;
}

std::optional<TileRect> ToolItem::getBoundingBox() const noexcept
{
    const auto getPosition = [](const LevelD::Thing& item) -> sf::Vector2i
    { return sf::Vector2i(item.x, item.y); };

    return CommandHelper::getBoundingBox<LevelD::Thing>(
               items, levelSize, getPosition)
        .and_then(
            [this](const CoordRect& b) -> std::optional<TileRect>
            { return coordConverter.convertCoordToTileRect(b); });
}
