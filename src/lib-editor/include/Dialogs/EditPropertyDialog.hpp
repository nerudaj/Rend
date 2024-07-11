#pragma once

#include "Dialogs/FormValidatorToken.hpp"
#include "Gui.hpp"
#include "Interfaces/DialogInterfaces.hpp"
#include "Interfaces/ToolPropertyInterface.hpp"
#include <Memory.hpp>

class [[nodiscard]] EditPropertyDialog final : public ModernDialogInterface
{
public:
    EditPropertyDialog(
        mem::Rc<Gui> gui, mem::Box<ToolPropertyInterface> property);

    const ToolPropertyInterface& getProperty() const noexcept
    {
        return *property;
    }

protected:
    void buildLayoutImpl(tgui::Panel::Ptr target) override;

    std::expected<ReturnFlag, ErrorMessage>
    validateBeforeConfirmation() const override;

protected:
    FormValidatorToken formValidatorToken;
    mem::Box<ToolPropertyInterface> property;
};