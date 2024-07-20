#include "Dialogs/EditMetadataDialog.hpp"
#include "WidgetBuilder.hpp"
#include <Configs/Strings.hpp>

EditMetadataDialog::EditMetadataDialog(mem::Rc<Gui> gui)
    : DialogInterface(
        gui, "EditMetadataDialog", Strings::Dialog::Title::EDIT_METADATA)
{
}

void EditMetadataDialog::open(
    const LevelMetadata& metadata, std::function<void()> confirmCallback)
{
    skyboxTheme = metadata.skyboxTheme;
    texturePack = metadata.texturePack;
    author = metadata.author;

    DialogInterface::open(confirmCallback);
}

void EditMetadataDialog::buildLayoutImpl(tgui::Panel::Ptr panel)
{
    auto&& builder = FormBuilder();
    addAttributesToFormular(builder);
    panel->add(builder.build());
}
