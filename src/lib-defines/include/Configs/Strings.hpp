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
        } // namespace MainMenu

        namespace Options
        {
            RAWSTRING TITLE = "options";
            RAWSTRING DISPLAY = "display";
            RAWSTRING AUDIO = "audio";
            RAWSTRING INPUT = "input";
            RAWSTRING FULLSCREEN = "fullscreen";
            RAWSTRING SET_RESOLUTION = "set resolution";
            RAWSTRING SOUND_VOLUME = "sound volume";
            RAWSTRING MUSIC_VOLUME = "music volume";
            RAWSTRING MOUSE_SENSITIVITY = "mouse sensitivity";
        } // namespace Options
    }     // namespace AppState

    namespace Dialog
    {
        namespace Title
        {
            RAWSTRING WARNING = "Warning";
            RAWSTRING OPEN_LEVEL = "Open level";
            RAWSTRING SAVE_LEVEL = "Save level";
        } // namespace Title

        namespace Message
        {
            RAWSTRING UNSAVED_CHANGES =
                "You have some unsaved changes. Do you want to save them "
                "before exiting?";
            RAWSTRING CANNOT_PLAY_LEVEL =
                "Cannot play level because no level is opened.";
        } // namespace Message

        namespace Body
        {
            RAWSTRING SELECT_LEVEL = "Select level";
            RAWSTRING LEVEL_NAME = "Level name";
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
            RAWSTRING LAYER_UP = "Move to upper layer (Ctrl + Up)";
            RAWSTRING LAYER_DOWN = "Move to lower layer (Ctrl + Down)";
        } // namespace ContextMenu
    }     // namespace Editor
} // namespace Strings

#undef RAWSTRING