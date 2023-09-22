# Readme

This project serves as a quickstart for development with dgm-lib so you can start prototyping games fast and easy. It comes with simple main menu, options for setting resolution, fullscreen and game audio, simple game loop and also a pause screen.

There are also some handy primitives already implemented such as event queue.

The environment also comes with prepared unit testing infrastructure as well as simple benchmarking sandbox.

## Dependencies

 * cmake 3.19 - Newer versions might have some issues with FetchContent
 * newest Visual Studio

All required dependencies (dgm-lib, SFML, TGUI, etc) are automatically downloaded and linked via CMake. If you want to bump any of them, update `cmake/dependencies.cmake` file. First couple of lines contains versions of those dependencies. Just bumping the version should be sufficient to update it.

## How to configure and build

Configuring is easy, if you have cmake in `%PATH%`:

```
mkdir vsbuild
cd vsbuild
cmake ..
```

Following commands will produce a `vsbuild` folder with `Example.sln` which you can open in Visual Studio and start developing. There is also a handy script `MakeRelease.bat` which performs full cleanup, configuration and build in release mode and prepares ready to ship zipfile with your game in `RELEASE` folder.

You can customize your configuration by adding following flags:

 * `-DCACHE_THIRD_PARTY=ON`: This will download dependencies to the root folder under `deps` instead of your build folder. This is useful if you often reconfigure without access to the internet.
 * `-DDISABLE_TESTING=ON`: If you don't want to have unit testing projects, use this option to disable their configuration. `ctest` command will also be disabled.
 * `-DENABLE_BENCHMARKS=OFF`: Use this to automatically include Google Benchmark into your project

## How to customize

Open file `cmake/settings.cmake` where you find bunch of variables that you can customize and will affect name of the output binary, of the solution file and release package name.

## How to bump dependencies

Open file `cmake/dependencies.cmake` and simply change the version numbers at the beginning of the file of libraries that you need to update.

## How to change version numbers

Edit file `cmake/version.cmake`

## Skipping main menu

If you run the binary with parameter `-s`, it will skip the main menu and jump right into the game. If you exit the game, you'll be returned into main menu.

## Starting development

As a first thing just simply build the entire project and launch the binary to see what it does.

Look for project `lib-game`. Start by adding some objects to `Scene` and `RenderContext` structs, update their builder functions and then render the object by updating logic in the `RenderingEngine` and make it do something with `GameRulesEngine` and `PhysicsEngine`.

Define new `events` and consume them in appropriate engines to communicate between them. Pust events into processing with `EventQueue::push`. The queue is automatically processed at the end of the frame.
