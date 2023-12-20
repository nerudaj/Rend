#pragma once

#include "Interfaces/CurrentLayerObserverInterface.hpp"
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <string>

class [[nodiscard]] LayerController final : public LayerObserverInterface
{
public:
    LayerController(tgui::Gui& gui)
    {
        buildIconTextures();
        buildLayoutForIcon(gui);
        updateIcon();
    }

public: // LayerObserverInterface
    std::size_t getCurrentLayerIdx() const noexcept override;

    std::size_t getLayerCount() const noexcept override;

public:
    void moveUp();

    void moveDown();

    std::string toString() const;

private:
    void updateIcon();
    void buildIconTextures();
    void buildLayoutForIcon(tgui::Gui& gui);

private:
    tgui::Panel::Ptr iconDisplay;
    std::array<tgui::Texture, 2> layerIcons;
    std::size_t layerIdx = 0;
};