#pragma once

#include "utils/DependencyContainer.hpp"
#include <Configs/Sizers.hpp>
#include <Configs/Strings.hpp>
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import Memory;
import AppMessage;
import CoreTypes;

class [[nodiscard]] GuiState
{
protected:
    [[nodiscard]] GuiState(mem::Rc<DependencyContainer> dic) noexcept : dic(dic)
    {
    }

    virtual ~GuiState() = default;

protected:
    virtual void buildLayoutImpl() = 0;

    void buildLayout()
    {
        dic->gui->gui.removeAllWidgets();
        buildLayoutImpl();
    }

    void restoreFocus(sf::RenderWindow& window)
    {
        dic->gui->gui.setWindow(window);
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
    mem::Rc<DependencyContainer> dic;
};
