# Matt's World
This is a top down adventure game.

# Controls
Use WASD or the Arrow Keys to move

## Build for Multiple Platforms
You can build for multiple platforms using CMake, you will need the following installed to link for your platform of choice 
* SDL2
* SDL2-image
* SDL2-mixer
* SDL2-ttf
* libvorbis
### Windows
* You can install dependencies using [vcpkg](https://github.com/microsoft/vcpkg)
    * for SDL_Mixer use ./vcpkg.exe install sdl2-mixer[libvorbis]
    * you may need to remove SDL_Mixer first, sdl2-mixer[dynamic-load] causes issues with cmake
    * lately vcpkg has been unreliable, if all else fails you can copy the SDL2 dlls into the build directory
* Open Project in Visual Studio (cmake support installed)
* Build
* Run SimpleGame.exe

### Linux / MacOS
* install dependencies using package manager i.e. apt, pacman, etc. for MacOS this would be [brew](https://brew.sh/)
```zsh
mkdir build
cd build
cmake ..
make
./SimpleGame
```
### Web
```zsh
mkdir build
cd build
emcmake cmake ..
emmake make
emrun TinksWorld.html
```