#include "engine/RenderingEngine.hpp"
#include "utils/VertexArrayBuilder.hpp"
#include <core/EntityTraits.hpp>
#include <numbers>
#include <utils/GameLogicHelpers.hpp>

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

[[nodiscard]] static sf::RectangleShape createSkybox(
    const sf::Texture& texture, unsigned screenWidth, unsigned screenHeight)
{
    auto result = sf::RectangleShape(
        sf::Vector2f(sf::Vector2u(screenWidth * 2, screenHeight / 2)));
    result.setTexture(&texture);
    return result;
}

RenderingEngine::RenderingEngine(
    const RenderSettings& settings,
    const dgm::ResourceManager& resmgr,
    const LevelD& level,
    Scene& scene)
    : settings(settings)
    , scene(scene)
    , tilesetTexture(resmgr.get<sf::Texture>("tileset.png").value().get())
    , tilesetClipping(resmgr.get<dgm::Clip>("tileset.png.clip").value().get())
    , spritesheetTexture(resmgr.get<sf::Texture>("items.png").value().get())
    , spritesheetClipping(resmgr.get<dgm::Clip>("items.png.clip").value().get())
    , weaponSprite(RenderContextBuilder::createWeaponSprite(
          resmgr.get<sf::Texture>("weapons.png").value().get(),
          settings.WIDTH,
          settings.HEIGHT))
    , skyboxSprite(createSkybox(
          resmgr.get<sf::Texture>("skyboxes.png").value().get(),
          settings.WIDTH,
          settings.HEIGHT))
    , weaponClipping(resmgr.get<dgm::Clip>("weapons.png.clip").value().get())
    , shader(resmgr.get<sf::Shader>("shader").value().get())
    , text(RenderContextBuilder::createTextObject(
          resmgr.get<sf::Font>("pico-8.ttf").value().get(),
          32u,
          sf::Color::White))
    , drawableLevel(RenderContextBuilder::buildLevelRepresentation(level))
    , caster(scene.level.bottomMesh.getVoxelSize())
    , depthBuffer(settings.WIDTH)
{
}

void RenderingEngine::update(const float deltaTime)
{
    fpsCounter.update(deltaTime);
    scene.redOverlayIntensity = std::clamp(
        scene.redOverlayIntensity - OVERLAY_INTENSITY_DECAY_RATE * deltaTime,
        0.f,
        255.f);
}

void RenderingEngine::renderWorldTo(dgm::Window&) {}

void RenderingEngine::renderHudTo(dgm::Window& window)
{
    auto&& pov = scene.things[scene.cameraAnchorIdx];

    // Render skybox
    const auto povAngle = dgm::Math::cartesianToPolar(pov.direction).angle;
    const auto w2 = 2 * settings.WIDTH;
    const auto renderPositionX = -povAngle * w2 / 180.f;
    skyboxSprite.setPosition(renderPositionX, 0.f);
    window.draw(skyboxSprite);

    if (renderPositionX > 0)
    {
        skyboxSprite.setPosition(renderPositionX - w2, 0.f);
        window.draw(skyboxSprite);
    }
    else if (renderPositionX + w2 < settings.WIDTH)
    {
        skyboxSprite.setPosition(renderPositionX + w2, 0.f);
        window.draw(skyboxSprite);
    }

    render3d(window);

    text.setString(fpsCounter.getText());
    text.setPosition({ 10.f, 10.f });
    window.draw(text);

    if (pov.typeId == EntityType::Player)
    {
        renderAlivePlayerHud(
            window, pov, scene.playerStates[pov.stateIdx].inventory);
    }
    else
    {
        text.setString("Press [Space] to respawn");
        const auto bounds = text.getGlobalBounds();
        text.setPosition(
            (settings.WIDTH - bounds.width) / 2.f,
            (settings.HEIGHT - bounds.height) / 2.f);
        window.draw(text);
    }
}

void RenderingEngine::render3d(dgm::Window& window)
{
    const auto W = float(scene.level.bottomMesh.getVoxelSize().x);
    const auto& player = scene.things[scene.cameraAnchorIdx];
    const auto pos = player.hitbox.getPosition() / W;
    const auto plane = getPerpendicular(player.direction) * settings.FOV;

#define fireRay(col, isLeftmost, isRightmost)                                  \
    {                                                                          \
        float cameraX = 2 * float(col) / settings.WIDTH - 1;                   \
        auto&& rayDir = player.direction + plane * cameraX;                    \
        depthBuffer[col] =                                                     \
            caster.castRay<isLeftmost, isRightmost>(pos, rayDir, scene);       \
    }

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
    fireRay(0, true, false);
    fireRay(settings.WIDTH - 1, false, true);
    for (unsigned col = 1; col < settings.WIDTH - 1; col++)
    {
        fireRay(col, false, false);
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
            return drawableLevel.bottomTextures[tileId];
        else if (heightHint > 1.f)
            return drawableLevel.upperTextures[tileId];
        return static_cast<int>(TilesetMapping::CeilLow);
    };

    auto&& quads = sf::VertexArray(sf::Triangles); // mesh to render
    const float midHeight = settings.HEIGHT / 2.f;

    auto addFace = [&](const Face& face)
    {
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
                        ? drawableLevel.upperTextures[face.tileId]
                        : drawableLevel.bottomTextures[face.tileId]),
                static_cast<sf::Uint8>(
                    drawableLevel.lightmap[face.neighboringTileId]) },
            tilesetClipping);
    };

    auto addFlat = [&](const Flat& flat)
    {
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

                .brightness = static_cast<sf::Uint8>(
                    drawableLevel.lightmap[flat.tileId]) },
            tilesetClipping);
    };

    auto&& flats = caster.getFlats();
    auto&& faces = caster.getBottomFaces();
    unsigned flatsItr = 0, facesItr = 0;
    while (flatsItr < flats.size() && facesItr < faces.size())
    {
        if (flats[flatsItr].distance > faces[facesItr].distance)
        {
            auto&& flat = flats[flatsItr++];
            addFlat(flat);
        }
        else
        {
            auto&& face = faces[facesItr++];
            addFace(face);
        }
    }
    while (flatsItr < flats.size())
    {
        auto&& flat = flats[flatsItr++];
        addFlat(flat);
    }
    while (facesItr < faces.size())
    {
        auto&& face = faces[facesItr++];
        addFace(face);
    }

    sf::RenderStates states;
    states.texture = &tilesetTexture;
    states.shader = &shader;
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
            TexturedFace {
                .leftColumn = leftColumn,
                .rightColumn = rightColumn,
                .leftHeight = height,
                .rightHeight = height,
                .leftTexHint = hints->first,
                .rightTexHint = hints->second,
                .heightHint = 0.f,
                .textureId = static_cast<std::uint8_t>(thing.textureId),
                .brightness = static_cast<sf::Uint8>(
                    drawableLevel.lightmap.at(sf::Vector2u(thing.center))),
                .flipTexture = thing.flipTexture },
            spritesheetClipping);
    }

    sf::RenderStates states;
    states.texture = &spritesheetTexture;
    states.shader = &shader;
    window.getWindowContext().draw(quads, states);
}

void RenderingEngine::renderAlivePlayerHud(
    dgm::Window& window, const Entity& player, const PlayerInventory& inventory)
{
    auto light = drawableLevel.lightmap.at(sf::Vector2u(
        player.hitbox.getPosition()
        / static_cast<float>(drawableLevel.lightmap.getVoxelSize().x)));

    weaponSprite.setTextureRect(weaponClipping.getFrame(
        static_cast<std::size_t>(inventory.animationContext.spriteClipIndex)));
    weaponSprite.setFillColor(sf::Color { light, light, light });
    window.draw(weaponSprite);

    text.setString(std::format(
        "h: {} a: {}\nw: {}\nammo: {}\ns: {}",
        player.health,
        player.armor,
        inventory.acquiredWeapons.to_string(),
        getAmmoCountForActiveWeapon(inventory),
        inventory.score));
    const auto textBounds = text.getGlobalBounds();
    text.setPosition(10.f, settings.HEIGHT - textBounds.height - 10.f);

    /* FOR AI DEBUGGING
    context.text.setString(
        TOP_STATES_TO_STRING.at(scene.playerStates[0].blackboard->aiTopState)
        + " "
        + AI_STATE_TO_STRING.at(scene.playerStates[0].blackboard->aiState));*/
    window.draw(text);

    auto redOverlay = sf::RectangleShape(sf::Vector2f(window.getSize()));
    redOverlay.setFillColor(
        sf::Color(255, 0, 0, sf::Int8(scene.redOverlayIntensity)));
    window.draw(redOverlay);
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
        if (id == scene.cameraAnchorIdx) [[unlikely]]
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

    while (leftColumn < rightColumn && leftColumn < int(settings.WIDTH)
           && (leftColumn < 0 || depthBuffer[leftColumn] < thingDistance))
    {
        ++leftColumn;
        ++movedLeftColumnBy;
    }

    // Fully obscured or outside of view
    if (leftColumn >= int(settings.WIDTH) || leftColumn >= rightColumn)
        return std::nullopt;

    while (leftColumn < rightColumn && rightColumn >= 0
           && (rightColumn >= int(settings.WIDTH)
               || depthBuffer[rightColumn] < thingDistance))
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
