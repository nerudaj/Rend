#pragma once

#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import Memory;
import Audio;
import AppMessage;
import CoreTypes;

class [[nodiscard]] GuiState
{
protected:
    [[nodiscard]] GuiState(
        mem::Rc<tgui::Gui> gui, mem::Rc<AudioPlayer> audioPlayer) noexcept
        : gui(gui), audioPlayer(audioPlayer)
    {
    }

    virtual ~GuiState() = default;

protected:
    virtual void buildLayoutImpl() = 0;

    void buildLayout()
    {
        gui->removeAllWidgets();
        buildLayoutImpl();
    }

    void restoreFocus(sf::RenderWindow& window)
    {
        gui->setWindow(window);
        buildLayout();
    }

    void handleRestoreMessage(dgm::App& app, const std::string& message)
    {
        auto&& msg = deserializeAppMessage(message);
        if (!msg) return;

        std::visit(
            overloaded { [&](const PopIfNotMainMenu&)
                         { app.popState(message); },
                         [&](const PopIfNotMapRotationWrapper&)
                         { app.popState(message); },
                         [](const PopIfPause&) {} },
            msg.value());
    }

protected:
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AudioPlayer> audioPlayer;
};
