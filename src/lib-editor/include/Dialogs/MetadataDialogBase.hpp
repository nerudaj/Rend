#pragma once

#include "FormBuilder.hpp"
#include <LevelTheme.hpp>

class [[nodiscard]] MetadataDialogBase
{
public:
    virtual ~MetadataDialogBase() = default;

public:
    [[nodiscard]] constexpr SkyboxTheme getSkyboxTheme() const noexcept
    {
        return skyboxTheme;
    }

    [[nodiscard]] constexpr TexturePack getTexturePack() const noexcept
    {
        return texturePack;
    }

    [[nodiscard]] constexpr std::string getAuthorName() const noexcept
    {
        return author;
    }

protected:
    void addAttributesToFormular(FormBuilder& builder);

protected:
    SkyboxTheme skyboxTheme = SkyboxTheme {};
    TexturePack texturePack = TexturePack {};
    std::string author = "";
};
