#include "utils/VertexArrayBuilder.hpp"

void VertexObjectBuilder::makeFace(
    sf::VertexArray& quads,
    float midHeight,
    TexturedFace&& face,
    dgm::Clip& clip)
{
    // Removes some visual artifacts
    if (face.leftHeight < 0.f || face.rightHeight < 0.f) return;

    auto&& clipRect = clip.getFrame(face.textureId);
    const float leftHint =
        clipRect.width
        * (face.flipTexture ? 1.f - face.rightTexHint : face.leftTexHint);
    const float rightHint =
        clipRect.width
        * (face.flipTexture ? 1.f - face.leftTexHint : face.rightTexHint);
    const float ltx = clipRect.left + (face.flipTexture ? rightHint : leftHint);
    const float rtx = clipRect.left + (face.flipTexture ? leftHint : rightHint);
    const float tty = clipRect.top;
    const float bty = clipRect.top + clipRect.heightHint;
    const float leftHeightTop =
        midHeight - face.leftHeight / 2 - face.leftHeight * face.heightHint;
    const float leftHeightBottom =
        midHeight + face.leftHeight / 2 - face.leftHeight * face.heightHint;
    const float rightHeightTop =
        midHeight - face.rightHeight / 2 - face.rightHeight * face.heightHint;
    const float rightHeightBottom =
        midHeight + face.rightHeight / 2 - face.rightHeight * face.heightHint;

    // Removes some visual artifacts
    if (leftHeightBottom < 0.f && rightHeightBottom < 0.f) return;

    auto light = sf::Color(face.brightness, face.brightness, face.brightness);

    // Upper triangle
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.leftColumn), leftHeightTop),
        light,
        { ltx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.rightColumn), rightHeightTop),
        light,
        { rtx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.leftColumn), leftHeightBottom),
        light,
        { ltx, bty }));

    // Bottom triangle
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.leftColumn), leftHeightBottom),
        light,
        { ltx, bty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.rightColumn), rightHeightTop),
        light,
        { rtx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.rightColumn), rightHeightBottom),
        light,
        { rtx, bty }));
}

void VertexObjectBuilder::makeFlat(
    sf::VertexArray& quads,
    float midHeight,
    TexturedFlat&& flat,
    dgm::Clip& clip)
{
    auto&& clipRect = clip.getFrame(flat.textureId);
    const float ltx = clipRect.left;
    const float rtx = clipRect.left + clipRect.width;
    const float tty = clipRect.top;
    const float bty = clipRect.top + clipRect.heightHint;
    auto light = sf::Color(flat.brightness, flat.brightness, flat.brightness);

    const float h0 = midHeight - flat.vertices[0].y * flat.heightHint;
    const float h1 = midHeight - flat.vertices[1].y * flat.heightHint;
    const float h2 = midHeight - flat.vertices[2].y * flat.heightHint;
    const float h3 = midHeight - flat.vertices[3].y * flat.heightHint;

    if (std::isinf(h0) || std::isinf(h1) || std::isinf(h2) || std::isinf(h3))
        return;

    // Upper triangle
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[0].x, h0 }, light, { ltx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[1].x, h1 }, light, { rtx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[2].x, h2 }, light, { rtx, bty }));

    // Bottom triangle
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[0].x, h0 }, light, { ltx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[2].x, h2 }, light, { rtx, bty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[3].x, h3 }, light, { ltx, bty }));
}