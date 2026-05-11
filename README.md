# Super Potoo World

An old-school 2D platformer developed in C using SDL2. This project was completed during my first year of engineering school.

## Features

- **Classic Gameplay**: Play as Potoo in a demanding platforming world.
- **Varied Enemies**: Face magical nuts, demons, and the fearsome MSN (Mega Super Nut).
- **Multiple Worlds**: Explore the Mountains, Lake, Flying Ships, and Volcano.
- **Custom Physics Engine**: Uses `PlatformerEngine` for smooth movement and collision handling. This engine was not developed by me.
- **Animation System**: Dynamic rendering powered by `RenderingEngine`.

## Requirements

The project uses **vcpkg** for dependency management on Windows and system packages on Linux.

### Linux (Debian/Ubuntu)
Install the SDL2 and OpenMP development libraries:
```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev libomp-dev
```

### Windows
1. Install [CMake](https://cmake.org/download/).
2. Install [vcpkg](https://github.com/microsoft/vcpkg).
3. The project will automatically fetch its dependencies (SDL2, etc.) during CMake configuration.

## Build

### Linux
```bash
mkdir build && cd build
cmake ..
make
```

### Windows (Visual Studio)
```powershell
mkdir build; cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[PATH_TO_VCPKG]/scripts/buildsystems/vcpkg.cmake
# Then open the generated .sln file in Visual Studio and build the project.
```

## Continuous Integration
This project uses **GitHub Actions** to verify builds on Linux and Windows on every commit.

## Project Structure
- `src/`: Source code (`.c`).
- `include/`: Header files (`.h`).
- `Assets/`: Resources (images, levels, fonts).
- `external/`: Third-party libraries and engines.

## Authors
- Ledyxoo
