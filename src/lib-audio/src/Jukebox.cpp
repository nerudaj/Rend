import Jukebox;

#include <DGM/dgm.hpp>
#include <SFML/Audio.hpp>

void Jukebox::setVolume(float volume)
{
    player.setVolume(volume);
}

void Jukebox::playTitleSong()
{
    playGeneric(PlaybackMode::Title, playlist.titleSongs);
}

void Jukebox::playIngameSong()
{
    playGeneric(PlaybackMode::Ingame, playlist.ingameSongs);
}

void Jukebox::playInterludeSong()
{
    playGeneric(PlaybackMode::Interlude, playlist.interludeSongs);
}

void Jukebox::playGeneric(
    PlaybackMode mode, const std::vector<std::string>& songs)
{
    if (playbackMode == mode) return;
    playbackMode = mode;
    player.openFromFile(getSongPath(getRandomSongName(songs)));
    player.setLoop(true);
    player.play();
}

std::string
Jukebox::getRandomSongName(const std::vector<std::string>& songs) const
{
    return songs[rand() % songs.size()];
}

std::string Jukebox::getSongPath(const std::string& songName) const
{
    return (rootDir / "music" / songName).string();
}
