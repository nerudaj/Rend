# Readme

This project is my love letter to boomer shooters like Wolfenstein 3D and Doom. The 2.5D enigne is hand-written in C++ from scratch, using SFML for graphics, sounds and networking, TGUI for UI, dgm-lib for abstractions over SFML and dgm-fsm-lib for building AI FSM. The game is supposed to have competitive modes for 2-4 players.

You can read about the development process in my [weekly devlogs](https://medium.com/@nerudaj/list/devlogs-rend-97b960bf3cbe).

## Dependencies

 * cmake 3.26.1 (newest version are not guaranteed to work)
 * newest Visual Studio (v17)

All required C++ dependencies (dgm-lib, SFML, TGUI, etc) are automatically downloaded and linked via CMake.

## How to configure and build

Configuring is easy, if you have cmake in `%PATH%`:

```
mkdir vsbuild
cd vsbuild
cmake ..
cmake --build . --config Release
```

The commands will produce a `vsbuild` folder with a `Compiled` folder inside, where you can find all compiled binaries. If you're launching from this folder, you will have to use the `-r` command-line parameter otherwise the app will crash on startup. There will also be a `Rend.sln` solution file in the `vsbuild` folder.

## Command-line parameters

The game has the following parameters:

 * `-s` / `--skip-menu` - Jumps directly into the game
 * `-r <path>` / `--resource-dir <path>` - Path is relative from the current working dir to inside of the `resources` folder
 * `-m <name>` / `--map <name>` - Name, including .lvd extension of a file inside `resources/levels` folder
 * `-d <file>` / `--demofile <file>` - Path to file that should be used for reading/writing demo inputs
 * `-p` / `--play-demo` - Specifies that the game inputs should be read from the demo file rather from the controller. Use this to replay games from demo.
 * `-l` / `--limit` - Set point limit for the game 
