#pragma once

#include "Gui.hpp"
#include <expected>
#include <functional>
#include <variant>
#include <vector>

import Memory;
import Literals;
import Error;

struct OptionInput
{
    std::string label;
    std::string id;
    std::string value;
};

struct OptionDeferredInput
{
    std::string label;
    std::string id;
    std::function<std::string()> value;
};

struct OptionInputWithButton
{
    OptionInput base;
    std::function<void(void)> buttonCallback = [] {};
};

struct OptionDeferredInputWithButton
{
    OptionDeferredInput base;
    std::function<void(void)> buttonCallback = [] {};
};

struct OptionCheckbox
{
    std::string label;
    std::string id;
    bool defaultValue = false;
};

struct OptionText
{
    std::string text;
    unsigned rowsToAllocate;
};

struct OptionDropdown
{
    std::string label;
    std::string id;
    std::vector<std::string> values;
};

using OptionLine = std::variant<
    OptionInput,
    OptionDeferredInput,
    OptionInputWithButton,
    OptionDeferredInputWithButton,
    OptionCheckbox,
    OptionText,
    OptionDropdown>;

class DialogInterface
{
protected:
    mem::Rc<Gui> gui;
    const std::string DIALOG_ID;
    const std::string DIALOG_TITLE;
    const std::vector<OptionLine> OPTIONS;

protected:
    virtual void customOpenCode() = 0;

    [[nodiscard]] std::string getEditboxValue(const std::string& boxId) const
    {
        return gui->get<tgui::EditBox>(boxId)->getText().toStdString();
    }

public:
    void open(std::function<void()> confirmCallback);

    void close()
    {
        gui->closeModal(DIALOG_ID);
    }

    [[nodiscard]] bool isOpen() const
    {
        return gui->get<tgui::ChildWindow>(DIALOG_ID) != nullptr;
    }

    DialogInterface(
        mem::Rc<Gui> gui,
        const std::string& dialogId,
        const std::string& dialogTitle,
        const std::vector<OptionLine>& options);
    DialogInterface(const DialogInterface&) = delete;
    DialogInterface(DialogInterface&&) = delete;
    virtual ~DialogInterface() = default;
};

class ModernDialogInterface
{
public:
    ModernDialogInterface(
        mem::Rc<Gui> gui,
        const std::string& dialogId,
        const std::string& dialogTitle);
    ~ModernDialogInterface() = default;

public:
    void open(std::function<void()> confirmCallback);

    void close()
    {
        gui->closeModal(DIALOG_ID);
    }

    [[nodiscard]] bool isOpen() const
    {
        return gui->get<tgui::ChildWindow>(DIALOG_ID) != nullptr;
    }

protected:
    void buildLayout(std::function<void()> confirmCallback);
    void buildBottomButtons(
        tgui::ChildWindow::Ptr modal, std::function<void()> confirmCallback);

    virtual std::expected<ReturnFlag, ErrorMessage>
    validateBeforeConfirmation() const
    {
        return ReturnFlag::Success;
    };

    virtual void buildLayoutImpl(tgui::Panel::Ptr target) = 0;

protected:
    mem::Rc<Gui> gui;
    const std::string DIALOG_ID;
    const std::string DIALOG_TITLE;
};
