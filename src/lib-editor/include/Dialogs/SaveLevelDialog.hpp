#pragma once

#include "Dialogs/DialogBase.hpp"
#include <filesystem>

class [[nodiscard]] NewSaveLevelDialog final : public ModernDialogInterface
{
public:
    NewSaveLevelDialog(
        mem::Rc<Gui> gui, const std::filesystem::path& levelsDir);

public:
    [[nodiscard]] constexpr const std::string& getLevelName() const noexcept
    {
        return levelName;
    }

    [[nodiscard]] constexpr const std::string& getMapPackName() const noexcept
    {
        return mapPackName;
    }

private:
    void buildLayoutImpl(tgui::Panel::Ptr target) override;

    std::expected<ReturnFlag, ErrorMessage>
    validateBeforeConfirmation() const override;

    void handleNewMapPack();

private:
    const std::filesystem::path levelsDir;
    std::vector<std::string> mapPackNames;
    std::string mapPackName;
    std::string levelName;
    std::string mapPackNameInput;
};