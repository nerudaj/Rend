#pragma once

#include <DGM/dgm.hpp>

struct TexturedFace
{
    int leftColumn;
    int rightColumn;
    float leftHeight;
    float rightHeight;
    float leftTexHint;
    float rightTexHint;
    float heightHint;
    std::uint8_t textureId;
    sf::Uint8 brightness = 255;
    bool flipTexture = false;
};

struct TexturedFlat
{
    sf::Vector2f vertices[4];
    float heightHint;
    std::uint8_t textureId;
    sf::Uint8 brightness = 255;
};

class VertexObjectBuilder
{
public:
    static void makeLine(
        sf::VertexArray& lines,
        const sf::Vector2f& a,
        const sf::Vector2f& b,
        dgm::UniversalReference<sf::Color> auto&& color)
    {
        lines.append(sf::Vertex(a, color));
        lines.append(sf::Vertex(b, color));
    }

    static void makeFace(
        sf::VertexArray& quads,
        float midHeight,
        TexturedFace&& face,
        dgm::Clip& clip);

    static void makeFlat(
        sf::VertexArray& quads,
        float midHeight,
        TexturedFlat&& flat,
        dgm::Clip& clip);
};