#include <builder/VertexArrayBuilder.hpp>

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
    const float ltx = static_cast<float>(
        clipRect.left + (face.flipTexture ? rightHint : leftHint));
    const float rtx = static_cast<float>(
        clipRect.left + (face.flipTexture ? leftHint : rightHint));
    const float tty = static_cast<float>(clipRect.top);
    const float bty = static_cast<float>(clipRect.top + clipRect.height);
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

    uint16_t left = static_cast<uint16_t>(face.leftHeight);
    uint16_t right = static_cast<uint16_t>(face.rightHeight);

    auto light1 = sf::Color(left >> 8, left & 255, face.brightness, 0b00);
    auto light2 = sf::Color(right >> 8, right & 255, face.brightness, 0b10);
    auto light3 = sf::Color(right >> 8, right & 255, face.brightness, 0b11);
    auto light4 = sf::Color(left >> 8, left & 255, face.brightness, 0b01);

    // Upper triangle ABD
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.leftColumn), leftHeightTop),
        light1,
        { ltx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.rightColumn), rightHeightTop),
        light2,
        { rtx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.leftColumn), leftHeightBottom),
        light4,
        { ltx, bty }));

    // Bottom triangle DBC
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.leftColumn), leftHeightBottom),
        light4,
        { ltx, bty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.rightColumn), rightHeightTop),
        light2,
        { rtx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f(static_cast<float>(face.rightColumn), rightHeightBottom),
        light3,
        { rtx, bty }));
}

void VertexObjectBuilder::makeFlat(
    sf::VertexArray& quads,
    float midHeight,
    TexturedFlat&& flat,
    dgm::Clip& clip)
{
    auto&& clipRect = clip.getFrame(flat.textureId);
    const float ltx = static_cast<float>(clipRect.left);
    const float rtx = static_cast<float>(clipRect.left + clipRect.width);
    const float tty = static_cast<float>(clipRect.top);
    const float bty = static_cast<float>(clipRect.top + clipRect.height);

    const auto light1 =
        sf::Color(flat.brightness, flat.brightness, flat.brightness, 0b00);
    const auto light2 =
        sf::Color(flat.brightness, flat.brightness, flat.brightness, 0b10);
    const auto light3 =
        sf::Color(flat.brightness, flat.brightness, flat.brightness, 0b11);
    const auto light4 =
        sf::Color(flat.brightness, flat.brightness, flat.brightness, 0b01);

    const float h0 = midHeight - flat.vertices[0].y * flat.heightHint;
    const float h1 = midHeight - flat.vertices[1].y * flat.heightHint;
    const float h2 = midHeight - flat.vertices[2].y * flat.heightHint;
    const float h3 = midHeight - flat.vertices[3].y * flat.heightHint;

    if (std::isinf(h0) || std::isinf(h1) || std::isinf(h2) || std::isinf(h3))
        return;

    // Upper triangle
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[0].x, h0 }, light1, { ltx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[1].x, h1 }, light2, { rtx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[2].x, h2 }, light3, { rtx, bty }));

    // Bottom triangle
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[0].x, h0 }, light1, { ltx, tty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[2].x, h2 }, light3, { rtx, bty }));
    quads.append(sf::Vertex(
        sf::Vector2f { flat.vertices[3].x, h3 }, light4, { ltx, bty }));
}