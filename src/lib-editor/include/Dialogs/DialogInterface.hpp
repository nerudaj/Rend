#pragma once

#include "Gui.hpp"
#include <Error.hpp>
#include <Memory.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Tgui.hpp>
#include <expected>
#include <functional>
#include <string>

class DialogInterface
{
public:
    DialogInterface(
        mem::Rc<Gui> gui,
        const std::string& dialogId,
        const std::string& dialogTitle);
    ~DialogInterface() = default;

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
