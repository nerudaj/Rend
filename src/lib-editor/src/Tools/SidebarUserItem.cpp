#include "include/Tools/SidebarUserItem.hpp"

void SidebarUserItem::configure(const std::vector<PathRectPair>& textureClipPairs)
{
	renderData.clear();
	penHistory.clear();
	penValue = 0;

	renderData.reserve(textureClipPairs.size());
	for (auto&& [path, clip] : textureClipPairs)
	{
		renderData.push_back({});
		if (!renderData.back().texture.loadFromFile(path.string()))
		{
			throw std::runtime_error("Could not load texture: " + path.string());
		}

		renderData.back().clip = clip;
	}

	for (auto&& rd : renderData)
	{
		rd.sprite.setTexture(rd.texture);
		rd.sprite.setTextureRect(rd.clip);

		if (rd.clip.width == rd.clip.height)
		{
			rd.guiTexture = tgui::Texture(rd.texture, rd.clip);
		}
		else
		{
			const int size = std::max(rd.clip.width, rd.clip.height);

			sf::Image txImg = rd.texture.copyToImage();
			sf::Image sqImg;
			sqImg.create(size, size, sf::Color::Transparent);

			// Copy txImg to sqImg, centered
			unsigned xOffset = (size - rd.clip.width) / 2;
			unsigned yOffset = (size - rd.clip.height) / 2;
			for (int y = 0; y < rd.clip.height; y++)
			{
				for (int x = 0; x < rd.clip.width; x++)
				{
					sqImg.setPixel(
						x + xOffset,
						y + yOffset,
						txImg.getPixel(x + rd.clip.left, y + rd.clip.top));
				}
			}

			sf::Texture texture;
			texture.loadFromImage(sqImg);
			rd.guiTexture = tgui::Texture(texture);
		}
	}
	/*for (unsigned i = 0; i < renderData.size(); i++)
	{
		const auto& clip = renderData[i].clip;

		renderData[i].sprite.setTexture(renderData[i].texture);
		renderData[i].sprite.setTextureRect(clip);

		if (clip.width == clip.height)
		{
			renderData[i].guiTexture = tgui::Texture(renderData[i].texture, clip);
		}
		else
		{
			const int size = std::max(clip.width, clip.height);

			sf::Image txImg = renderData[i].texture.copyToImage();
			sf::Image sqImg;
			sqImg.create(size, size, sf::Color::Transparent);

			// Copy txImg to sqImg, centered
			unsigned xOffset = (size - clip.width) / 2;
			unsigned yOffset = (size - clip.height) / 2;
			for (int y = 0; y < clip.height; y++)
			{
				for (int x = 0; x < clip.width; x++)
				{
					sqImg.setPixel(x + xOffset, y + yOffset, txImg.getPixel(x + clip.left, y + clip.top));
				}
			}

			sf::Texture texture;
			texture.loadFromImage(sqImg);
			renderData[i].guiTexture = tgui::Texture(texture);
		}
	}*/
}

void SidebarUserItem::drawSprite(
	tgui::Canvas::Ptr& canvas,
	unsigned spriteId,
	const sf::Vector2i& pos,
	bool selected,
	float opacity)
{
	auto&& rd = renderData.at(spriteId);

	auto position = sf::Vector2f(
		float(int(pos.x) - rd.clip.width / 2),
		float(int(pos.y) - rd.clip.height / 2));
	rd.sprite.setColor(sf::Color(255, 255, 255, opacity));
	rd.sprite.setPosition(position);

	canvas->draw(rd.sprite);

	// Outline for selected items
	if (selected)
	{
		outline.setPosition(position);
		outline.setSize({ float(rd.clip.width), float(rd.clip.height) });
		canvas->draw(outline);
	}
}
