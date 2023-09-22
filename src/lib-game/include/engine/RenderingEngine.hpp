#pragma once

#include "core/Scene.hpp"
#include "events/RenderingEvents.hpp"
#include "render/FpsCounter.hpp"
#include "render/Raycaster.hpp"
#include "render/RenderContext.hpp"
#include <DGM/DGM.hpp>

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
        mem::Rc<const dgm::ResourceManager> resmgr, Scene& scene);

public: // Must visit on all related events
    constexpr inline void operator()(const EventRenderToggle& e) noexcept
    {
        if (e.fpsDisplay) showFps = !showFps;
        if (e.topDownRender) debugRender = !debugRender;
    }

public:
    void update(const dgm::Time& time);

    void renderWorldTo(dgm::Window& window);

    void renderHudTo(dgm::Window& window);

private:
    void render2d(dgm::Window& window);

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
    mem::Rc<const dgm::ResourceManager> resmgr;
    Scene& scene;
    const sf::Texture& tileset;
    bool showFps = true;
    bool debugRender = false;
    FpsCounter fpsCounter;
    const RenderSettings settings;
    RenderContext context;
    Raycaster caster;
};
