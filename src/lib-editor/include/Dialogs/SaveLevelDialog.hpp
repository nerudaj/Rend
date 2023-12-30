#pragma once

#include "Dialogs/DialogBase.hpp"
#include <filesystem>

class [[nodiscard]] SaveLevelDialog final : public DialogInterface
{
public:
    SaveLevelDialog(mem::Rc<Gui> gui);

public:
    std::string getLevelName() const;

protected:
    void customOpenCode() override {}
};

class [[nodiscard]] NewSaveLevelDialog final : public ModernDialogInterface
{
public:
    NewSaveLevelDialog(
        mem::Rc<Gui> gui, const std::filesystem::path& levelsDir);

public:
    const std::string& getLevelName() const
    {
        return levelName;
    }

private:
    void buildLayoutImpl(tgui::Panel::Ptr target) override;

    std::expected<ReturnFlag, ErrorMessage>
    validateBeforeConfirmation() const override;

private:
    const std::filesystem::path levelsDir;
    std::string levelName;
};