#pragma once

#define RAWSTRING constexpr const char*

namespace Strings
{
    namespace AppState
    {
        RAWSTRING CTX_MENU_NAME = "File";

        namespace ContextMenu
        {
            RAWSTRING NEW = "New (Ctrl+N)";
            RAWSTRING LOAD = "Load (Ctrl+O)";
            RAWSTRING SAVE = "Save (Ctrl+S)";
            RAWSTRING SAVE_AS = "Save as...";
            RAWSTRING PLAY = "Play (Ctrl+F5)";
            RAWSTRING PLAY2P = "Play with bot";
            RAWSTRING UNDO = "Undo (Ctrl+Z)";
            RAWSTRING REDO = "Redo (Ctrl+Y)";
            RAWSTRING EXIT = "Back to menu";
        } // namespace ContextMenu

        namespace MainMenu
        {
            RAWSTRING PLAY = "play";
            RAWSTRING EDITOR = "editor";
            RAWSTRING OPTIONS = "options";
            RAWSTRING EXIT = "exit";
            RAWSTRING BACK = "back";
            RAWSTRING BACK_TO_MENU = "back";
            RAWSTRING NEXT_MAP = "next map";
        } // namespace MainMenu

        namespace Options
        {
            RAWSTRING TITLE = "options";
            RAWSTRING DISPLAY = "display";
            RAWSTRING AUDIO = "audio";
            RAWSTRING INPUT = "input";
            RAWSTRING FULLSCREEN = "fullscreen";
            RAWSTRING SET_RESOLUTION = "resolution";
            RAWSTRING USE_DITHERED_SHADES = "dithered shadows";
            RAWSTRING SHOW_FPS = "show fps";
            RAWSTRING SHOW_CROSSHAIR = "show crosshair";
            RAWSTRING FOV = "fov";
            RAWSTRING SOUND_VOLUME = "sound volume";
            RAWSTRING MUSIC_VOLUME = "music volume";
            RAWSTRING MOUSE_SENSITIVITY = "mouse sensitivity";
            RAWSTRING TURN_SENSITIVITY = "turn sensitivity";
            RAWSTRING GAMEPAD_DEADZONE = "gamepad deadzone";
            RAWSTRING SWAP_ON_PICKUP = "autoswap weapon on pickup";
            RAWSTRING HUD_UI_SCALE = "hud ui scale";
        } // namespace Options

        namespace GameSetup
        {
            RAWSTRING TITLE = "game setup";
            RAWSTRING PLAYER_COUNT = "player count";
            RAWSTRING SELECT_PACK = "select map pack";
            RAWSTRING SELECT_MAPS = "configure map rotation";
            RAWSTRING FRAGLIMIT = "frag limit";
            RAWSTRING START_GAME = "start game";
            RAWSTRING DOTDOTDOT = "...";
        } // namespace GameSetup

        namespace Pause
        {
            RAWSTRING TITLE = "pause";
            RAWSTRING RESUME = "resume";
            RAWSTRING TO_MENU = "main menu";
        } // namespace Pause

        namespace Scores
        {
            RAWSTRING TITLE = "results";
            RAWSTRING PLAYER_TH = "player";
            RAWSTRING SCORE_TH = "score";
        } // namespace Scores
    }     // namespace AppState

    namespace Dialog
    {
        namespace Title
        {
            RAWSTRING WARNING = "Warning";
            RAWSTRING OPEN_LEVEL = "Open level";
            RAWSTRING SAVE_LEVEL = "Save level";
            RAWSTRING EDIT_METADATA = "Edit level metadata";
            RAWSTRING NEW_LEVEL = "New level";
            RAWSTRING ERR = "Error";
            RAWSTRING LOADING = "Loading";
            RAWSTRING SELECT_MAPS = "select maps";
        } // namespace Title

        namespace Message
        {
            RAWSTRING UNSAVED_CHANGES =
                "You have some unsaved changes. Do you want to save them "
                "before exiting?";
            RAWSTRING CANNOT_PLAY_LEVEL =
                "Cannot play level because no level is opened.";
            RAWSTRING LEVEL_NAME_EMPTY = "Level name cannot be empty.";
            RAWSTRING LEVEL_NAME_TAKEN = "Level name is already taken.";
            RAWSTRING BAKING_LIGHTS = "Baking in lights...";
            RAWSTRING COMPUTING_DISTANCE_INDEX = "Computing distance index...";
            RAWSTRING PACK_NAME_EMPTY = "Map pack name cannot be empty.";
            RAWSTRING PACK_NAME_TAKEN = "Map pack name is already taken.";
        } // namespace Message

        namespace Body
        {
            RAWSTRING CREATE_PACK = "or add new";
            RAWSTRING SELECT_PACK = "Select map pack";
            RAWSTRING SELECT_LEVEL = "Select level";
            RAWSTRING LEVEL_NAME = "Level name";
            RAWSTRING ADD = "Add";
        } // namespace Body
    }     // namespace Dialog

    namespace Editor
    {
        namespace ContextMenu
        {
            RAWSTRING MESH_MODE = "Mesh mode (M)";
            RAWSTRING ITEM_MODE = "Items mode (I)";
            RAWSTRING TRIG_MODE = "Trigger mode (T)";
            RAWSTRING RESIZE = "Resize level (R)";
            RAWSTRING SHRINK = "Shrink level to fit (S)";
            RAWSTRING EDIT_METADATA = "Edit metadata";
            RAWSTRING LAYER_UP = "Move to upper layer (Ctrl + Up)";
            RAWSTRING LAYER_DOWN = "Move to lower layer (Ctrl + Down)";
        } // namespace ContextMenu

        namespace NewLevel
        {
            RAWSTRING WIDTH = "Level width";
            RAWSTRING HEIGHT = "Level height";
            RAWSTRING SKYBOX_THEME = "Skybox theme";
            RAWSTRING TEXTURE_PACK = "Texture pack";
            RAWSTRING AUTHOR = "Author name";
        } // namespace NewLevel
    }     // namespace Editor

    namespace Game
    {
        RAWSTRING RESPAWN_PROMPT = "Press [Space] to respawn";
        RAWSTRING SUICIDE = "You killed yourself";
        RAWSTRING KILLED_BY = "{} killed you";
        RAWSTRING YOU_KILLED = "You killed {}";
    } // namespace Game

    namespace Level
    {
        RAWSTRING SKYBOX_COUNTRY = "Day";
        RAWSTRING SKYBOX_DAWNTIME = "Dusk";
        RAWSTRING SKYBOX_SPACE = "Night";
        RAWSTRING TEXTURES_ALPHA = "AlphaVersion";
        RAWSTRING TEXTURES_SPACE = "Space station";
        RAWSTRING TEXTURES_COUNTRY = "Countryside";
        RAWSTRING TEXTURES_NEON = "Neon";
    } // namespace Level

    RAWSTRING TITLE = "rend";
    RAWSTRING CAPTITLE = "Rend";
} // namespace Strings

#undef RAWSTRING