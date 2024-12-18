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
            RAWSTRING JOIN = "join game";
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
            RAWSTRING PLAYER = "player";
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
            RAWSTRING CURSOR_SENSITIVITY = "cursor sensitivity";
            RAWSTRING SWAP_ON_PICKUP = "autoswap weapon on pickup";
            RAWSTRING TEAM_PREFERENCE = "team preference";
            RAWSTRING HUD_UI_SCALE = "hud ui scale";
            RAWSTRING RESOLUTION_CHANGE_WARNING =
                "it is recommended to restart the game after changing "
                "resolution to prevent visual artifacts in the game.";
            RAWSTRING PLAYER_NAME = "player name";
        } // namespace Options

        namespace GameSetup
        {
            RAWSTRING TITLE = "game setup";
            RAWSTRING SERVER_IP = "server ip";
            RAWSTRING MAX_NPCS = "maximum number of bots";
            RAWSTRING SELECT_PACK = "select map pack";
            RAWSTRING RANDOM_ROTATION = "random map rotation";
            RAWSTRING SELECT_MAPS = "configure map rotation";
            RAWSTRING SELECT_GAMEMODE = "choose game mode";
            RAWSTRING GAMEMODE_DM = "deathmatch";
            RAWSTRING GAMEMODE_SCTF = "single-flag ctf";
            RAWSTRING POINTLIMIT = "point limit";
            RAWSTRING START_GAME = "start game";
            RAWSTRING DOTDOTDOT = "...";
        } // namespace GameSetup

        namespace JoinGame
        {
            RAWSTRING TITLE = "join game";
            RAWSTRING CONFIRM = "join";
            RAWSTRING INPUT_IP = "host's ip";
        } // namespace JoinGame

        namespace PeerLobby
        {
            RAWSTRING TITLE = "lobby";
            RAWSTRING READY = "ready";
            RAWSTRING NOT_READY = "not ready";
            RAWSTRING DISCONNECTED = "disconnected";
            RAWSTRING PNAME = "player name";
            RAWSTRING IS_READY = "is ready";
            RAWSTRING GAME_SETUP = "game setup";
            RAWSTRING PLAYER_SETUP = "player setup";
            RAWSTRING PLAYERS = "players";
        } // namespace PeerLobby

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
            RAWSTRING WON = "won";
            RAWSTRING RED_TEAM_WON = "red team won";
            RAWSTRING BLUE_TEAM_WON = "blue team won";
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

        RAWSTRING SUBMIT_OK = "Ok";
        RAWSTRING SUBMIT_CANCEL = "Cancel";

        namespace EditProperty
        {
            RAWSTRING TILE_X = "Tile X:";
            RAWSTRING TILE_X_TOOLTIP = "X coordinate of the tile";
            RAWSTRING TILE_Y = "Tile Y:";
            RAWSTRING TILE_Y_TOOLTIP = "Y coordinate of the tile";
            RAWSTRING LAYER_ID = "Layer ID:";
            RAWSTRING LAYER_ID_TOOLTIP = "In which layer is tile located";
            RAWSTRING IS_IMPASSABLE = "Is impassable:";
            RAWSTRING IS_IMPASSABLE_TOOLTIP =
                "Whether this tile blocks the player";
            RAWSTRING IS_DEFAULT_IMPASSABLE = "Impassable by default:";
            RAWSTRING IS_DEFAULT_IMPASSABLE_TOOLTIP =
                "Whether this type of tile is impassable by default";
            RAWSTRING ITEM_ID = "Item ID:";
            RAWSTRING ITEM_ID_TOOLTIP = "Unique identifier of the object";
            RAWSTRING ITEM_X = "X coordinate:";
            RAWSTRING ITEM_POS_TOOLTIP =
                "Measured in pixels from top-left corner";
            RAWSTRING ITEM_Y = "Y coordinate:";
            RAWSTRING ITEM_FLAGS = "Flags:";
            RAWSTRING ITEM_FLAGS_TOOLTIP =
                "16 bit value to alter behaviour of this object";
            RAWSTRING ITEM_ROTATION = "Spawn rotation:";
            RAWSTRING ITEM_ROTATION_TOOLTIP =
                "Rotation is taken into account for player spawns";
        } // namespace EditProperty
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
            RAWSTRING COMPAT = "Gamemode";
        } // namespace NewLevel

        RAWSTRING EDIT_PROPERTIES = "Edit Properties";
    } // namespace Editor

    namespace Game
    {
        RAWSTRING RESPAWN_PROMPT = "press [Space] to respawn";
        RAWSTRING SUICIDE = "you killed yourself";
        RAWSTRING KILLED_BY = "{} killed you";
        RAWSTRING YOU_KILLED = "you killed {}";
        RAWSTRING XY_SCORED = "{} scored!";
        RAWSTRING YOU_SCORED = "you scored!";
        RAWSTRING YOU_HAVE_FLAG = "you have the flag!";
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
        RAWSTRING GAMEMODE_DM = "Deathmatch";
        RAWSTRING GAMEMODE_SCTF = "Single-flag CTF";
    } // namespace Level

    RAWSTRING TITLE = "rend";
    RAWSTRING CAPTITLE = "Rend";

    namespace Error
    {
        RAWSTRING DESYNCED =
            "You were disconnected, because your game got desynced";
        RAWSTRING SERVER_OFFLINE =
            "You were disconnected, because server appears to be offline";
        RAWSTRING NO_MAP_SELECTED =
            "You were disconnected, because no map was selected";
        RAWSTRING BAD_GAMEMODE =
            "You were disconnected, because the selected map was not "
            "compatible with selected gamemode";
    } // namespace Error

    namespace Enums
    {
        RAWSTRING TEAM_NONE = "none";
        RAWSTRING TEAM_RED = "red";
        RAWSTRING TEAM_BLUE = "blue";
    } // namespace Enums
} // namespace Strings

#undef RAWSTRING