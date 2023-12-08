module;

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

export module Playlist;

export
{

    struct Playlist
    {
        std::vector<std::string> titleSongs;
        std::vector<std::string> ingameSongs;
        std::vector<std::string> interludeSongs;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
        Playlist, titleSongs, ingameSongs, interludeSongs);
}