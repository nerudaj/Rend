#include "ToolProperties/TriggerToolProperty.hpp"
#include "Dialogs/DialogBuilderHelper.hpp"

void TriggerToolProperty::fillEditDialog(
    tgui::Panel::Ptr& panel, FormValidatorToken& formValidatorToken)
{
    using namespace DialogBuilderHelper;

    if (!actionDefinitions->contains(data.id))
    {
        throw std::runtime_error(
            "Trigger already has action with ID higher "
            "than provided action annotations");
    }

    auto dst = tgui::ScrollablePanel::create();
    panel->add(dst);

    const bool DISABLED = false;

    unsigned row = 0;
    addOption(
        dst,
        formValidatorToken,
        "X coordinate:",
        "Measured in pixels from top-left corner",
        data.x,
        row++);
    addOption(
        dst,
        formValidatorToken,
        "Y coordinate:",
        "Measured in pixels from top-left corner",
        data.y,
        row++);
    addOption(
        dst,
        formValidatorToken,
        "Layer ID:",
        "ID of layer where trigger is located",
        data.layerIdx,
        row++,
        DISABLED);
    if (data.areaType == LevelD::Trigger::AreaType::Circle)
    {
        addOption(
            dst,
            formValidatorToken,
            "Radius:",
            "Measured in pixels",
            data.radius,
            row++);
    }
    else
    {
        addOption(
            dst,
            formValidatorToken,
            "Width:",
            "Measured in pixels",
            data.width,
            row++);
        addOption(
            dst,
            formValidatorToken,
            "Height:",
            "Measured in pixels",
            data.height,
            row++);
    }
    addOption(
        dst,
        formValidatorToken,
        "Trigger type:",
        "How the trigger should be executed",
        data.type,
        row++);
    addOption(
        dst,
        formValidatorToken,
        "Tag:",
        "Value used to group related objects",
        data.tag,
        row++,
        true,
        true);
    addOption(
        dst, "Metadata:", "Text field for custom data", data.metadata, row++);

    auto paramsContainer = tgui::Panel::create();

    auto buildParams = [](tgui::Panel::Ptr paramsContainer,
                          FormValidatorToken& formValidatorToken,
                          uint32_t* firstParamAddress,
                          const std::vector<std::string>& paramNames)
    {
        paramsContainer->removeAllWidgets();
        paramsContainer->setSize("100%", getRowHeight() * paramNames.size());

        unsigned row2 = 0;
        for (auto&& param : paramNames)
        {
            addOption(
                paramsContainer->cast<tgui::Container>(),
                formValidatorToken,
                param,
                "Action parameter",
                firstParamAddress[row2],
                row2++);
        }
    };

    auto itemChanged = [&, paramsContainer](std::size_t index)
    {
        if (index == data.id) return;

        data.id = index;
        buildParams(
            paramsContainer,
            formValidatorToken,
            &(data.a1),
            actionDefinitions->at(data.id).params);
    };

    addComboOption(
        dst->cast<tgui::Container>(),
        "Action",
        "Select action to execute when trigger is activated",
        getActionNames(),
        itemChanged,
        data.id,
        row++);

    paramsContainer->setPosition("0%", getRowHeight() * row);
    buildParams(
        paramsContainer,
        formValidatorToken,
        &(data.a1),
        actionDefinitions->at(data.id).params);
    dst->add(paramsContainer);
}

std::vector<std::string> TriggerToolProperty::getActionNames() const
{
    std::vector<std::string> result;
    result.reserve(actionDefinitions->size());

    for (auto&& [_, def] : *actionDefinitions)
        result.push_back(def.name);

    return result;
}
