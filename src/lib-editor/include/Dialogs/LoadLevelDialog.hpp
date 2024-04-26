#pragma once

#include <Dialogs/DialogBase.hpp>
#include <filesystem>

class [[nodiscard]] LoadLevelDialog final : public ModernDialogInterface
{
public:
    LoadLevelDialog(mem::Rc<Gui> gui, const std::filesystem::path& rootDir);

public:
    [[nodiscard]] constexpr const std::string& getMapPackName() const noexcept
    {
        return mapPackName;
    }

    [[nodiscard]] constexpr const std::string& getLevelName() const noexcept
    {
        return mapName;
    }

protected:
    void buildLayoutImpl(tgui::Panel::Ptr panel) override;

    void handleSelectedMapPack(const size_t idx);

private:
    const std::filesystem::path levelsDir;
    std::vector<std::string> mapPackNames;
    std::string mapPackName;
    std::vector<std::string> mapNames;
    std::string mapName;
    mem::Rc<Gui> gui;
};
