#pragma once

#include "core/Scene.hpp"
#include "render/FpsCounter.hpp"
#include "render/Raycaster.hpp"
#include <DGM/DGM.hpp>
#include <LevelD.hpp>

import Memory;
import Options;

struct ClippableTexture
{
    const sf::Texture& texture;
    dgm::Clip clipping;
};

struct ClippableRect
{
    sf::RectangleShape sprite;
    dgm::Clip clipping;
};

class RenderingEngine final
{
public:
    [[nodiscard]] RenderingEngine(
        const DisplayOptions& settings,
        const dgm::ResourceManager& resmgr,
        Scene& scene);

public:
    void update(const float deltaTime);

    void renderTo(dgm::Window& window);

private:
    void renderSkybox(dgm::Window& window, const float angle);
    void renderWorld(dgm::Window& window);
    void renderFps(dgm::Window& window);
    void renderPlayerHud(
        dgm::Window& window,
        const Entity& player,
        const PlayerInventory& inventory);
    void renderHudActiveWeapon(
        dgm::Window& window,
        const Entity& player,
        const PlayerInventory& inventory);
    void renderHudForHealth(dgm::Window& window, const Entity& player);
    void renderHudForArmor(dgm::Window& window, const Entity& player);
    void
    renderHudForAmmo(dgm::Window& window, const PlayerInventory& inventory);
    void renderHudForWeaponSelection(
        dgm::Window& window, const PlayerInventory& inventory);
    void
    renderHudForScore(dgm::Window& window, const PlayerInventory& inventory);
    void
    renderHurtOverlay(dgm::Window& window, const RenderContext& renderContext);
    void
    renderHudMessage(dgm::Window& window, const RenderContext& renderContext);
    void renderRespawnPrompt(dgm::Window& window);

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
    const DisplayOptions settings;
    Scene& scene;
    ClippableTexture tileset;
    ClippableTexture spritesheet;
    ClippableRect weapon;
    ClippableRect skybox;
    ClippableRect hud;
    sf::Shader& shader;
    const sf::Texture& noiseTexture;
    sf::Text text;
    Raycaster caster;
    FpsCounter fpsCounter;
    std::vector<float> depthBuffer;
};
