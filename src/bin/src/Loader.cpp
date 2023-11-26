#include "Loader.hpp"
#include <LevelD.hpp>
#pragma warning(push, 0)
#include <SFML/Audio.hpp>
#pragma warning(pop)
#include <TGUI/TGUI.hpp>

// Takes std::expected and throws exception if it contains error
#define THROW_ON_ERROR(expr)                                                   \
    if (auto&& result = expr; !result) throw std::runtime_error(result.error());

static inline void
loadTexture(const std::filesystem::path& path, sf::Texture& texture)
{
    texture.loadFromFile(path.string());
}

static inline void loadSfFont(const std::filesystem::path& path, sf::Font& font)
{
    font.loadFromFile(path.string());
}

static inline void
loadSound(const std::filesystem::path& path, sf::SoundBuffer& buffer)
{
    buffer.loadFromFile(path.string());
}

static inline void
loadTguiFont(const std::filesystem::path& path, tgui::Font& font)
{
    font = tgui::Font(path.string());
}

static inline void loadAnimationStates(
    const std::filesystem::path& path, dgm::AnimationStates& animStates)
{
    animStates = dgm::JsonLoader().loadAnimationsFromFile(path);
}

static inline void loadClip(const std::filesystem::path& path, dgm::Clip& clip)
{
    clip = dgm::JsonLoader().loadClipFromFile(path);
}

static inline void loadLevel(const std::filesystem::path& path, LevelD& lvd)
{
    lvd.loadFromFile(path.string());
}

static inline void
loadShader(const std::filesystem::path& path, sf::Shader& shader)
{
    auto&& path1 = path.string() + ".vert";
    auto&& path2 = path.string() + ".frag";

    if (!shader.loadFromFile(path1, path2))
    {
        throw std::runtime_error("Cannot load shader!");
    }
}

void Loader::loadResources(
    dgm::ResourceManager& resmgr, const std::filesystem::path& rootDir)
{
    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<sf::Texture>(
        rootDir / "graphics", loadTexture, { ".png" }));

    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<sf::Font>(
        rootDir / "fonts", loadSfFont, { ".ttf" }));

    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<sf::SoundBuffer>(
        rootDir / "sounds", loadSound, { ".wav" }));

    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<tgui::Font>(
        rootDir / "fonts", loadTguiFont, { ".ttf" }));

    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<dgm::AnimationStates>(
        rootDir / "graphics", loadAnimationStates, { ".anim" }));

    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<dgm::Clip>(
        rootDir / "graphics", loadClip, { ".clip" }));

    THROW_ON_ERROR(resmgr.loadResourcesFromDirectory<LevelD>(
        rootDir / "levels", loadLevel, { ".lvd" }));

    THROW_ON_ERROR(resmgr.loadResource<sf::Shader>(
        rootDir / "shaders" / "shader", loadShader));
    THROW_ON_ERROR(resmgr.loadResource<sf::Shader>(
        rootDir / "shaders" / "ditheredShader", loadShader));
}

#undef THROW_ON_ERROR