#include "engine/RenderingEngine.hpp"
#include "builder/VertexArrayBuilder.hpp"
#include <Configs/Strings.hpp>
#include <builder/RenderContextBuilder.hpp>
#include <core/EntityDefinitions.hpp>
#include <numbers>
#include <ranges>
#include <utils/MathHelpers.hpp>

import TexturePath;

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

[[nodiscard, msvc::intrinsic]] constexpr static auto
spriteIdToIndex(SpriteId id) noexcept
{
    return static_cast<std::underlying_type_t<SpriteId>>(id);
}

RenderingEngine::RenderingEngine(
    const DisplayOptions& settings,
    const dgm::ResourceManager& resmgr,
    Scene& scene)
    : settings(settings)
    , scene(scene)
    , tileset { .texture = resmgr
                               .get<sf::Texture>(TexturePath::getTilesetName(
                                   scene.level.texturePack))
                               .value()
                               .get(),
                .clipping =
                    resmgr
                        .get<dgm::Clip>(
                            TexturePath::getTilesetName(scene.level.texturePack)
                            + ".clip")
                        .value()
                        .get() }
    , spritesheet { .texture =
                        resmgr.get<sf::Texture>("sprites.png").value().get(),
                    .clipping = resmgr.get<dgm::Clip>("sprites.png.clip")
                                    .value()
                                    .get() }
    , weapon { .sprite = RenderContextBuilder::createWeaponSprite(
                   resmgr.get<sf::Texture>("weapons.png").value().get(),
                   settings.resolution.width,
                   settings.resolution.height),
               .clipping =
                   resmgr.get<dgm::Clip>("weapons.png.clip").value().get() }
    , skybox { .sprite = RenderContextBuilder::createSkybox(
                   resmgr.get<sf::Texture>("skyboxes.png").value().get(),
                   settings.resolution.width,
                   settings.resolution.height),
               .clipping =
                   resmgr.get<dgm::Clip>("skyboxes.png.clip").value().get() }
    , hud { .sprite = RenderContextBuilder::createHudIcon(
                resmgr.get<sf::Texture>("hud.png").value().get(),
                settings.resolution.width),
            .clipping = resmgr.get<dgm::Clip>("hud.png.clip").value().get() }
    , shader(resmgr.getMutable<sf::Shader>("shader").value().get())
    , noiseTexture(resmgr.get<sf::Texture>("ditherNoise.png").value().get())
    , text(RenderContextBuilder::createTextObject(
          resmgr.get<sf::Font>("pico-8.ttf").value().get(),
          32u,
          sf::Color::White))
    , caster(scene.level.bottomMesh.getVoxelSize())
    , depthBuffer(settings.resolution.width)
{
    shader.setUniform("noise", noiseTexture);
    shader.setUniform("shadeColor", sf::Glsl::Vec4(sf::Color(0, 0, 0)));
    shader.setUniform(
        "resolution",
        sf::Glsl::Vec2(
            static_cast<float>(settings.resolution.width),
            static_cast<float>(settings.resolution.height)));
    shader.setUniform("ditherShadows", settings.useDitheredShadows);
    skybox.sprite.setTextureRect(skybox.clipping.getFrame(
        static_cast<std::underlying_type_t<SkyboxTheme>>(
            scene.level.skyboxTheme)));
}

void RenderingEngine::update(const float deltaTime)
{
    fpsCounter.update(deltaTime);
    scene.camera.redOverlayIntensity = std::clamp(
        scene.camera.redOverlayIntensity
            - OVERLAY_INTENSITY_DECAY_RATE * deltaTime,
        0.f,
        255.f);
}

void RenderingEngine::renderTo(dgm::Window& window)
{
    auto&& pov = scene.things[scene.camera.anchorIdx];

    renderSkybox(window, dgm::Math::cartesianToPolar(pov.direction).angle);
    renderWorld(window);
    renderFps(window);

    if (pov.typeId == EntityType::Player)
    {
        renderPlayerHud(
            window, pov, scene.playerStates[pov.stateIdx].inventory);
    }
    else
    {
        renderRespawnPrompt(window);
    }
}

void RenderingEngine::renderSkybox(dgm::Window& window, const float angle)
{
    const auto w2 = 2 * settings.resolution.width;
    const auto renderPositionX = -angle * w2 / 180.f;
    skybox.sprite.setPosition(renderPositionX, 0.f);
    window.draw(skybox.sprite);

    if (renderPositionX > 0)
    {
        skybox.sprite.setPosition(renderPositionX - w2, 0.f);
        window.draw(skybox.sprite);
    }
    else if (renderPositionX + w2 < settings.resolution.width)
    {
        skybox.sprite.setPosition(renderPositionX + w2, 0.f);
        window.draw(skybox.sprite);
    }
}

void RenderingEngine::renderWorld(dgm::Window& window)
{
    const auto W = float(scene.level.bottomMesh.getVoxelSize().x);
    const auto& player = scene.things[scene.camera.anchorIdx];
    const auto pos = player.hitbox.getPosition() / W;
    const auto plane = getPerpendicular(player.direction) * settings.fov;

#define fireRay(col, isLeftmost, isRightmost)                                  \
    {                                                                          \
        float cameraX = 2 * float(col) / settings.resolution.width - 1;        \
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
            return settings.resolution.height
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
        return int(((col + 1) / 2) * settings.resolution.width);
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
    fireRay(settings.resolution.width - 1, false, true);
    for (unsigned col = 1; col < settings.resolution.width - 1; col++)
    {
        fireRay(col, false, false);
    }
    caster.finalize();

    renderLevelMesh(window, getHeight, getColumn);
    renderSprites(window, pos, player.direction, getHeight, getColumn);
}

void RenderingEngine::renderFps(dgm::Window& window)
{
    if (!settings.showFps) return;
    text.setString(fpsCounter.getText());
    text.setPosition(
        { settings.resolution.width - text.getGlobalBounds().width - 10.f,
          10.f });
    window.draw(text);
}

void RenderingEngine::renderPlayerHud(
    dgm::Window& window, const Entity& player, const PlayerInventory& inventory)
{
    renderHudActiveWeapon(window, player, inventory);
    renderHudForHealth(window, player);
    renderHudForArmor(window, player);
    renderHudForAmmo(window, inventory);
    renderHudForWeaponSelection(window, inventory);
    renderHudForScore(window, inventory);
    renderHurtOverlay(window);
}

void RenderingEngine::renderHudActiveWeapon(
    dgm::Window& window, const Entity& player, const PlayerInventory& inventory)
{
    auto light = scene.drawableLevel.lightmap.at(sf::Vector2u(
        player.hitbox.getPosition()
        / static_cast<float>(scene.drawableLevel.lightmap.getVoxelSize().x)));

    const bool isLowering = [](AnimationStateId id)
    {
        using enum AnimationStateId;
        return id == Lower || id == FastLower;
    }(inventory.animationContext.animationStateId);

    const bool isRaising = [](AnimationStateId id)
    {
        using enum AnimationStateId;
        return id == Raise || id == FastRaise;
    }(inventory.animationContext.animationStateId);

    const float lowerRaiseProgress =
        (scene.tick - inventory.animationContext.lastAnimationUpdate)
        / static_cast<float>(
            ENTITY_PROPERTIES.at(inventory.activeWeaponType)
                .states.at(inventory.animationContext.animationStateId)
                .clip.front()
                .duration);

    const auto oldPos = weapon.sprite.getPosition();
    if (isLowering || isRaising)
    {
        const float slideFactor =
            isLowering ? lowerRaiseProgress : 1.f - lowerRaiseProgress;
        const float slideOffset = weapon.sprite.getSize().y * slideFactor;
        const float yPos = oldPos.y + slideOffset;
        weapon.sprite.setPosition(oldPos.x, yPos);
    }

    weapon.sprite.setTextureRect(weapon.clipping.getFrame(
        static_cast<std::size_t>(inventory.animationContext.spriteClipIndex)));
    weapon.sprite.setFillColor(sf::Color { light, light, light });
    window.draw(weapon.sprite);

    weapon.sprite.setPosition(oldPos);
}

void RenderingEngine::renderHudForHealth(
    dgm::Window& window, const Entity& player)
{
    hud.sprite.setPosition(
        10.f, settings.resolution.height - 10.f - hud.sprite.getSize().y);
    hud.sprite.setTextureRect(
        hud.clipping.getFrame(spriteIdToIndex(SpriteId::HUD_Health)));
    window.draw(hud.sprite);

    text.setString(std::to_string(player.health));
    const auto textBounds = text.getGlobalBounds();
    text.setPosition(
        hud.sprite.getPosition().x + hud.sprite.getSize().x + 10.f,
        hud.sprite.getPosition().y + hud.sprite.getSize().y / 2.f
            - textBounds.height / 2.f);
    window.draw(text);
}

void RenderingEngine::renderHudForArmor(
    dgm::Window& window, const Entity& player)
{
    hud.sprite.setPosition(
        10.f, settings.resolution.height - 10.f - hud.sprite.getSize().y * 2.f);
    hud.sprite.setTextureRect(
        hud.clipping.getFrame(spriteIdToIndex(SpriteId::HUD_Armor)));
    window.draw(hud.sprite);

    text.setString(std::to_string(player.armor));
    const auto textBounds = text.getGlobalBounds();
    text.setPosition(
        hud.sprite.getPosition().x + hud.sprite.getSize().x + 10.f,
        hud.sprite.getPosition().y + hud.sprite.getSize().y / 2.f
            - textBounds.height / 2.f);
    window.draw(text);
}

void RenderingEngine::renderHudForAmmo(
    dgm::Window& window, const PlayerInventory& inventory)
{
    hud.sprite.setPosition(
        settings.resolution.width - 10.f - hud.sprite.getSize().x,
        settings.resolution.height - 10.f - hud.sprite.getSize().y);

    auto ammoIndex = ammoTypeToAmmoIndex(
        ENTITY_PROPERTIES.at(inventory.activeWeaponType).ammoType);
    hud.sprite.setTextureRect(hud.clipping.getFrame(
        spriteIdToIndex(SpriteId::HUD_BulletAmmo) + ammoIndex));
    window.draw(hud.sprite);

    text.setString(std::to_string(inventory.ammo[ammoIndex]));
    const auto textBounds = text.getGlobalBounds();
    text.setPosition(
        hud.sprite.getPosition().x - 10.f - textBounds.width,
        hud.sprite.getPosition().y + hud.sprite.getSize().y / 2.f
            - textBounds.height / 2.f);
    window.draw(text);
}

void RenderingEngine::renderHudForWeaponSelection(
    dgm::Window& window, const PlayerInventory& inventory)
{
    if (inventory.selectionTimeout <= 0.f) return;

    const auto weaponCount = inventory.acquiredWeapons.size();
    const float baseX = settings.resolution.width / 2.f
                        - weaponCount * hud.sprite.getSize().x / 2.f;
    for (auto idx : std::views::iota(0u, weaponCount))
    {
        const bool isEquipped =
            weaponTypeToIndex(inventory.activeWeaponType) == idx;
        const bool isAcquired = inventory.acquiredWeapons[idx];
        const bool isSelected = inventory.selectionIdx == idx;

        hud.sprite.setPosition(
            baseX + idx * hud.sprite.getSize().x,
            settings.resolution.height / 2.f - hud.sprite.getSize().y / 2.f);
        hud.sprite.setFillColor(
            isSelected   ? COLOR_PICO8_MAGENTA
            : isAcquired ? COLOR_PICO8_GREEN
                         : COLOR_PICO8_RED);
        hud.sprite.setTextureRect(hud.clipping.getFrame(
            idx
            + spriteIdToIndex(
                isEquipped ? SpriteId::HUD_FlaregunFilled
                           : SpriteId::HUD_FlaregunOutline)));
        window.draw(hud.sprite);

        if (isSelected)
        {
            hud.sprite.setTextureRect(hud.clipping.getFrame(
                spriteIdToIndex(SpriteId::HUD_SelectFrame)));
            window.draw(hud.sprite);
        }
    }

    hud.sprite.setFillColor(sf::Color::White);
}

void RenderingEngine::renderHudForScore(
    dgm::Window& window, const PlayerInventory& inventory)
{
    text.setString(std::to_string(inventory.score));
    text.setPosition({ 10.f, 10.f });
    window.draw(text);
}

void RenderingEngine::renderHurtOverlay(dgm::Window& window)
{
    auto redOverlay = sf::RectangleShape(sf::Vector2f(window.getSize()));
    redOverlay.setFillColor(
        sf::Color(255, 0, 0, sf::Int8(scene.camera.redOverlayIntensity)));
    window.draw(redOverlay);
}

void RenderingEngine::renderRespawnPrompt(dgm::Window& window)
{
    text.setString(Strings::Game::RESPAWN_PROMPT);
    const auto bounds = text.getGlobalBounds();
    text.setPosition(
        (settings.resolution.width - bounds.width) / 2.f,
        (settings.resolution.height - bounds.height) / 2.f);
    window.draw(text);
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
            return scene.drawableLevel.bottomTextures[tileId];
        else if (heightHint > 1.f)
            return scene.drawableLevel.upperTextures[tileId];
        return static_cast<int>(LevelTileId::CeilLow);
    };

    auto&& quads = sf::VertexArray(sf::Triangles); // mesh to render
    const float midHeight = settings.resolution.height / 2.f;

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
                        ? scene.drawableLevel.upperTextures[face.tileId]
                        : scene.drawableLevel.bottomTextures[face.tileId]),
                static_cast<sf::Uint8>(
                    scene.drawableLevel.lightmap[face.neighboringTileId]) },
            tileset.clipping);
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
                    scene.drawableLevel.lightmap[flat.tileId]) },
            tileset.clipping);
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
    states.texture = &tileset.texture;
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
    const auto&& midHeight = settings.resolution.height / 2.f;
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
                .brightness =
                    static_cast<sf::Uint8>(scene.drawableLevel.lightmap.at(
                        sf::Vector2u(thing.center))),
                .flipTexture = thing.flipTexture },
            spritesheet.clipping);
    }

    sf::RenderStates states;
    states.texture = &spritesheet.texture;
    states.shader = &shader;
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
        if (id == scene.camera.anchorIdx) [[unlikely]]
            continue;

        auto&& thing = scene.things[id];

        const auto& thingPosition = thing.hitbox.getPosition() / W;
        const bool isDirectional =
            ENTITY_PROPERTIES.at(thing.typeId).traits & Trait::Directional;
        const auto [textureId, flipTexture] =
            isDirectional
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
    const int screenWidth = static_cast<int>(settings.resolution.width);
    const auto originalWidth = rightColumn - leftColumn + 1;
    int movedLeftColumnBy = 0;
    int movedRightColumnBy = 0;

    if (leftColumn >= screenWidth || rightColumn < 0) return std::nullopt;

    if (leftColumn < 0)
    {
        movedLeftColumnBy = -leftColumn;
        leftColumn = 0;
    }

    if (rightColumn >= screenWidth)
    {
        movedRightColumnBy = rightColumn - screenWidth;
        rightColumn = screenWidth - 1;
    }

    while (leftColumn < rightColumn && leftColumn < screenWidth
           && (leftColumn < 0 || depthBuffer[leftColumn] < thingDistance))
    {
        ++leftColumn;
        ++movedLeftColumnBy;
    }

    // Fully obscured or outside of view
    if (leftColumn >= screenWidth || leftColumn >= rightColumn)
        return std::nullopt;

    while (leftColumn < rightColumn && rightColumn >= 0
           && (rightColumn >= screenWidth
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
