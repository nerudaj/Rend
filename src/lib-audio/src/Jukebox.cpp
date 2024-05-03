#include "Jukebox.hpp"

void Jukebox::playGeneric(PlaybackMode mode, const std::vector<std::string>& songs)
{
	if (playbackMode == mode) return;
	playbackMode = mode;
	player.openFromFile(getSongPath(getRandomSongName(songs)));
	player.setLoop(true);
	player.play();
}