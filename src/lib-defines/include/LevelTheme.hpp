#pragma once

#include <format>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

enum class [[nodiscard]] SkyboxTheme
{
    Countryside,
    Dawntime,
    Nightsky
};

class SkyboxThemeUtils
{
public:
    static SkyboxTheme fromString(const std::string& str);

    static [[nodiscard]] std::string toString(SkyboxTheme skyboxTheme);

    static [[nodiscard]] std::vector<std::string> getAllNames();
};

enum class [[nodiscard]] TexturePack
{
    AlphaVersion,
    SpaceStation,
    CountrySide,
    Neon
};

class TexturePackUtils
{
public:
    static TexturePack fromString(const std::string& str);

    static [[nodiscard]] std::string toString(TexturePack skyboxTheme);

    static [[nodiscard]] std::vector<std::string> getAllNames();
};

struct LevelTheme
{
    std::string skybox = SkyboxThemeUtils::toString(SkyboxTheme::Countryside);
    std::string textures =
        TexturePackUtils::toString(TexturePack::AlphaVersion);

    static LevelTheme fromJson(const std::string& str) noexcept;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(LevelTheme, skybox, textures);
