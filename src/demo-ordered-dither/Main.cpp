#include <SFML/Graphics.hpp>
#include "RootDir.hpp"
#include <cassert>

int main()
{
	auto&& window = sf::RenderWindow(sf::VideoMode(1280, 720), "");

	sf::Texture texture;
	assert(texture.loadFromFile(ROOTDIR + "/tile.png"));
	sf::Texture noiseTexture;
	assert(noiseTexture.loadFromFile(ROOTDIR + "/noise2.png"));

	sf::Shader shader;
	shader.loadFromFile(ROOTDIR + "/shader.vert", ROOTDIR + "/shader.frag");
	shader.setUniform("noise", noiseTexture);
	shader.setUniform("shadeColor", sf::Glsl::Vec4(sf::Color(0, 0, 0)));

	// 17,29,53
	// 29, 43, 83

	auto&& vertices = sf::VertexArray(sf::Quads, 4);
	vertices[0].position = sf::Vector2f(10.f, 10.f);
	vertices[0].color = sf::Color(0, 255, 255, 0b00);
	vertices[0].texCoords = sf::Vector2f(0.f, 0.f);

	vertices[1].position = sf::Vector2f(1270.f, 190.f);
	vertices[1].color = sf::Color(0, 128, 0, 0b10);
	vertices[1].texCoords = sf::Vector2f(16.f, 0.f);

	vertices[2].position = sf::Vector2f(1270.f, 550.f);
	vertices[2].color = sf::Color(0, 128, 0, 0b11);
	vertices[2].texCoords = sf::Vector2f(16.f, 16.f);

	vertices[3].position = sf::Vector2f(10.f, 710.f);
	vertices[3].color = sf::Color(0, 255, 255, 0b01);
	vertices[3].texCoords = sf::Vector2f(0.f, 16.f);

	auto&& states = sf::RenderStates{};
	states.texture = &texture;
	states.shader = &shader;

	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear();

		window.draw(vertices, states);

		window.display();
	}

	return 0;
}