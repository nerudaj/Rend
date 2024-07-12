#include "WidgetBuilder.hpp"
#include "TguiHelper.hpp"
#include <Configs/Sizers.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

[[nodiscard]] std::string randomString(std::size_t len)
{
    constexpr auto CHARS = std::string_view(
        "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    return std::views::iota(0u, len)
           | std::views::transform([CHARS](std::size_t)
                                   { return CHARS[rand() % std::size(CHARS)]; })
           | std::ranges::to<std::string>();
}

tgui::Label::Ptr WidgetBuilder::createLabelInternal(
    const std::string& text, const float sizeMultiplier, const bool justify)
{
    auto&& label = tgui::Label::create(text);
    label->getRenderer()->setTextColor(sf::Color::White);
    label->getRenderer()->setTextOutlineColor(tgui::Color::Black);
    label->getRenderer()->setTextOutlineThickness(1.f);
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    label->setHorizontalAlignment(
        justify ? tgui::Label::HorizontalAlignment::Center
                : tgui::Label::HorizontalAlignment::Left);
    label->setTextSize(Sizers::getBaseTextSize() * sizeMultiplier);
    label->setSize({ "100%", "100%" });
    return label;
}

tgui::Panel::Ptr
WidgetBuilder::createPanel(const tgui::Layout2d& size, const tgui::Color color)
{
    auto&& panel = tgui::Panel::create(size);
    panel->setPosition({ "0%", "0%" });
    panel->getRenderer()->setBackgroundColor(color);
    return panel;
}

static tgui::Label::Ptr createRowLabel(const std::string& text)
{
    auto&& label = tgui::Label::create(text);
    label->getRenderer()->setTextColor(sf::Color::Black);
    label->setSize("60%", "100%");
    label->setPosition("0%", "0%");
    label->setTextSize(Sizers::GetMenuBarTextHeight());
    label->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    return label;
}

tgui::Panel::Ptr WidgetBuilder::createOptionRow(
    const std::string& labelText,
    tgui::Widget::Ptr widgetPtr,
    std::optional<std::string> widgetId)
{
    auto&& row = getStandardizedRow();
    row->add(createRowLabel(labelText));

    auto&& widgetPanel = tgui::Panel::create({ "40%", "100%" });
    widgetPanel->setPosition("60%", "0%");
    widgetPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    row->add(widgetPanel);

    widgetId
        .or_else(
            [&]() -> std::optional<std::string>
            {
                widgetPanel->add(widgetPtr);
                return std::nullopt;
            })
        .and_then(
            [&](auto id) -> std::optional<std::string>
            {
                widgetPanel->add(widgetPtr, id);
                return id;
            });

    return row;
}

tgui::Panel::Ptr WidgetBuilder::createOptionRowWithSubmitButton(
    const std::string& labelText,
    tgui::Widget::Ptr widgetPtr,
    tgui::Button::Ptr buttonPtr)
{
    auto&& row = getStandardizedRow();
    row->add(createRowLabel(labelText));

    auto&& widgetPanel = tgui::Panel::create({ "25%", "100%" });
    widgetPanel->setPosition("60%", "0%");
    widgetPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    widgetPanel->add(widgetPtr);
    row->add(widgetPanel);

    auto&& btnPanel = tgui::Panel::create({ "15%", "100%" });
    btnPanel->setPosition("85%", "0%");
    btnPanel->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    btnPanel->add(buttonPtr);
    row->add(btnPanel);

    return row;
}

tgui::Panel::Ptr WidgetBuilder::getStandardizedRow(tgui::Color bgcolor)
{
    auto&& row = tgui::Panel::create();
    row->setSize("100%", std::to_string(Sizers::GetMenuBarHeight()).c_str());
    row->getRenderer()->setBackgroundColor(bgcolor);
    return row;
}

tgui::Button::Ptr WidgetBuilder::createButton(
    const std::string& label, std::function<void(void)> onClick)
{
    auto&& button = tgui::Button::create(label);
    button->onClick(onClick);
    button->setTextSize(Sizers::getBaseTextSize());
    button->setSize({ "100%", "100%" });
    return button;
}

tgui::CheckBox::Ptr
WidgetBuilder::createCheckbox(bool checked, std::function<void(bool)> onChange)
{
    auto&& checkbox = tgui::CheckBox::create();
    auto&& size = checkbox->getSizeLayout();
    checkbox->setPosition(("100%" - size.x) / 2, ("100%" - size.y) / 2);
    checkbox->setChecked(checked);
    checkbox->onChange(onChange);
    return checkbox;
}

tgui::Panel::Ptr WidgetBuilder::createSlider(
    float value,
    std::function<void(float)> onChange,
    tgui::Gui& gui,
    std::function<std::string(float)> valueFormatter,
    float lo,
    float hi,
    float step)
{
    auto&& result = tgui::Panel::create();
    result->getRenderer()->setBackgroundColor(tgui::Color::Transparent);
    const auto&& ID = randomString(16);

    auto&& dummyLabel = tgui::Label::create(valueFormatter(hi + step));
    dummyLabel->setTextSize(Sizers::GetMenuBarTextHeight());
    dummyLabel->setAutoSize(true);
    gui.add(dummyLabel, "DummyLabel");
    auto size = dummyLabel->getSizeLayout();

    auto&& valueLabel = tgui::Label::create(valueFormatter(value));
    valueLabel->setTextSize(Sizers::GetMenuBarTextHeight());
    valueLabel->getRenderer()->setTextColor(sf::Color::Black);
    valueLabel->setSize(size.x, "100%");
    valueLabel->setPosition("parent.width" - size.x, "0%");
    valueLabel->setVerticalAlignment(tgui::Label::VerticalAlignment::Center);
    valueLabel->setHorizontalAlignment(
        tgui::Label::HorizontalAlignment::Center);
    result->add(valueLabel, ID);

    auto&& slider = tgui::Slider::create(lo, hi);
    slider->setPosition("2%", "25%");
    slider->setSize("parent.width - 4% - " - size.x, "50%");
    slider->setStep(step);
    slider->setValue(value);
    slider->onValueChange(
        [&gui, ID, onChange, valueFormatter](float value)
        {
            gui.get<tgui::Label>(ID)->setText(valueFormatter(value));
            onChange(value);
        });
    result->add(slider);

    gui.remove(gui.get<tgui::Label>("DummyLabel"));

    return result;
}

tgui::ComboBox::Ptr WidgetBuilder::createDropdown(
    const std::vector<std::string>& items,
    const std::string& selected,
    std::function<void(std::size_t)> onSelect)
{
    auto&& dropdown = tgui::ComboBox::create();
    dropdown->setSize("100%", "80%");
    dropdown->setPosition("0%", "10%");
    updateDropdownItems(dropdown, items);
    dropdown->setSelectedItem(selected);
    dropdown->onItemSelect(onSelect);

    return dropdown;
}

void WidgetBuilder::updateDropdownItems(
    tgui::ComboBox::Ptr dropdown, const std::vector<std::string>& items)
{
    dropdown->removeAllItems();
    for (auto&& item : items)
    {
        dropdown->addItem(item, item);
    }
    dropdown->setSelectedItem(items.front());
}

tgui::EditBox::Ptr WidgetBuilder::createTextInput(
    const std::string& initialValue,
    std::function<void(tgui::String)> onChange,
    const std::string& regexValidator)
{
    auto&& box = tgui::EditBox::create();
    box->setSize("100%", "80%");
    box->setPosition("0%", "10%");
    if (!regexValidator.empty()) box->setInputValidator(regexValidator);
    box->setText(initialValue);
    box->onTextChange(onChange);
    return box;
}

tgui::Panel::Ptr WidgetBuilder::createTabbedContent(
    const std::vector<std::string>& tabLabels,
    const std::string& contentPanelId,
    std::function<void(const tgui::String&)> onTabChange,
    std::optional<std::string> tabsId)
{
    auto&& finalPanel = WidgetBuilder::createPanel();
    auto&& tabs = tgui::Tabs::create();
    tabs->setSize({ "100%", Sizers::getBaseContainerHeight() });
    tabs->setTextSize(Sizers::getBaseTextSize());

    for (auto&& label : tabLabels)
    {
        tabs->add(label);
    }

    tabs->select(tabLabels.front());

    auto&& content = WidgetBuilder::createPanel(
        { "100%",
          ("100% - " + std::to_string(Sizers::getBaseContainerHeight()))
              .c_str() });
    content->setPosition({ "0%", Sizers::getBaseContainerHeight() });

    tabs->onTabSelect(onTabChange);
    if (tabsId)
        finalPanel->add(tabs, tabsId.value());
    else
        finalPanel->add(tabs);
    finalPanel->add(content, contentPanelId);

    return finalPanel;
}

tgui::Label::Ptr WidgetBuilder::createTooltip(const std::string& text)
{
    auto label = tgui::Label::create(text);
    label->getRenderer()->setBackgroundColor(sf::Color::White);
    label->getRenderer()->setBorders(tgui::Borders::Outline(1));
    label->getRenderer()->setBorderColor(sf::Color::Black);
    label->setTextSize(Sizers::getBaseTextSize());
    return label;
}
