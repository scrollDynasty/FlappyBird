# 🎮 Flappy Bird Clone

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![C++](https://img.shields.io/badge/C++-17-00599C.svg?logo=c%2B%2B)
![SDL2](https://img.shields.io/badge/SDL-2.0-brightgreen.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A modern C++ implementation of the classic Flappy Bird game using SDL2. This project demonstrates game development concepts including physics, collision detection, and difficulty progression.

## 🚀 Features

- Smooth gameplay mechanics
- Progressive difficulty system
- Modern UI with rounded corners
- Score tracking
- Clean, modern C++ codebase

## 🛠️ Built With

- C++17
- SDL2 (Simple DirectMedia Layer)
- SDL2_image
- SDL2_ttf
- CMake 3.20+
- MinGW-w64 (for Windows)

## ⚙️ Prerequisites

Before you begin, ensure you have the following installed:
- CMake (3.20 or higher)
- A C++17 compatible compiler
- Git

## 📥 Installation

### Windows

1. Clone the repository:
```bash
git clone https://github.com/scrollDynasty/FlappyBird.git
cd FlappyBird
```

2. Download SDL2 development libraries:
    * SDL2
    * SDL2_image
    * SDL2_ttf
3. Extract the downloaded libraries to the project root:
```
FlappyBird/
├── SDL2/
├── SDL2_image/
└── SDL2_ttf/
```

4. Build the project:
```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Linux

1. Install dependencies:
```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev

# Fedora
sudo dnf install SDL2-devel SDL2_image-devel SDL2_ttf-devel

# Arch Linux
sudo pacman -S sdl2 sdl2_image sdl2_ttf
```

2. Clone and build:
```bash
git clone https://github.com/scrollDynasty/FlappyBird.git
cd FlappyBird
mkdir build && cd build
cmake ..
make
```

## 🎮 Running the Game

After building, you can find the executable in the `build/Debug` or `build/Release` directory:
```bash
# Windows
./build/Debug/FlappyBird.exe

# Linux
./build/FlappyBird
```

## 🕹️ Controls

* **Space**: Jump/Start game
* **Escape**: Quit game
* Press **Space** to restart after game over

## 📁 Project Structure

```
FlappyBird/
├── src/
│   ├── main.cpp
│   ├── Game.cpp
│   └── Game.h
├── assets/
│   ├── bird.png
│   ├── background.png
│   ├── pipe.png
│   ├── ground.png
│   └── font.ttf
├── CMakeLists.txt
└── README.md
```

## 🔧 Configuration

The game's configuration can be modified in `Game.h`:
```cpp
static const int SCREEN_WIDTH = 800;
static const int SCREEN_HEIGHT = 600;
static const int BIRD_WIDTH = 40;
static const int BIRD_HEIGHT = 40;
static constexpr float INITIAL_GRAVITY = 0.12f;
static constexpr float INITIAL_JUMP_FORCE = -4.0f;
```

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🎨 Assets

The game assets are either created specifically for this project or sourced from open-source resources. If you plan to use this project commercially, please ensure you have the appropriate rights to all assets.

## ✨ Acknowledgments

* Original Flappy Bird game by Dong Nguyen
* SDL2 development team
* The C++ community

Made with ❤️ by scrollDynasty

Last updated: 2025-01-23