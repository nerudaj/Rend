#include "Dialogs/SaveLevelDialog.hpp"
#include "Configs/Strings.hpp"
#include "GuiBuilder.hpp"
#include <Filesystem.hpp>

constexpr const char* PACK_NAME_ID = "PackNameId";
constexpr const char* LEVEL_NAME_ID = "LevelNameId";

NewSaveLevelDialog::NewSaveLevelDialog(
    mem::Rc<Gui> gui, const std::filesystem::path& levelsDir)
    : ModernDialogInterface(
        gui, "SaveLevelDialogId", Strings::Dialog::Title::SAVE_LEVEL)
    , levelsDir(levelsDir)
    , mapPackNames(Filesystem::getLevelPackNames(levelsDir))
    , mapPackName(mapPackNames.front())
{
}

void NewSaveLevelDialog::buildLayoutImpl(tgui::Panel::Ptr target)
{
    target->add(
        FormBuilder()
            .addOptionWithWidgetId(
                Strings::Dialog::Body::SELECT_PACK,
                WidgetBuilder::createDropdown(
                    mapPackNames,
                    mapPackName,
                    [&](const size_t idx)
                    { mapPackName = mapPackNames.at(idx); }),
                PACK_NAME_ID)
            .addOptionWithSubmit(
                Strings::Dialog::Body::CREATE_PACK,
                WidgetBuilder::createTextInput(
                    "",
                    [&](tgui::String str)
                    { mapPackNameInput = str.toStdString(); },
                    "[a-zA-Z0-9_]+"),
                WidgetBuilder::createButton(
                    Strings::Dialog::Body::ADD,
                    std::bind(&NewSaveLevelDialog::handleNewMapPack, this)))
            .addOption(
                Strings::Dialog::Body::LEVEL_NAME,
                WidgetBuilder::createTextInput(
                    levelName,
                    [&](tgui::String str) { levelName = str.toStdString(); },
                    "[a-zA-Z0-9_]+"))
            .build());
}

std::expected<ReturnFlag, ErrorMessage>
NewSaveLevelDialog::validateBeforeConfirmation() const
{
    if (levelName.empty())
        return std::unexpected(
            std::string(Strings::Dialog::Message::LEVEL_NAME_EMPTY));

    if (!std::filesystem::exists(levelsDir / mapPackName))
        return ReturnFlag::Success;

    for (auto&& path :
         std::filesystem::directory_iterator(levelsDir / mapPackName))
    {
        auto filename = path.path().stem();
        if (filename == levelName)
            return std::unexpected(
                std::string(Strings::Dialog::Message::LEVEL_NAME_TAKEN));
    }

    return ReturnFlag::Success;
}

void NewSaveLevelDialog::handleNewMapPack()
{
    if (mapPackNameInput.empty())
        throw std::runtime_error(Strings::Dialog::Message::PACK_NAME_EMPTY);

    for (auto&& path : std::filesystem::directory_iterator(levelsDir))
    {
        if (mapPackNameInput == path.path().stem())
            throw std::runtime_error(Strings::Dialog::Message::PACK_NAME_TAKEN);
    }

    mapPackNames.insert(mapPackNames.begin(), mapPackNameInput);
    auto&& dropdown = gui->get<tgui::ComboBox>(PACK_NAME_ID);
    WidgetBuilder::updateDropdownItems(dropdown, mapPackNames);
}