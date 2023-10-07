#include <cmath>
#include <utils/RaycastingBase.hpp>

RaycastingBase::RaycastingState
RaycastingBase::computeInitialRaycastringStateFromPositionAndDirection(
    const sf::Vector2f& pos, const sf::Vector2f& dir) noexcept
{
    auto tile = sf::Vector2u(pos);
    auto tileStep = sf::Vector2i(0, 0);
    auto rayStep = sf::Vector2f(
        std::sqrt(1 + (dir.y * dir.y) / (dir.x * dir.x)),
        std::sqrt(1 + (dir.x * dir.x) / (dir.y * dir.y)));
    auto&& intercept = sf::Vector2f();

    if (dir.x < 0)
    {
        tileStep.x = -1;
        intercept.x = (pos.x - tile.x) * rayStep.x;
    }
    else
    {
        tileStep.x = 1;
        intercept.x = (tile.x + 1 - pos.x) * rayStep.x;
    }

    if (dir.y < 0)
    {
        tileStep.y = -1;
        intercept.y = (pos.y - tile.y) * rayStep.y;
    }
    else
    {
        tileStep.y = 1;
        intercept.y = (tile.y + 1 - pos.y) * rayStep.y;
    }

    return RaycastingState { .tile = tile,
                             .tileStep = tileStep,
                             .rayStep = rayStep,
                             .intercept = intercept };
}

bool RaycastingBase::advanceRay(
    sf::Vector2f& intercept,
    sf::Vector2u& tile,
    const sf::Vector2f& rayStep,
    const sf::Vector2i& tileStep) noexcept
{
    if (intercept.x < intercept.y)
    {
        intercept.x += rayStep.x;
        tile.x += tileStep.x;
        return false; // vertical north-south
    }
    else
    {
        intercept.y += rayStep.y;
        tile.y += tileStep.y;
        return true; // horizontal west-east
    }
}
