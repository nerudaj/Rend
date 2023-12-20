#include "Tools/LayerController.hpp"
#include <string>

std::size_t LayerController::getCurrentLayerIdx() const noexcept
{
    return layerIdx;
}

std::size_t LayerController::getLayerCount() const noexcept
{
    return 2;
}

void LayerController::moveUp()
{
    if (layerIdx + 1 < getLayerCount())
    {
        ++layerIdx;
    }

    updateIcon();
}

void LayerController::moveDown()
{
    if (layerIdx > 0)
    {
        --layerIdx;
    }

    updateIcon();
}

std::string LayerController::toString() const
{
    switch (getCurrentLayerIdx())
    {
    case 0:
        return "Bottom layer";
    case 1:
        return "Middle layer";
    case 2:
        return "Upper layer";
    default:
        return "ERROR: Too many layers";
    }
}

[[nodiscard]] static sf::ConvexShape createShape(
    const sf::Vector2f& dim, const sf::Vector2f& origin, sf::Color color)
{
    sf::ConvexShape shape;
    shape.setPointCount(4);
    shape.setPoint(0, sf::Vector2f(dim.x / 2.f, origin.y));
    shape.setPoint(1, sf::Vector2f(dim.x, origin.y + dim.y / 3.f));
    shape.setPoint(2, sf::Vector2f(dim.x / 2.f, origin.y + 2 * dim.y / 3.f));
    shape.setPoint(3, sf::Vector2f(0.f, origin.y + dim.y / 3.f));
    shape.setFillColor(color);
    return shape;
}

[[nodiscard]] static const sf::Texture&
getIconTexture(sf::RenderTexture& render, bool upper)
{
    const auto gray = sf::Color(128, 128, 128);
    const auto dim = sf::Vector2f(render.getSize());

    render.clear(sf::Color::Transparent);

    render.draw(createShape(
        dim, sf::Vector2f(0.f, dim.y / 3.f), upper ? gray : sf::Color::Green));
    render.draw(createShape(
        dim, sf::Vector2f(0.f, 0.f), upper ? sf::Color::Green : gray));

    render.display();

    return render.getTexture();
}

void LayerController::updateIcon()
{
    iconDisplay->getRenderer()->setTextureBackground(
        layerIcons.at(getCurrentLayerIdx()));
}

void LayerController::buildIconTextures()
{
    auto render = sf::RenderTexture();
    render.create(128u, 128u);
    layerIcons[0] = tgui::Texture(getIconTexture(render, false));
    layerIcons[1] = tgui::Texture(getIconTexture(render, true));
}

void LayerController::buildLayoutForIcon(tgui::Gui& gui)
{
    iconDisplay = tgui::Panel::create({ "10%", "10%" });
    iconDisplay->setPosition({ "1%", "89%" });
    gui.add(iconDisplay);
}
