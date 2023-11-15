#include "include/Dialogs/DialogBase.hpp"
#include "include/Globals.hpp"
#include "include/Utilities/FileApi.hpp"
#include "include/Utilities/Literals.hpp"

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
    modal->connect("EscapeKeyPressed", [this] { close(); });
    modal->connect("Closed", [this] { close(); });
    gui->addModal(modal, DIALOG_ID);

    constexpr auto ROW_HEIGHT = 6_upercent;
    constexpr auto ROW_HEIGHT_STR = "6%";
    constexpr auto MARGIN = 2_upercent;
    constexpr auto ROW_SPACING = 1_upercent;

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
        label->setSize("26%", ROW_HEIGHT_STR);
        label->setPosition("2%", computeYposFromRow(row));
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        modal->add(label);
    };

    auto addEditBox = [&](const std::string& value,
                          const std::string& id,
                          unsigned row,
                          bool narrow = false)
    {
        auto box = tgui::EditBox::create();
        box->setSize(narrow ? "58%" : "66%", ROW_HEIGHT_STR);
        box->setPosition("32%", computeYposFromRow(row));
        box->setText(value);
        modal->add(box, id);
    };

    auto addCheckbox =
        [&](const bool value, const std::string& id, unsigned row)
    {
        auto check = tgui::CheckBox::create();
        check->setPosition("32%", computeYposFromRow(row));
        modal->add(check, id);
    };

    auto addHelperButton = [&](std::function<void(void)> callback, unsigned row)
    {
        auto btn = tgui::Button::create("...");
        btn->setSize("8%", "6%");
        btn->setPosition("90%", computeYposFromRow(row));
        btn->connect("clicked", callback);
        modal->add(btn);
    };

    auto addText =
        [&](const std::string& text, unsigned occupiedRows, unsigned row)
    {
        auto label = tgui::Label::create(text);
        label->setSize("96%", computeMultiRowHeight(occupiedRows));
        label->setPosition("2%", computeYposFromRow(row));
        label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
        modal->add(label);
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
                } }

            ,
            std::forward<const OptionLine>(option));
        row++;
    }

    auto btn = tgui::Button::create("Ok");
    btn->setSize("20%", "8%");
    btn->setPosition("56%", "90%");
    btn->connect(
        "clicked",
        [this, confirmCallback]
        {
            confirmCallback();
            close();
        });
    modal->add(btn);

    btn = tgui::Button::create("Cancel");
    btn->setSize("20%", "8%");
    btn->setPosition("78%", "90%");
    btn->connect("clicked", [this] { close(); });
    modal->add(btn);

    customOpenCode();
}

DialogInterface::DialogInterface(
    GC<Gui> gui,
    const std::string& dialogId,
    const std::string& dialogTitle,
    const std::vector<OptionLine>& options)
    : gui(gui), DIALOG_ID(dialogId), DIALOG_TITLE(dialogTitle), OPTIONS(options)
{
}
