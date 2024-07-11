#include "Dialogs/EditPropertyDialog.hpp"
#include <Configs/Strings.hpp>

static inline constexpr const char* EDIT_DIALOG_ID = "EditPropertyDialog";

EditPropertyDialog::EditPropertyDialog(
    mem::Rc<Gui> gui, mem::Box<ToolPropertyInterface> property)
    : ModernDialogInterface(
        gui, EDIT_DIALOG_ID, Strings::Editor::EDIT_PROPERTIES)
    , property(std::move(property))
{
}

void EditPropertyDialog::buildLayoutImpl(tgui::Panel::Ptr target)
{
    property->fillEditDialog(target, formValidatorToken);
}

std::expected<ReturnFlag, ErrorMessage>
EditPropertyDialog::validateBeforeConfirmation() const
{
    if (formValidatorToken.isValid()) return ReturnFlag::Success;
    return std::unexpected("Some of the fields contain invalid values");
}
