#pragma once

#include <LoggerInterface.hpp>
#include <Memory.hpp>
#include <functional>
#include <string>

class MapLoaderInterface
{
public:
    virtual ~MapLoaderInterface() = default;

public:
    virtual std::string loadMapInBase64(
        const std::string& mapPackName, const std::string& mapName) const = 0;
};

struct [[nodiscard]] ServerDependencies final
{
    mem::Rc<LoggerInterface> logger;
    mem::Rc<MapLoaderInterface> mapLoader;
};
