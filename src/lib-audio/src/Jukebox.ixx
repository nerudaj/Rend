module;

#include <DGM/dgm.hpp>
#include <SFML/Audio.hpp>

export module Jukebox;

import Playlist;

export class [[nodiscard]] Jukebox final
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
    void setVolume(float volume);
    void playTitleSong();
    void playIngameSong();
    void playInterludeSong();

    void stop()
    {
        player.stop();
    }

private:
    void playGeneric(PlaybackMode mode, const std::vector<std::string>& songs);
    std::string getRandomSongName(const std::vector<std::string>& songs) const;
    std::string getSongPath(const std::string& songName) const;

private:
    Playlist playlist;
    std::filesystem::path rootDir;
    PlaybackMode playbackMode = PlaybackMode::None;
    sf::Music player;
};
