#include "include/Dialogs/DialogBuilderHelper.hpp"
#include "include/Configs/Sizers.hpp"
#include "include/Utilities/PropertyTag.hpp"

const sf::Color ROW_BGR_DARK = sf::Color(222, 222, 222);
const sf::Color ROW_BGR_LIGHT = sf::Color(255, 255, 255);
const std::string LABEL_LEFT_MARGIN = "10%";
const std::string LABEL_WIDTH = "40%";
const std::string VALUE_LEFT_MARGIN = "50%";
const std::string VALUE_WIDTH = "20%";
const std::string TAG_LEFT_MARGIN = "70%";

tgui::Panel::Ptr _getRowBackground(unsigned y, const std::string& tooltip)
{
	auto tt = tgui::Label::create(tooltip);
	tt->getRenderer()->setBackgroundColor(sf::Color::White);
	tt->getRenderer()->setBorders(tgui::Borders::Outline(1));
	tt->getRenderer()->setBorderColor(sf::Color::Black);

	auto row = tgui::Panel::create();
	row->setSize("100%", DialogBuilderHelper::getRowHeight());
	row->setPosition({ "0%", y * DialogBuilderHelper::getRowHeight() });
	row->setToolTip(tt);
	row->getRenderer()->setBackgroundColor(y % 2 == 0 ? ROW_BGR_DARK : ROW_BGR_LIGHT);
	return row;
}

tgui::Label::Ptr _getLabel(const std::string& label)
{
	auto l = tgui::Label::create(label);
	l->setSize(LABEL_WIDTH, "100%");
	l->setPosition(LABEL_LEFT_MARGIN, "0%");
	l->setTextSize(Sizers::GetMenuBarTextHeight());
	return l;
}

template<typename T>
void _addOptionUint(
	tgui::Container::Ptr target,
	FormValidatorToken& token,
	const std::string& label,
	const std::string& tooltip,
	T& val,
	unsigned ypos,
	bool enabled,
	bool tag = false)
{
	auto row = _getRowBackground(ypos, tooltip);
	target->add(row);

	auto lbl = _getLabel(label);
	row->add(lbl);

	auto edit = tgui::EditBox::create();
	edit->setSize(VALUE_WIDTH, "100%");
	edit->setPosition(VALUE_LEFT_MARGIN, "0%");
	edit->setText(std::to_string(val));
	edit->setEnabled(enabled);
	edit->getRenderer()->setBorderColor(sf::Color::Black);
	edit->setInputValidator(tgui::EditBox::Validator::UInt);

	auto updateValue = [&val, &token, edit] (const std::string& newVal)
	{
		try
		{
			std::size_t endpos;
			unsigned long value = std::stoul(newVal, &endpos);
			if (value > T(-1)) throw 1;
			val = T(value);
			edit->getRenderer()->setBorderColor(sf::Color::Black);
			token.setValid();
		}
		catch (...)
		{
			edit->getRenderer()->setBorderColor(sf::Color::Red);
			token.invalidate();
		}
	};

	if (enabled)
	{
		edit->connect("TextChanged", updateValue);
	}

	if (tag)
	{
		auto btn = tgui::Button::create("New tag");
		btn->setSize(VALUE_WIDTH, "100%");
		btn->setPosition(TAG_LEFT_MARGIN, "0%");
		btn->setEnabled(enabled);
		btn->connect("pressed", [edit]
			{
				edit->setText(std::to_string(PropertyTag::get().getNewTag()));
			});
		row->add(btn);
	}

	row->add(edit, "EditBox" + label);
}

void DialogBuilderHelper::addOption(
	tgui::ScrollablePanel::Ptr& target,
	const std::string& label,
	const std::string& tooltip,
	bool& data,
	unsigned ypos,
	bool enabled)
{
	auto row = _getRowBackground(ypos, tooltip);
	target->add(row);

	auto lbl = _getLabel(label);
	row->add(lbl);

	auto checkbox = tgui::CheckBox::create();
	checkbox->setSize(row->getSize().y, row->getSize().y);
	checkbox->setPosition(VALUE_LEFT_MARGIN, "0%");
	checkbox->setChecked(data);
	checkbox->setEnabled(enabled);
	checkbox->connect("Changed", [&data] (bool newVal) { data = newVal; });

	row->add(checkbox);
}

void DialogBuilderHelper::addOption(
	tgui::Container::Ptr target,
	FormValidatorToken& token,
	const std::string& label,
	const std::string& tooltip,
	uint32_t& data,
	unsigned ypos,
	bool enabled,
	bool tag)
{
	_addOptionUint(
		target,
		token,
		label,
		tooltip,
		data,
		ypos,
		enabled,
		tag);
}

void DialogBuilderHelper::addOption(
	tgui::ScrollablePanel::Ptr& target,
	FormValidatorToken& token,
	const std::string& label,
	const std::string& tooltip,
	uint16_t& data,
	unsigned ypos,
	bool enabled)
{
	_addOptionUint(
		target->cast<tgui::Container>(),
		token,
		label,
		tooltip,
		data,
		ypos,
		enabled);
}

void DialogBuilderHelper::addOption(
	tgui::ScrollablePanel::Ptr& target,
	const std::string& label,
	const std::string& tooltip,
	std::string& data,
	unsigned ypos,
	bool enabled)
{
	auto row = _getRowBackground(ypos, tooltip);
	target->add(row);

	auto lbl = _getLabel(label);
	row->add(lbl);

	auto edit = tgui::EditBox::create();
	edit->setSize(VALUE_WIDTH, "100%");
	edit->setPosition(VALUE_LEFT_MARGIN, "0%");
	edit->setText(data);
	edit->setMaximumCharacters(255);
	edit->setEnabled(enabled);
	edit->getRenderer()->setBorderColor(sf::Color::Black);

	if (enabled)
	{
		edit->connect("TextChanged", [&data] (const std::string& newVal) { data = newVal; });
	}

	row->add(edit);
}

void DialogBuilderHelper::addComboOption(
	tgui::Container::Ptr target,
	const std::string& label,
	const std::string& tooltip,
	const std::vector<std::string>& values,
	std::function<void(std::size_t)> onChangeCallback,
	std::size_t initialIndex,
	unsigned ypos)
{
	auto row = _getRowBackground(ypos, tooltip);
	target->add(row);

	auto lbl = _getLabel(label);
	row->add(lbl);

	auto combo = tgui::ComboBox::create();
	combo->setSize(VALUE_WIDTH, "100%");
	combo->setPosition(VALUE_LEFT_MARGIN, "0%");

	for (auto&& value : values)
		combo->addItem(value);
	combo->connect("ItemSelected", [combo, onChangeCallback]
	{
		onChangeCallback(combo->getSelectedItemIndex());
	});
	combo->setSelectedItemByIndex(initialIndex);
	row->add(combo);
}

unsigned int DialogBuilderHelper::getRowHeight()
{
	return Sizers::GetMenuBarHeight();
}
