#pragma once

#include "GuiState.hpp"
#include "utils/DependencyContainer.hpp"

class [[nodiscard]] AppStatePaused final
    : public dgm::AppState
    , public GuiState
{
public:
    AppStatePaused(dgm::App& app, mem::Rc<DependencyContainer> dic)
        : dgm::AppState(
            app,
            dgm::AppStateConfig { .shouldUpdateUnderlyingState = true,
                                  .shouldDrawUnderlyingState = true })
        , GuiState(dic)
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

private:
    void buildLayoutImpl() override;

    void restoreFocusImpl(const std::string& message) override
    {
        if (PopIfPause::canDeserializeFrom(message)) app.popState();
        GuiState::restoreFocus(app.window.getWindowContext());
    }
};