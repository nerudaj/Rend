#pragma once

#include "Dialogs/DialogBase.hpp"
#include <filesystem>
#include <functional>
#include <string>

import Memory;

enum class UserChoice
{
    Confirmed = 0,
    Denied,
    Cancelled
};

class YesNoCancelDialogInterface
{
public:
    virtual void open(
        const std::string title,
        const std::string& text,
        std::function<void(UserChoice)> completedCallback) = 0;

    virtual ~YesNoCancelDialogInterface() = default;
};

class ErrorInfoDialogInterface
{
public:
    virtual ~ErrorInfoDialogInterface() = default;

public:
    virtual void open(const std::string& text) = 0;
};

class PlaytestSettingsDialogInterface : public DialogInterface
{
public:
    PlaytestSettingsDialogInterface(
        mem::Rc<Gui> gui,
        const std::string& id,
        const std::string& title,
        const std::vector<OptionLine>& options)
        : DialogInterface(gui, id, title, options)
    {
    }

    virtual ~PlaytestSettingsDialogInterface() = default;

public:
    [[nodiscard]] virtual std::filesystem::path getBinaryPath() const = 0;

    [[nodiscard]] virtual std::string getLaunchParameters() const = 0;

    [[nodiscard]] virtual std::filesystem::path getWorkingDirPath() const = 0;
};
