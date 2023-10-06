#include "engine/RenderingEngine.hpp"
#include "utils/VertexArrayBuilder.hpp"
#include <core/EntityTraits.hpp>
#include <numbers>

[[nodiscard]] static std::pair<std::uint8_t, bool> getRotatedSpriteClipId(
    const sf::Vector2f& cameraDir,
    const sf::Vector2f& thingDir,
    std::uint8_t baseClipIndex)
{
    const float angle =
        std::acos(cameraDir.x * thingDir.x + cameraDir.y * thingDir.y);
    const bool flip =
        (-cameraDir.y * thingDir.x + cameraDir.x * thingDir.y) > 0.f;

    if (angle < std::numbers::pi_v<float> / 8.f)
        return { baseClipIndex, false };
    else if (angle < std::numbers::pi_v<float> * 3.f / 8.f)
        return { baseClipIndex + 1u, flip };
    else if (angle < std::numbers::pi_v<float> * 5.f / 8.f)
        return { baseClipIndex + 2u, flip };
    else if (angle < std::numbers::pi_v<float> * 7.f / 8.f)
        return { baseClipIndex + 3u, flip };
    return { baseClipIndex + 4u, false };
}

RenderingEngine::RenderingEngine(
    mem::Rc<const dgm::ResourceManager> _resmgr, Scene& scene)
    : resmgr(_resmgr)
    , scene(scene)
    , tileset(_resmgr->get<sf::Texture>("tileset.png").value().get())
    , settings(RenderSettings {})
    , context(RenderContext::buildRenderContext(
          *resmgr, scene.mapname, settings.WIDTH))
    , caster(scene.level.bottomMesh.getVoxelSize())
{
}

void RenderingEngine::update(const float deltaTime)
{
    fpsCounter.update(deltaTime);
}

void RenderingEngine::renderWorldTo(dgm::Window&) {}

void RenderingEngine::renderHudTo(dgm::Window& window)
{
    if (debugRender)
        render2d(window);
    else
        render3d(window);

    context.text.setString(fpsCounter.getText());
    context.text.setPosition({ 10.f, 10.f });
    window.draw(context.text);

    auto&& pov = scene.things[scene.playerId];
    if (pov.typeId == EntityType::Player)
    {
        auto& inventory = scene.playerStates[pov.stateId].inventory;
        context.text.setString(std::format(
            "H: {} A: {} W: {}",
            pov.health,
            pov.armor,
            inventory.acquiredWeapons.to_string()));

        sf::RectangleShape weapon;
        float size = settings.WIDTH / 3.f;
        weapon.setTexture(&context.weaponHudTexture);
        weapon.setTextureRect(
            context.weaponHudClipping.getFrame(static_cast<std::size_t>(
                inventory.animationContext.spriteClipIndex)));
        weapon.setSize({ size, size });
        weapon.setPosition(
            { settings.WIDTH / 2.f - size / 2.f, settings.HEIGHT - size });
        window.draw(weapon);
    }
    else
    {
        context.text.setString("Press [Space] to respawn");
    }

    context.text.setPosition(200.f, 200.f);
    window.draw(context.text);
}

void RenderingEngine::render2d(dgm::Window& window)
{
    const auto& player = scene.things[scene.playerId];

    // window.draw(context.level);
    player.hitbox.debugRender(window, sf::Color::Red);

    auto W = float(scene.level.bottomMesh.getVoxelSize().x);
    auto pos = player.hitbox.getPosition() / W;
    auto plane = getPerpendicular(player.direction) * settings.FOV;
    caster.prepare();
    for (unsigned col = 0; col < settings.WIDTH; col++)
    {
        float cameraX = 2 * float(col) / settings.WIDTH - 1;
        auto&& rayDir = player.direction + plane * cameraX;
        caster.castRay(pos, rayDir, scene, col == 0, col == settings.WIDTH - 1);
    }

    auto&& lines = sf::VertexArray(sf::Lines);
    VertexObjectBuilder::makeLine(
        lines, pos * W, (pos + player.direction * 10.f) * W, sf::Color::Red);
    for (auto&& face : caster.getBottomFaces())
    {
        auto avg = (face.leftVertex + face.rightVertex) / 2.f;
        VertexObjectBuilder::makeLine(lines, pos * W, avg * W, sf::Color::Blue);
    }

    window.draw(lines);
}

void RenderingEngine::render3d(dgm::Window& window)
{
    const auto W = float(scene.level.bottomMesh.getVoxelSize().x);
    const auto& player = scene.things[scene.playerId];
    const auto pos = player.hitbox.getPosition() / W;
    const auto plane = getPerpendicular(player.direction) * settings.FOV;

    auto fireRay = [&](unsigned col)
    {
        float cameraX = 2 * float(col) / settings.WIDTH - 1;
        auto&& rayDir = player.direction + plane * cameraX;
        context.depthBuffer[col] = caster.castRay(
            pos, rayDir, scene, col == 0, col == settings.WIDTH - 1);
    };

    const float c = dgm::Math::getDotProduct(-player.direction, pos);
    auto getHeight =
        [this, c, player](
            const sf::Vector2f& point) { // Normally this should be divided by
                                         // camera direction vector size, but
                                         // that is guaranteed to be unit
            return settings.HEIGHT
                   / std::abs(
                       (dgm::Math::getDotProduct(point, player.direction) + c));
        };

    auto getColumn = [this, pos, plane, player](const sf::Vector2f& v)
    {
        const auto V = v - pos;
        const auto size = dgm::Math::getSize(V);
        const float k = size / (dgm::Math::getDotProduct(V, player.direction));
        const auto scaledPlane = V / size * k - player.direction;
        bool isXnearZero = std::abs(plane.x) < EPSILON;
        float col =
            isXnearZero ? scaledPlane.y / plane.y : scaledPlane.x / plane.x;
        return int(((col + 1) / 2) * settings.WIDTH);
    };

    /*
     * Leftmost and rightmost rays need to be fired first
     * because the upper level might add multiple faces with
     * a single ray and we need to sanitize them all.
     * By calling the rightmost ray this early, it is guaranteed
     * to add and sanitize these faces in the correct order
     * fixing a display bug that only happened when there were
     * two upper faces per rightmost column at once.
     */
    caster.prepare();
    fireRay(0);
    fireRay(settings.WIDTH - 1);
    for (unsigned col = 1; col < settings.WIDTH - 1; col++)
    {
        fireRay(col);
    }
    caster.finalize();

    renderLevelMesh(window, getHeight, getColumn);
    renderSprites(window, pos, player.direction, getHeight, getColumn);
}

void RenderingEngine::renderLevelMesh(
    dgm::Window& window,
    std::function<float(const sf::Vector2f&)> getHeight,
    std::function<int(const sf::Vector2f&)> getColumn)
{
    auto vertexToScreenSpaceFlat = [&](const sf::Vector2f& vertex) {
        return sf::Vector2f { float(getColumn(vertex)), getHeight(vertex) };
    };

    auto getFlatTexture = [&](unsigned tileId, float heightHint)
    {
        if (heightHint < 0.f)
            return context.level.bottomTextures[tileId];
        else if (heightHint > 1.f)
            return static_cast<int>(TilesetMapping::CeilSky);
        return static_cast<int>(TilesetMapping::CeilLow);
    };

    const float midHeight = settings.HEIGHT / 2.f;

    auto&& quads = sf::VertexArray(sf::Triangles); // mesh to render
    auto&& flats = caster.getFlats();
    auto&& faces = caster.getBottomFaces();
    unsigned flatsItr = 0, facesItr = 0;
    while (flatsItr < flats.size() && facesItr < faces.size())
    {
        if (flats[flatsItr].distance > faces[facesItr].distance)
        {
            auto&& flat = flats[flatsItr++];
            VertexObjectBuilder::makeFlat(
                quads,
                midHeight,
                TexturedFlat {
                    .vertices = { vertexToScreenSpaceFlat(flat.vertices[0]),
                                  vertexToScreenSpaceFlat(flat.vertices[1]),
                                  vertexToScreenSpaceFlat(flat.vertices[2]),
                                  vertexToScreenSpaceFlat(flat.vertices[3]) },
                    .heightHint = flat.heightHint,
                    .textureId = static_cast<uint8_t>(
                        getFlatTexture(flat.tileId, flat.heightHint)),
                    .brightness = sf::Uint8 { 255 } },
                context.tilesetClipping);
        }
        else
        {
            auto&& face = faces[facesItr++];
            VertexObjectBuilder::makeFace(
                quads,
                midHeight,
                TexturedFace {
                    getColumn(face.leftVertex),
                    getColumn(face.rightVertex),
                    getHeight(face.leftVertex),
                    getHeight(face.rightVertex),
                    face.leftTexHint,
                    face.rightTexHint,
                    face.heightHint,
                    static_cast<uint8_t>(
                        face.heightHint > 0.f
                            ? context.level.upperTextures[face.tileId]
                            : context.level.bottomTextures[face.tileId]),
                    face.side == 0 ? sf::Uint8 { 255 } : sf::Uint8 { 128 } },
                context.tilesetClipping);
        }
    }
    while (flatsItr < flats.size())
    {
        auto&& flat = flats[flatsItr++];
        VertexObjectBuilder::makeFlat(
            quads,
            midHeight,
            TexturedFlat {
                .vertices = { vertexToScreenSpaceFlat(flat.vertices[0]),
                              vertexToScreenSpaceFlat(flat.vertices[1]),
                              vertexToScreenSpaceFlat(flat.vertices[2]),
                              vertexToScreenSpaceFlat(flat.vertices[3]) },
                .heightHint = flat.heightHint,
                .textureId = static_cast<uint8_t>(
                    getFlatTexture(flat.tileId, flat.heightHint)),
                .brightness = sf::Uint8 { 255 } },
            context.tilesetClipping);
    }
    while (facesItr < faces.size())
    {
        auto&& face = faces[facesItr++];
        VertexObjectBuilder::makeFace(
            quads,
            midHeight,
            TexturedFace { .leftColumn = getColumn(face.leftVertex),
                           .rightColumn = getColumn(face.rightVertex),
                           .leftHeight = getHeight(face.leftVertex),
                           .rightHeight = getHeight(face.rightVertex),
                           .leftTexHint = face.leftTexHint,
                           .rightTexHint = face.rightTexHint,
                           .heightHint = face.heightHint,
                           .textureId = static_cast<uint8_t>(
                               face.heightHint > 0.f
                                   ? context.level.upperTextures[face.tileId]
                                   : context.level.bottomTextures[face.tileId]),
                           .brightness = face.side == 0 ? sf::Uint8 { 255 }
                                                        : sf::Uint8 { 128 },
                           .flipTexture = false },
            context.tilesetClipping);
    }

    sf::RenderStates states;
    states.texture = &context.tilesetTexture;
    states.shader = &context.shader;
    window.getWindowContext().draw(quads, states);
}

void RenderingEngine::renderSprites(
    dgm::Window& window,
    const sf::Vector2f& cameraPosition,
    const sf::Vector2f& cameraDirection,
    std::function<float(const sf::Vector2f&)> getHeight,
    std::function<int(const sf::Vector2f&)> getColumn)
{
    const auto&& midHeight = settings.HEIGHT / 2.f;
    const auto&& thingPlane = getPerpendicular(cameraDirection) * 0.5f;

    const auto&& thingsToRender = getFilteredAndOrderedThingsToRender(
        caster.getVisibleThingIds(), cameraPosition, cameraDirection);

    auto&& quads = sf::VertexArray(sf::Triangles); // mesh to render
    for (auto&& thing : thingsToRender)
    {
        const auto height = getHeight(thing.center);
        auto leftColumn = getColumn(thing.center - thingPlane);
        auto rightColumn = getColumn(thing.center + thingPlane);

        const auto hints =
            cropSpriteIfObscured(leftColumn, rightColumn, thing.distance);
        if (!hints) [[unlikely]]
            continue;

        VertexObjectBuilder::makeFace(
            quads,
            midHeight,
            TexturedFace { .leftColumn = leftColumn,
                           .rightColumn = rightColumn,
                           .leftHeight = height,
                           .rightHeight = height,
                           .leftTexHint = hints->first,
                           .rightTexHint = hints->second,
                           .heightHint = 0.f,
                           .textureId =
                               static_cast<std::uint8_t>(thing.textureId),
                           .brightness = 255,
                           .flipTexture = thing.flipTexture },
            context.spritesheetClipping);
    }

    sf::RenderStates states;
    states.texture = &context.spritesheetTexture;
    states.shader = &context.shader;
    window.getWindowContext().draw(quads, states);
}

std::vector<RenderingEngine::ThingToRender>
RenderingEngine::getFilteredAndOrderedThingsToRender(
    const std::vector<std::size_t>& candidateIds,
    const sf::Vector2f& cameraPosition,
    const sf::Vector2f& cameraDirection)
{
    const auto&& W = float(scene.level.bottomMesh.getVoxelSize().x);

    auto&& result = std::vector<ThingToRender>();
    result.reserve(candidateIds.size());

    for (auto&& id : candidateIds)
    {
        // Happens only once per array
        if (id == scene.playerId) [[unlikely]]
            continue;

        auto&& thing = scene.things[id];

        const auto& thingPosition = thing.hitbox.getPosition() / W;
        const auto [textureId, flipTexture] =
            isDirectional(thing.typeId)
                ? getRotatedSpriteClipId(
                    cameraDirection,
                    -thing.direction,
                    static_cast<std::uint8_t>(
                        thing.animationContext.spriteClipIndex))
                : std::pair { static_cast<std::uint8_t>(
                                  thing.animationContext.spriteClipIndex),
                              false };

        result.push_back(ThingToRender {
            .distance = dgm::Math::getSize(thingPosition - cameraPosition),
            .center = thingPosition,
            .textureId = textureId,
            .flipTexture = flipTexture });
    }

    std::sort(
        result.begin(),
        result.end(),
        [](auto& a, auto& b) -> bool { return a.distance > b.distance; });

    return result;
}

std::optional<std::pair<float, float>> RenderingEngine::cropSpriteIfObscured(
    int& leftColumn, int& rightColumn, float thingDistance)
{
    const auto originalWidth = rightColumn - leftColumn + 1;
    int movedLeftColumnBy = 0;
    int movedRightColumnBy = 0;

    if (leftColumn >= int(settings.WIDTH) || rightColumn < 0)
        return std::nullopt;

    while (
        leftColumn < rightColumn && leftColumn < int(settings.WIDTH)
        && (leftColumn < 0 || context.depthBuffer[leftColumn] < thingDistance))
    {
        ++leftColumn;
        ++movedLeftColumnBy;
    }

    // Fully obscured or outside of view
    if (leftColumn >= int(settings.WIDTH) || leftColumn >= rightColumn)
        return std::nullopt;

    while (leftColumn < rightColumn && rightColumn >= 0
           && (rightColumn >= int(settings.WIDTH)
               || context.depthBuffer[rightColumn] < thingDistance))
    {
        --rightColumn;
        ++movedRightColumnBy;
    }

    // Fully obscured or outside of view
    if (leftColumn >= rightColumn || rightColumn < 0) return std::nullopt;

    return std::pair {
        movedLeftColumnBy / static_cast<float>(originalWidth),
        1.f - movedRightColumnBy / static_cast<float>(originalWidth)
    };
}
