#pragma once

#pragma warning(push, 0)
#include <DGM/dgm.hpp>
#include <SFML/Audio.hpp>
#pragma warning(pop)
#include "Playlist.hpp"
#include <filesystem>

class [[nodiscard]] Jukebox final
{
private:
    enum class PlaybackMode
    {
        None,
        Title,
        Ingame,
        Interlude
    };

public:
    Jukebox(
        const dgm::ResourceManager& resmgr,
        const std::filesystem::path& rootDir) noexcept
        : playlist(resmgr.get<Playlist>("tracks.playlist").value().get())
        , rootDir(rootDir)
    {
    }

    Jukebox(const Jukebox&) = delete;
    Jukebox(Jukebox&&) = default;

public:
    void setVolume(float volume)
    {
        player.setVolume(volume);
    }

    void playTitleSong()
    {
        playGeneric(PlaybackMode::Title, playlist.titleSongs);
    }

    void playIngameSong()
    {
        playGeneric(PlaybackMode::Ingame, playlist.ingameSongs);
    }

    void playInterludeSong()
    {
        playGeneric(PlaybackMode::Interlude, playlist.interludeSongs);
    }

    void stop()
    {
        player.stop();
        playbackMode = PlaybackMode::None;
    }

private:
    void playGeneric(PlaybackMode mode, const std::vector<std::string>& songs);

    std::string getRandomSongName(const std::vector<std::string>& songs) const
    {
        return songs[rand() % songs.size()];
    }

    std::string getSongPath(const std::string& songName) const
    {
        return (rootDir / "music" / songName).string();
    }

private:
    Playlist playlist;
    std::filesystem::path rootDir;
    PlaybackMode playbackMode = PlaybackMode::None;
    sf::Music player;
};
