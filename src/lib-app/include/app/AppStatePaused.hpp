#pragma once

#include "utils/DependencyContainer.hpp"
#include <DGM/dgm.hpp>

class [[nodiscard]] AppStatePaused final : public dgm::AppState
{
public:
    AppStatePaused(dgm::App& app, mem::Rc<DependencyContainer> dic)
        : dgm::AppState(
            app,
            dgm::AppStateConfig { .shouldUpdateUnderlyingState = true,
                                  .shouldDrawUnderlyingState = true })
        , dic(dic)
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
    void buildLayout();

    void restoreFocusImpl(const std::string& message) override;

private:
    mem::Rc<DependencyContainer> dic;
};