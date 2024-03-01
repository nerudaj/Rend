#pragma once

#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import Memory;
import Audio;

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

protected:
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AudioPlayer> audioPlayer;
};
