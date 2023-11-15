#include "include/Tools/ToolTrigger.hpp"
#include "include/Commands/CommandHelper.hpp"
#include "include/Commands/CreateDeleteObjectCommand.hpp"
#include "include/Commands/MoveObjectCommand.hpp"
#include "include/Commands/SetObjectPropertyCommand.hpp"
#include "include/JsonHelper.hpp"
#include "include/LogConsole.hpp"

/* Implementing ToolWithDragAndSelect */
std::optional<std::size_t>
ToolTrigger::getObjectIndexFromMousePos(const sf::Vector2i& pos) const
{
    for (unsigned i = 0; i < triggers.size(); i++)
    {
        const sf::Vector2i tpos(triggers[i].x, triggers[i].y);
        if (triggers[i].areaType == PenType::Circle)
        {
            if (dgm::Math::vectorSize(sf::Vector2f(pos - tpos))
                < triggers[i].radius)
            {
                return i;
            }
        }
        else if (triggers[i].areaType == PenType::Rectangle)
        {
            if (int(triggers[i].x) <= pos.x
                && pos.x < int(triggers[i].x + triggers[i].width)
                && int(triggers[i].y) <= pos.y
                && pos.y < int(triggers[i].y + triggers[i].height))
            {
                return i;
            }
        }
    }

    return {};
}

sf::Vector2i ToolTrigger::getPositionOfObjectWithIndex(std::size_t index) const
{
    return sf::Vector2i(triggers.at(index).x, triggers.at(index).y);
}

void ToolTrigger::selectObjectsInArea(const sf::IntRect& selectedArea)
{
    for (std::size_t i = 0; i < triggers.size(); i++)
    {
        sf::Vector2i itemCenter;
        itemCenter = triggers[i].areaType == PenType::Circle
                         ? sf::Vector2i(triggers[i].x, triggers[i].y)
                         : sf::Vector2i(
                             triggers[i].x + triggers[i].width / 2,
                             triggers[i].y + triggers[i].height / 2);

        if (selectedArea.contains(itemCenter))
        {
            selectedObjects.insert(i);
        }
    }
}

void ToolTrigger::moveSelectedObjectsTo(const sf::Vector2i& point)
{
    CommandHelper::moveSelectedObjectsTo(
        triggers, selectedObjects, dragContext, point, sf::Vector2u(levelSize));
}

void ToolTrigger::createMoveCommand(
    const sf::Vector2i& src, const sf::Vector2i& dest)
{
    commandQueue->push<MoveTriggerCommand>(
        triggers,
        selectedObjects,
        dragContext,
        src,
        dest,
        sf::Vector2u(levelSize));
}

void ToolTrigger::createDeleteCommand()
{
    commandQueue->push<DeleteTriggerCommand>(
        triggers,
        std::vector<std::size_t>(
            selectedObjects.begin(), selectedObjects.end()));
}

/* Rest of ToolTrigger */

void ToolTrigger::updateVisForTrigger(
    sf::CircleShape& vis, const LevelD::Trigger& trig)
{
    vis.setOrigin(float(trig.radius), float(trig.radius));
    vis.setPosition(float(trig.x), float(trig.y));
    vis.setRadius(float(trig.radius));
}

void ToolTrigger::updateVisForTrigger(
    sf::RectangleShape& vis, const LevelD::Trigger& trig)
{
    vis.setPosition(float(trig.x), float(trig.y));
    vis.setSize({ float(trig.width), float(trig.height) });
}

void ToolTrigger::configureActionsDict(const nlohmann::json& config)
{
    if (!config.is_array()) return;

    unsigned actionDefIndex = 0;
    try
    {
        for (auto&& actionDef : config)
        {
            TriggerActionDefinition def = actionDef;

            if (actionDefinitions->contains(def.id))
            {
                throw std::runtime_error(
                    std::format("Redefinition of action with id {}", def.id));
            }
            else if (def.params.size() > 5)
            {
                throw std::runtime_error(
                    std::format("Params array can have maximum of 5 items"));
            }

            (*actionDefinitions)[def.id] = def;
            ++actionDefIndex;
        }
    }
    catch (std::exception& e)
    {
        throw std::runtime_error(std::format(
            "Error parsing configuration for toolTrigger:\n"
            "When parsing {}th action definition:\n"
            "{}",
            actionDefIndex,
            e.what()));
    }
}

/* Build & Draw */
void ToolTrigger::configure(nlohmann::json& config)
{
    triggers.clear();
    actionDefinitions->clear();

    coordConverter = CoordConverter(JsonHelper::arrayToVector2u(
        config["toolMesh"]["texture"]["tileDimensions"]));

    if (config.contains("toolTrigger")
        && config["toolTrigger"].contains("actionDefinitions"))
    {
        configureActionsDict(config["toolTrigger"]["actionDefinitions"]);
    }
    else
    {
        actionDefinitions->clear();
        (*actionDefinitions)[0] = { .id = 0,
                                    .name = "Not configured",
                                    .params = {} };
    }

    // This is static, outline and fill colors of shapes are dependent on draw
    // opacity
    circShape.setOutlineThickness(2.f);
    rectShape.setOutlineThickness(2.f);

    circMarker.setOutlineColor(sf::Color::Red);
    circMarker.setOutlineThickness(2.f);
    circMarker.setFillColor(sf::Color::Transparent);

    rectMarker.setOutlineColor(sf::Color::Red);
    rectMarker.setOutlineThickness(2.f);
    rectMarker.setFillColor(sf::Color::Transparent);

    selectMarker.setOutlineColor(sf::Color::Red);
    selectMarker.setOutlineThickness(2.f);
    selectMarker.setFillColor(sf::Color::Transparent);
}

void ToolTrigger::resize(
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

    for (auto& trigger : triggers)
    {
        trigger.x += offset.x;
        trigger.y += offset.y;
    }
}

void ToolTrigger::shrinkTo(const TileRect& boundingBox)
{
    const auto coordBox = coordConverter.convertTileToCoordRect(boundingBox);

    for (auto&& trigger : triggers)
    {
        trigger.x -= coordBox.left;
        trigger.y -= coordBox.top;
    }
}

void ToolTrigger::saveTo(LevelD& lvd) const
{
    lvd.triggers = triggers;
}

void ToolTrigger::loadFrom(const LevelD& lvd)
{
    auto&& tileSize = coordConverter.getTileSize();

    levelSize.x = lvd.mesh.layerWidth * tileSize.x;
    levelSize.y = lvd.mesh.layerHeight * tileSize.y;
    triggers = lvd.triggers;

    for (const auto& trig : triggers)
    {
        PropertyTag::get().updateTag(trig.tag);
    }
}

void ToolTrigger::drawTo(tgui::Canvas::Ptr& canvas, uint8_t opacity)
{
    const float opacityFactor = opacity / 255.f;

    circShape.setFillColor(sf::Color(64, 255, 0, uint8_t(128 * opacityFactor)));
    circShape.setOutlineColor(
        sf::Color(64, 255, 0, uint8_t(255 * opacityFactor)));

    rectShape.setFillColor(
        sf::Color(128, 0, 255, uint8_t(128 * opacityFactor)));
    rectShape.setOutlineColor(
        sf::Color(128, 0, 255, uint8_t(255 * opacityFactor)));

    // Render all triggers, highlight selected ones
    for (std::size_t i = 0; i < triggers.size(); i++)
    {
        auto& trig = triggers[i];
        if (trig.layerId != getCurrentLayerId()) continue;

        if (trig.areaType == LevelD::Trigger::AreaType::Circle)
        {
            updateVisForTrigger(circShape, trig);
            canvas->draw(circShape);

            if (selectedObjects.contains(i))
            {
                updateVisForTrigger(circMarker, trig);
                canvas->draw(circMarker);
            }
        }
        else if (trig.areaType == LevelD::Trigger::AreaType::Rectangle)
        {
            updateVisForTrigger(rectShape, trig);
            canvas->draw(rectShape);

            if (selectedObjects.contains(i))
            {
                updateVisForTrigger(rectMarker, trig);
                canvas->draw(rectMarker);
            }
        }
    }

    // If drawing, render preview for drawing
    if (drawing)
    {
        auto&& penPos = getPenPosition();

        if (sidebarUser.getPenType() == PenType::Circle)
        {
            const float radius =
                dgm::Math::vectorSize(sf::Vector2f(penPos - drawStart));
            circShape.setOrigin(radius, radius);
            circShape.setPosition(sf::Vector2f(drawStart));
            circShape.setRadius(radius);
            canvas->draw(circShape);
        }
        else if (sidebarUser.getPenType() == PenType::Rectangle)
        {
            rectShape.setPosition(sf::Vector2f(drawStart));
            rectShape.setSize(sf::Vector2f(penPos - drawStart));
            canvas->draw(rectShape);
        }
    }

    if (selecting)
    {
        canvas->draw(selectMarker);
    }
}

sf::Vector2u ToolTrigger::getNormalizedPosition(const LevelD::Trigger& trigger)
{
    if (trigger.areaType == PenType::Rectangle)
        return sf::Vector2u(trigger.x, trigger.y)
               + sf::Vector2u(trigger.width, trigger.height) / 2u;
    return { trigger.x, trigger.y };
}

/* Pen stuff */
void ToolTrigger::penClicked(const sf::Vector2i& position)
{
    std::size_t id;

    if (!isValidPenPosForDrawing(position)) return;

    auto itemId = getObjectIndexFromMousePos(position);
    if (itemId.has_value())
    {
        if (selectedObjects.contains(*itemId))
            selectedObjects.erase(*itemId);
        else
            selectedObjects.insert(*itemId);
        return;
    }

    if (drawing)
    {
        LevelD::Trigger trigger;
        trigger.areaType = sidebarUser.getPenType();

        if (trigger.areaType == PenType::Circle)
        {
            trigger.x = drawStart.x;
            trigger.y = drawStart.y;
            trigger.radius = uint16_t(
                dgm::Math::vectorSize(sf::Vector2f(position - drawStart)));
        }
        else if (trigger.areaType == PenType::Rectangle)
        {
            trigger.x = std::min(drawStart.x, position.x);
            trigger.y = std::min(drawStart.y, position.y);
            trigger.width = std::abs(position.x - drawStart.x);
            trigger.height = std::abs(position.y - drawStart.y);
        }

        trigger.layerId = static_cast<uint32_t>(getCurrentLayerId());

        commandQueue->push<CreateTriggerCommand>(triggers, trigger);
    }
    else
    {
        drawStart = position;
    }

    drawing = !drawing;
}

void ToolTrigger::penDragCancel(const sf::Vector2i& origin)
{
    drawing = false;
    super::penDragCancel(origin);
}

/* Properties */
ExpectedPropertyPtr ToolTrigger::getProperty(const sf::Vector2i& penPos) const
{
    auto trigId = getObjectIndexFromMousePos(penPos);
    if (!trigId.has_value()) return std::unexpected(BaseError());

    if (triggers[*trigId].layerId != getCurrentLayerId())
        return std::unexpected(BaseError());

    auto&& result =
        Box<TriggerToolProperty>(actionDefinitions, triggers[*trigId], *trigId);

    return std::move(result);
}

std::optional<TileRect> ToolTrigger::getBoundingBox() const noexcept
{
    const auto getPosition =
        [this](const LevelD::Trigger& trigger) -> sf::Vector2i
    { return sf::Vector2i(getNormalizedPosition(trigger)); };

    return CommandHelper::getBoundingBox<LevelD::Trigger>(
               triggers, levelSize, getPosition)
        .and_then(
            [this](const CoordRect& b) -> std::optional<TileRect>
            { return coordConverter.convertCoordToTileRect(b); });
}

void ToolTrigger::buildCtxMenuInternal(tgui::MenuBar::Ptr& menu) {}

void ToolTrigger::setProperty(const ToolPropertyInterface& prop)
{
    auto&& property = dynamic_cast<const TriggerToolProperty&>(prop);

    PropertyTag::get().updateTag(property.data.tag);

    commandQueue->push<SetObjectPropertyCommand<LevelD::Trigger>>(
        triggers, property.id, property.data, sf::Vector2u(levelSize));
}

std::optional<GenericObject>
ToolTrigger::getHighlightedObject(const sf::Vector2i& penPos) const
{
    auto&& prop = getProperty(penPos);
    if (!prop) return {};

    auto&& trigProp = dynamic_cast<TriggerToolProperty&>(*(prop.value()));
    return GenericObject { .position = getNormalizedPosition(trigProp.data),
                           .tag = trigProp.data.tag };
}

std::vector<sf::Vector2u>
ToolTrigger::getPositionsOfObjectsWithTag(unsigned tag) const
{
    std::vector<sf::Vector2u> result;
    for (auto&& trigger : triggers)
    {
        if (trigger.tag == tag)
        {
            result.push_back(getNormalizedPosition(trigger));
        }
    }

    return result;
}
