#pragma once

#include <Configs/Sizers.hpp>
#include <DGM/dgm.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>

import Memory;
import Audio;

class GuiState
{
protected:
    struct [[nodiscard]] ButtonProps final
    {
        std::string label;
        std::function<void(void)> callback;
    };

protected:
    [[nodiscard]] GuiState(
        mem::Rc<tgui::Gui> gui, mem::Rc<AudioPlayer> audioPlayer) noexcept
        : gui(gui), audioPlayer(audioPlayer)
    {
    }

protected:
    [[nodiscard]] tgui::Label::Ptr createWindowTitle(
        tgui::Layout2d position, tgui::Layout2d size, const std::string& text);

    [[nodiscard]] tgui::Label::Ptr
    createH1Title(const std::string& text, const sf::Color color);

    [[nodiscard]] tgui::Panel::Ptr createBackground(
        const dgm::ResourceManager& resmgr, const std::string& imgName) const;

    [[nodiscard]] tgui::Panel::Ptr createPanel(
        const tgui::Layout2d& position,
        const tgui::Layout2d& size,
        tgui::Color bgcolor = tgui::Color(255, 255, 255, 64)) const;

    [[nodiscard]] static tgui::Layout2d getCommonLayoutSize(unsigned rowCount)
    {
        return tgui::Layout2d(
            "60%", tgui::String { std::to_string(5 * rowCount) + "%" });
    }

    [[nodiscard]] static tgui::Layout2d getCommonLayoutPosition()
    {
        return { "20%", "35%" };
    }

    // Creates a button that can be later retrieved from gui using id
    // std::string("Button" + label)
    [[nodiscard]] tgui::Button::Ptr createButton(
        const std::string& label,
        tgui::Layout2d position,
        tgui::Layout2d size,
        std::function<void(void)> onClick);

    [[nodiscard]] tgui::Button::Ptr createBackButton(
        std::function<void(void)> callback, const std::string& label = "back")
    {
        return createButton(
            label,
            { "1%", "94%" },
            { "15%", Sizers::getBaseRowHeight() },
            callback);
    }

    /**
     *  \brief Adds a list of buttons to layout
     *
     *  \param layout        Pointer to existing horizontal or vertical layout
     * which has been added to gui \param properties    Vector of labels and
     * callback functions for the buttons \param gapSize       Specifies size of
     * the space between buttons. This should be number between 0 and 1 and it
     * denotes percentage of the layout size \param textAutoSize  Whether the
     * labels should have automatic text size. This finds the optimal text size
     * which will be shared among all buttons
     */
    void createButtonListInLayout(
        tgui::BoxLayoutRatios::Ptr layout,
        const std::vector<ButtonProps>& properties,
        const float gapSize = 0.1f,
        const bool textAutoSize = true,
        const unsigned textSize = 0);

    virtual void buildLayoutImpl() = 0;

    void buildLayout()
    {
        gui->removeAllWidgets();
        buildLayoutImpl();
    }

    void restoreFocus(sf::RenderWindow& window)
    {
        gui->setWindow(window);
        buildLayout();
    }

protected:
    mem::Rc<tgui::Gui> gui;
    mem::Rc<AudioPlayer> audioPlayer;
};
