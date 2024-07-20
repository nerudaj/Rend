#include "Dialogs/NewLevelDialog.hpp"
#include "GuiBuilder.hpp"
#include <Configs/Strings.hpp>

ModernNewLevelDialog::ModernNewLevelDialog(mem::Rc<Gui> gui)
    : DialogInterface(
        gui, "ModernNewLevelDialog", Strings::Dialog::Title::NEW_LEVEL)
{
}

void ModernNewLevelDialog::buildLayoutImpl(tgui::Panel::Ptr panel)
{
    auto&& builder = FormBuilder();
    std::ignore = builder
                      .addOption(
                          Strings::Editor::NewLevel::WIDTH,
                          WidgetBuilder::createDropdown(
                              ALLOWED_LEVEL_SIZES,
                              std::to_string(width),
                              [this](std::size_t idx)
                              { width = std::stol(ALLOWED_LEVEL_SIZES[idx]); }))
                      .addOption(
                          Strings::Editor::NewLevel::HEIGHT,
                          WidgetBuilder::createDropdown(
                              ALLOWED_LEVEL_SIZES,
                              std::to_string(height),
                              [this](std::size_t idx) {
                                  height = std::stol(ALLOWED_LEVEL_SIZES[idx]);
                              }));
    addAttributesToFormular(builder);
    panel->add(builder.build());
}
