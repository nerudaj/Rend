#pragma once

#include "Client.hpp"
#include "app/AppStateLobbyBase.hpp"
#include "utils/DependencyContainer.hpp"
#include <Dialogs/MapPickerDialog.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <dgm/classes/AppState.hpp>
#include <dgm/classes/ResourceManager.hpp>

class [[nodiscard]] AppStateGameSetup final : public AppStateLobbyBase
{
public:
    AppStateGameSetup(dgm::App& app, mem::Rc<DependencyContainer> dic) noexcept;
    ~AppStateGameSetup();

public:
    void input() override;

    void update() override {}

    void draw() override
    {
        dic->gui->draw();
    }

private:
    void buildLayoutGameSetupImpl(tgui::Panel::Ptr target) override;

private:
    void restoreFocusImpl(const std::string& message) override;

    void selectMapPack(const std::string& packname);

    void selectMapPackAndSendUpdate(const std::string& packname);

    void adjustMapOrderAndSendUpdate();

    void shuffleMapOrder();

    void openMapPicker();

    void handleMapRotationUpdate();

    void sendLobbyUpdate();

private:
    std::vector<std::string> mapPackNames;
    mem::Box<MapPickerDialog> mapPickerDialog;
};
