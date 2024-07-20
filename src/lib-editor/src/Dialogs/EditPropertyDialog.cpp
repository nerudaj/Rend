#include "Dialogs/EditPropertyDialog.hpp"
#include <Configs/Strings.hpp>

static inline constexpr const char* EDIT_DIALOG_ID = "EditPropertyDialog";

EditPropertyDialog::EditPropertyDialog(
    mem::Rc<Gui> gui, mem::Box<ToolPropertyInterface> property)
    : DialogInterface(
        gui, EDIT_DIALOG_ID, Strings::Editor::EDIT_PROPERTIES)
    , property(std::move(property))
{
}

void EditPropertyDialog::buildLayoutImpl(tgui::Panel::Ptr target)
{
    property->fillEditDialog(target);
}