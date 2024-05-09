#pragma once

#include "Dialogs/DialogBuilderHelper.hpp"
#include "Dialogs/FormValidatorToken.hpp"
#include "Gui.hpp"
#include "Interfaces/ToolInterface.hpp"
#include "Interfaces/ToolPropertyInterface.hpp"
#include <Memory.hpp>
#include <functional>

class [[nodiscard]] EditPropertyDialog final
{
public:
    [[nodiscard]] EditPropertyDialog(mem::Rc<Gui> gui) noexcept : gui(gui) {}

public:
    void
    open(mem::Box<ToolPropertyInterface> property, ToolInterface& targetTool);

    void close();

public:
    static inline constexpr const char* DIALOG_ID = "EditPropertyDialog";

private:
    void buildBottomButtonBar(
        tgui::ChildWindow::Ptr& modal, std::function<void(void)> submit);

    tgui::Button::Ptr createButton(
        const std::string& label,
        const tgui::Layout2d& size,
        const tgui::Layout2d& position,
        std::function<void(void)> callback);

private:
    mem::Rc<Gui> gui;
    FormValidatorToken formValidatorToken;
    mem::Box<ToolPropertyInterface> currentProperty =
        mem::Box<NullToolProperty2>();
};
