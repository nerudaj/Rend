#include "include/Utilities/Utilities.hpp"
#include <DGM/classes/Math.hpp>
#include <algorithm>

std::tuple<sf::Image, dgm::Clip> Utilities::createChessboardTexture(const sf::Color& accentColor)
{
	sf::Image img;
	img.create(2, 1);
	img.setPixel(0, 0, sf::Color::Transparent);
	img.setPixel(1, 0, accentColor);
	return { img, dgm::Clip({ 1, 1 }, { 0, 0, 2, 1 }) };
}

sf::Vector2i Utilities::clipNegativeCoords(const sf::Vector2i& v)
{
	return sf::Vector2i(
		std::max(0, v.x),
		std::max(0, v.y)
	);
}

std::optional<TileRect> Utilities::unifyRects(
	const std::optional<TileRect>& a,
	const std::optional<TileRect>& b) noexcept
{
	if (a.has_value() && !b.has_value())
		return a;
	else if (!a.has_value() && b.has_value())
		return b;
	else if (!a.has_value() && !b.has_value())
		return {};

	const auto aVal = a.value();
	const auto bVal = b.value();

	const int left = std::min(aVal.left, bVal.left);
	const int top = std::min(aVal.top, bVal.top);
	const int right = std::max(aVal.right, bVal.right);
	const int bottom = std::max(aVal.bottom, bVal.bottom);

	return TileRect(left, top, right, bottom);
}

sf::ConvexShape Utilities::createArrow(
	const sf::Vector2i& from,
	const sf::Vector2i& to,
	const sf::Color& outlineColor,
	const float arrowThickness)
{
	constexpr float ARROW_SHOULDER_LENGTH = 40.f;
	const float SQRT_2 = std::sqrt(2.f);
	const float SQRT_HALF = std::sqrt(1 / 2.f);

	const auto forward = sf::Vector2f(to - from);
	const auto size = dgm::Math::vectorSize(forward);
	if (size == 0.f) return sf::ConvexShape();

	const auto normal = sf::Vector2f(
		-forward.y,
		forward.x) / size;
	const auto normal45 = dgm::Math::rotateVector(normal, 45);
	const auto normal90 = dgm::Math::rotateVector(normal, 90);
	const auto normal135 = dgm::Math::rotateVector(normal, 135);

	// There is a line intersecting both from and to
	// with equation ax + by + c = 0
	// where a = normal.x and b = normal.y
	const float line_C = -(normal.x * from.x + normal.y * from.y);

	auto mirrorPoint = [&normal, line_C](const sf::Vector2f& vec)
		-> sf::Vector2f
	{
		// Don't need to divide this by anything - normal is unit vector
		const float distance =
			(normal.x * vec.x + normal.y * vec.y + line_C);
		return vec - 2.f * normal * distance;
	};

	const auto toF = sf::Vector2f(to);
	const auto arrowCorner1 = toF + normal45 * ARROW_SHOULDER_LENGTH;
	const auto arrowCorner2 = arrowCorner1 + normal135 * arrowThickness;
	const auto innerArrorCorner =
		toF +
		normal90 * arrowThickness * SQRT_2 +
		normal45 * arrowThickness * SQRT_HALF;
	const auto bottomCorner = sf::Vector2f(from) + normal * arrowThickness / 2.f;

	sf::ConvexShape arrow;
	arrow.setPointCount(9);

	arrow.setPoint(0, toF);
	arrow.setPoint(1, arrowCorner1);
	arrow.setPoint(2, arrowCorner2);
	arrow.setPoint(3, innerArrorCorner);
	arrow.setPoint(4, bottomCorner);
	arrow.setPoint(5, mirrorPoint(bottomCorner));
	arrow.setPoint(6, mirrorPoint(innerArrorCorner));
	arrow.setPoint(7, mirrorPoint(arrowCorner2));
	arrow.setPoint(8, mirrorPoint(arrowCorner1));

	arrow.setOutlineColor(outlineColor);
	arrow.setOutlineThickness(1.f);
	arrow.setFillColor(sf::Color::Transparent);

	return arrow;
}
