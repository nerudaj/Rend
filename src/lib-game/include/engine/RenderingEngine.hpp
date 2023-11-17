#pragma once

#include "core/Scene.hpp"
#include "events/RenderingEvents.hpp"
#include "render/FpsCounter.hpp"
#include "render/Raycaster.hpp"
#include "render/RenderContext.hpp"
#include <DGM/DGM.hpp>
#include <LevelD.hpp>

import Memory;

struct RenderSettings
{
    const float FOV = 0.66f;
    const unsigned WIDTH = 1280;
    const unsigned HEIGHT = 720;
};

class RenderingEngine final
{
public:
    [[nodiscard]] RenderingEngine(
        const dgm::ResourceManager& resmgr, const LevelD& level, Scene& scene);

public: // Must visit on all related events
    constexpr inline void operator()(const EventRenderToggle& e) noexcept
    {
        if (e.fpsDisplay) showFps = !showFps;
        // if (e.topDownRender) debugRender = !debugRender;
    }

public:
    void update(const float deltaTime);

    void renderWorldTo(dgm::Window& window);

    void renderHudTo(dgm::Window& window);

private:
    void render3d(dgm::Window& window);

    void renderLevelMesh(
        dgm::Window& window,
        std::function<float(const sf::Vector2f&)> getHeight,
        std::function<int(const sf::Vector2f&)> getColumn);

    void renderSprites(
        dgm::Window& window,
        const sf::Vector2f& cameraPosition,
        const sf::Vector2f& cameraDirection,
        std::function<float(const sf::Vector2f&)> getHeight,
        std::function<int(const sf::Vector2f&)> getColumn);

    void renderAlivePlayerHud(
        dgm::Window& window,
        const Entity& player,
        const PlayerInventory& inventory);

    struct ThingToRender
    {
        float distance;
        sf::Vector2f center;
        std::uint8_t textureId;
        bool flipTexture;
    };

    std::vector<ThingToRender> getFilteredAndOrderedThingsToRender(
        const std::vector<std::size_t>& candidateIds,
        const sf::Vector2f& cameraPosition,
        const sf::Vector2f& cameraDirection);

    /// <summary>
    /// Manipulates leftColumn and rightColumn references
    /// if the function returns nothing then the sprite is
    /// fully out-of-view and should be skipped
    /// </summary>
    /// <param name="leftColumn"></param>
    /// <param name="rightColumn"></param>
    /// <returns>left/right texture hints or nothing</returns>
    std::optional<std::pair<float, float>> cropSpriteIfObscured(
        int& leftColumn, int& rightColumn, float thingDistance);

private:
    const RenderSettings settings;
    Scene& scene;
    const sf::Texture& tilesetTexture;
    dgm::Clip tilesetClipping;
    const sf::Texture& spritesheetTexture;
    dgm::Clip spritesheetClipping;
    sf::RectangleShape weaponSprite;
    sf::RectangleShape skyboxSprite;
    dgm::Clip weaponClipping;
    const sf::Shader& shader;
    sf::Text text;
    DrawableLevel drawableLevel;
    Raycaster caster;
    FpsCounter fpsCounter;
    bool showFps = true;
    std::vector<float> depthBuffer;
};
