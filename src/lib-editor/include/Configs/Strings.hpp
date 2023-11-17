#pragma once

namespace Strings
{
    namespace AppState
    {
        constexpr const char* CTX_MENU_NAME = "File";

        namespace ContextMenu
        {
            constexpr const char* NEW = "New (Ctrl+N)";
            constexpr const char* LOAD = "Load (Ctrl+O)";
            constexpr const char* SAVE = "Save (Ctrl+S)";
            constexpr const char* SAVE_AS = "Save as...";
            constexpr const char* PLAY = "Play (Ctrl+F5)";
            constexpr const char* UNDO = "Undo (Ctrl+Z)";
            constexpr const char* REDO = "Redo (Ctrl+Y)";
            constexpr const char* EXIT = "Exit";
        } // namespace ContextMenu

    } // namespace AppState

    namespace Dialog
    {
        namespace Title
        {
            constexpr const char* WARNING = "Warning";
            constexpr const char* OPEN_LEVEL = "Open level";
            constexpr const char* SAVE_LEVEL = "Save level";
        } // namespace Title

        namespace Message
        {
            constexpr const char* UNSAVED_CHANGES =
                "You have some unsaved changes. Do you want to save them "
                "before exiting?";
            constexpr const char* CANNOT_PLAY_LEVEL =
                "Cannot play level because no level is opened.";
        } // namespace Message

        namespace Body
        {
            constexpr const char* SELECT_LEVEL = "Select level";
            constexpr const char* LEVEL_NAME = "Level name";
        } // namespace Body
    }     // namespace Dialog

    namespace Editor
    {
        namespace ContextMenu
        {
            constexpr const char* MESH_MODE = "Mesh mode (M)";
            constexpr const char* ITEM_MODE = "Items mode (I)";
            constexpr const char* TRIG_MODE = "Trigger mode (T)";
            constexpr const char* RESIZE = "Resize level (R)";
            constexpr const char* SHRINK = "Shrink level to fit (S)";
            constexpr const char* LAYER_UP = "Move to upper layer (Ctrl + Up)";
            constexpr const char* LAYER_DOWN =
                "Move to lower layer (Ctrl + Down)";
        } // namespace ContextMenu
    }     // namespace Editor

    namespace Launcher
    {
        constexpr const char* CTX_MENU_NAME = "Play";

        namespace ContextMenu
        {
            constexpr const char* PLAYTEST = "Play level (F5)";
            constexpr const char* CONFIGURE_LAUNCH = "Configure launch options";
        } // namespace ContextMenu
    }     // namespace Launcher
} // namespace Strings