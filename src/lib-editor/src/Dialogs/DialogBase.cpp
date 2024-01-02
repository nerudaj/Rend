#include "Dialogs/DialogBase.hpp"
#include "Globals.hpp"

template<class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};

void DialogInterface::open(std::function<void()> confirmCallback)
{
    if (gui->isAnyModalOpened()) return;

    auto modal = gui->createNewChildWindow(DIALOG_TITLE);
    modal->setSize("30%", "50%");
    modal->setPosition("35%", "25%");
    modal->onEscapeKeyPress([this](auto) { close(); });
    modal->onClose([this] { close(); });
    gui->addModal(modal, DIALOG_ID);

    constexpr auto ROW_HEIGHT = 6_upercent;
    constexpr auto ROW_HEIGHT_STR = "6%";
    constexpr auto MARGIN = 2_upercent;
    constexpr auto ROW_SPACING = 1_upercent;
    constexpr auto LABEL_WIDTH = "48%";
    constexpr auto INPUT_POSITION_X = "52%";

    auto computeYposFromRow = [&](unsigned row) -> std::string
    { return std::to_string(row * (ROW_HEIGHT + ROW_SPACING) + MARGIN) + "%"; };

    auto computeMultiRowHeight = [&ROW_HEIGHT](unsigned rowCount) -> std::string
    {
        return std::to_string(
                   ROW_HEIGHT + (rowCount - 1) * (ROW_HEIGHT + ROW_SPACING))
               + "%";
    };

    auto addLabel = [&](const std::string& text, unsigned row)
    {
        auto label = tgui::Label::create(text);
        label->setSize(LABEL_WIDTH, ROW_HEIGHT_STR);
        label->setPosition("2%", computeYposFromRow(row).c_str());
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        modal->add(label);
    };

    auto addEditBox = [&](const std::string& value,
                          const std::string& id,
                          unsigned row,
                          bool narrow = false)
    {
        auto box = tgui::EditBox::create();
        box->setSize(narrow ? "38%" : "46%", ROW_HEIGHT_STR);
        box->setPosition(INPUT_POSITION_X, computeYposFromRow(row).c_str());
        box->setText(value);
        modal->add(box, id);
    };

    auto addCheckbox = [&](const bool, const std::string& id, unsigned row)
    {
        auto check = tgui::CheckBox::create();
        check->setPosition(INPUT_POSITION_X, computeYposFromRow(row).c_str());
        modal->add(check, id);
    };

    auto addHelperButton = [&](std::function<void(void)> callback, unsigned row)
    {
        auto btn = tgui::Button::create("...");
        btn->setSize("8%", "6%");
        btn->setPosition("90%", computeYposFromRow(row).c_str());
        btn->onClick(callback);
        modal->add(btn);
    };

    auto addText =
        [&](const std::string& text, unsigned occupiedRows, unsigned row)
    {
        auto label = tgui::Label::create(text);
        label->setSize("96%", computeMultiRowHeight(occupiedRows).c_str());
        label->setPosition("2%", computeYposFromRow(row).c_str());
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        modal->add(label);
    };

    auto addDropdown = [&](const std::string& id,
                           const std::vector<std::string>& values,
                           unsigned row)
    {
        auto combo = tgui::ComboBox::create();
        for (auto&& opt : values)
        {
            combo->addItem(opt);
        }
        combo->setPosition(INPUT_POSITION_X, computeYposFromRow(row).c_str());
        combo->setSelectedItem(values.front());
        modal->add(combo, id);
    };

    unsigned row = 0;
    for (auto&& option : OPTIONS)
    {
        std::visit(
            overloaded {
                [&](const OptionInput& input)
                {
                    addLabel(input.label, row);
                    addEditBox(input.value, input.id, row);
                },
                [&](const OptionDeferredInput& input)
                {
                    addLabel(input.label, row);
                    addEditBox(input.value(), input.id, row);
                },
                [&](const OptionInputWithButton& input)
                {
                    addLabel(input.base.label, row);
                    addEditBox(
                        input.base.value, input.base.id, row, "narrow"_true);
                    addHelperButton(input.buttonCallback, row);
                },
                [&](const OptionDeferredInputWithButton& input)
                {
                    addLabel(input.base.label, row);
                    addEditBox(
                        input.base.value(), input.base.id, row, "narrow"_true);
                    addHelperButton(input.buttonCallback, row);
                },
                [&](const OptionCheckbox& input)
                {
                    addLabel(input.label, row);
                    addCheckbox(input.defaultValue, input.id, row);
                },
                [&](const OptionText& input)
                {
                    addText(input.text, input.rowsToAllocate, row);
                    row += input.rowsToAllocate - 1;
                },
                [&](const OptionDropdown& input)
                {
                    addLabel(input.label, row);
                    addDropdown(input.id, input.values, row);
                } }

            ,
            std::forward<const OptionLine>(option));
        row++;
    }

    auto btn = tgui::Button::create("Ok");
    btn->setSize("20%", "8%");
    btn->setPosition("56%", "90%");
    btn->onClick(
        [this, confirmCallback]
        {
            confirmCallback();
            close();
        });
    modal->add(btn);

    btn = tgui::Button::create("Cancel");
    btn->setSize("20%", "8%");
    btn->setPosition("78%", "90%");
    btn->onClick([this] { close(); });
    modal->add(btn);

    customOpenCode();
}

DialogInterface::DialogInterface(
    mem::Rc<Gui> gui,
    const std::string& dialogId,
    const std::string& dialogTitle,
    const std::vector<OptionLine>& options)
    : gui(gui), DIALOG_ID(dialogId), DIALOG_TITLE(dialogTitle), OPTIONS(options)
{
}

ModernDialogInterface::ModernDialogInterface(
    mem::Rc<Gui> gui,
    const std::string& dialogId,
    const std::string& dialogTitle)
    : gui(gui), DIALOG_ID(dialogId), DIALOG_TITLE(dialogTitle)
{
}

void ModernDialogInterface::open(std::function<void()> confirmCallback)
{
    if (isOpen()) return;
    buildLayout(confirmCallback);
}

void ModernDialogInterface::buildLayout(std::function<void()> confirmCallback)
{
    auto modal = gui->createNewChildWindow(DIALOG_TITLE);
    modal->setSize("30%", "50%");
    modal->setPosition("35%", "25%");
    modal->onEscapeKeyPress([this](auto) { close(); });
    modal->onClose([this] { close(); });
    gui->addModal(modal, DIALOG_ID);

    auto panel = tgui::Panel::create({ "90%", "85%" });
    panel->setPosition({ "5%", "5%" });
    panel->setRenderer(gui->theme.getRenderer("Panel"));
    modal->add(panel);

    buildBottomButtons(modal, confirmCallback);
    buildLayoutImpl(panel);
}

void ModernDialogInterface::buildBottomButtons(
    tgui::ChildWindow::Ptr modal, std::function<void()> confirmCallback)
{
    auto btn = tgui::Button::create("Ok");
    btn->setSize("20%", "8%");
    btn->setPosition("56%", "90%");
    btn->onClick(
        [this, confirmCallback]
        {
            auto validationResult = validateBeforeConfirmation();
            if (!validationResult)
            {
                throw std::runtime_error(validationResult.error());
            }
            else
            {
                confirmCallback();
                close();
            }
        });
    modal->add(btn);

    btn = tgui::Button::create("Cancel");
    btn->setSize("20%", "8%");
    btn->setPosition("78%", "90%");
    btn->onClick([this] { close(); });
    modal->add(btn);
}
