module;

#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

export module LayoutBuilder;

export import TguiHelper;
export import WidgetBuilder;

export class [[nodiscard]] FinalizedLayoutBuilder final
{
public:
    FinalizedLayoutBuilder(tgui::Panel::Ptr container) : container(container) {}

public:
    [[nodiscard]] inline tgui::Panel::Ptr build() const
    {
        return container;
    }

private:
    tgui::Panel::Ptr container;
};

export class [[nodiscard]] LayoutBuilderWithContentAndBackButton final
{
public:
    LayoutBuilderWithContentAndBackButton(tgui::Panel::Ptr container)
        : container(container)
    {
    }

public:
    FinalizedLayoutBuilder withNoSubmitButton() const
    {
        return FinalizedLayoutBuilder(container);
    }

    FinalizedLayoutBuilder withSubmitButton(tgui::Button::Ptr button);

private:
    tgui::Panel::Ptr container;
};

export class [[nodiscard]] LayoutBuilderWithContent final
{
public:
    LayoutBuilderWithContent(tgui::Panel::Ptr container) : container(container)
    {
    }

public:
    LayoutBuilderWithContentAndBackButton withNoBackButton() const
    {
        return LayoutBuilderWithContentAndBackButton(container);
    }

    LayoutBuilderWithContentAndBackButton
    withBackButton(tgui::Button::Ptr button);

private:
    tgui::Panel::Ptr container;
};

export class [[nodiscard]] LayoutBuilderWithBackgroundAndTitle final
{
public:
    LayoutBuilderWithBackgroundAndTitle(tgui::Panel::Ptr container)
        : container(container)
    {
    }

public:
    LayoutBuilderWithContent withContent(tgui::Container::Ptr content);

private:
    tgui::Panel::Ptr container;
};

export enum class [[nodiscard]] HeadingLevel { H1, H2 };

export class [[nodiscard]] LayoutBuilderWithBackground final
{
public:
    LayoutBuilderWithBackground(tgui::Panel::Ptr container)
        : container(container)
    {
    }

public:
    LayoutBuilderWithBackgroundAndTitle
    withTitle(const std::string& title, HeadingLevel level);

private:
    tgui::Panel::Ptr container;
};

export class [[nodiscard]] LayoutBuilder final
{
public:
    static LayoutBuilderWithBackground
    withBackgroundImage(const sf::Texture& texture);

    static LayoutBuilderWithBackground withNoBackgroundImage();
};