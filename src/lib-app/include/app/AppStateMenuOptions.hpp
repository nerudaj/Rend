#pragma once

#include "GuiBuilder.hpp"
#include "utils/DependencyContainer.hpp"

class [[nodiscard]] AppStateMenuOptions final : public dgm::AppState
{
public:
    AppStateMenuOptions(
        dgm::App& app,
        mem::Rc<DependencyContainer> dic,
        bool enteredFromGame = false)
        : dgm::AppState(
            app,
            dgm::AppStateConfig {
                .clearColor = sf::Color::White,
                .shouldUpdateUnderlyingState = enteredFromGame,
            })
        , dic(dic)
        , enteredFromGame(enteredFromGame)
    {
        buildLayout();
    }

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

    static void buildPlayerOptionsLayout(
        FormBuilder& builder,
        mem::Rc<DependencyContainer>& dic,
        bool changesDisabled,
        std::function<void(void)> onChanged = [] {});

private:
    void buildLayout();
    void buildDisplayOptionsLayout(FormBuilder& builder);
    void buildAudioOptionsLayout(FormBuilder& builder);
    void buildInputOptionsLayout(FormBuilder& builder);

    void handleTabSelected(const tgui::String& selectedTabName);

private:
    mem::Rc<DependencyContainer> dic;
    bool enteredFromGame;
};