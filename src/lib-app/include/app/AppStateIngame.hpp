#pragma once

#include "Settings.hpp"
#include "audio/AudioPlayer.hpp"
#include "engine/AudioEngine.hpp"
#include "engine/GameRulesEngine.hpp"
#include "engine/PhysicsEngine.hpp"
#include "engine/RenderingEngine.hpp"
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <core/Scene.hpp>

import Memory;

class AppStateIngame final : public dgm::AppState
{
public:
    [[nodiscard]] AppStateIngame(
        dgm::App& app,
        mem::Rc<const dgm::ResourceManager> resmgr,
        mem::Rc<tgui::Gui> gui,
        mem::Rc<Settings> settings,
        mem::Rc<AudioPlayer> audioPlayer);

public:
    virtual void input() override;
    virtual void update() override;
    virtual void draw() override;

    virtual [[nodiscard]] bool isTransparent() const noexcept override
    {
        return false;
    }

    virtual void restoreFocus() override {}

protected:
    mem::Rc<const dgm::ResourceManager> resmgr;
    mem::Rc<tgui::Gui> gui;
    mem::Rc<Settings> settings;
    mem::Rc<AudioPlayer> audioPlayer;

    const sf::Vector2f GAME_RESOLUTION;

    Scene scene;
    AudioEngine audioEngine;
    GameRulesEngine gameRulesEngine;
    PhysicsEngine physicsEngine;
    RenderingEngine renderingEngine;
};