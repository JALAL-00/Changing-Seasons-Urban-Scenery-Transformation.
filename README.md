# Changing Seasons: Urban Scenery Transformation

A comprehensive 2D computer graphics project developed for the "Computer Graphics" course (Spring 24-25) at American International University – Bangladesh. This application demonstrates fundamental graphics principles through the creation of multiple dynamic and interactive scenes using C++ and the OpenGL Utility Toolkit (GLUT).

![City Scene Collage](https://github.com/your-username/your-repo-name/blob/main/screenshots/city_scene_collage.png?raw=true)

## Project Overview

"Changing Seasons: Urban Scenery Transformation" is a state-driven application that showcases a mastery of 2D graphics programming. The core of the project is a main city scene that transitions through six unique environmental themes, complete with corresponding weather effects and lighting changes. The project also includes three other distinct scenes to demonstrate thematic versatility. All visual assets are generated procedurally at runtime using fundamental OpenGL drawing commands.

## Features

### The Urban City Scene

This is the primary and most feature-rich scene, demonstrating extensive state management and procedural generation.
- **Six Dynamic Themes:** The scene can be dynamically transformed through Evening, Night, Morning, Spring, Autumn, and Winter themes.
- **Unique Atmospherics:** Each theme features a unique color palette for the sky and water, distinct ambient lighting, and corresponding particle-based weather effects.
- **Procedural Weather:**
    - **Spring:** Gentle rainfall.
    - **Autumn:** Falling leaves with a fluttering motion.
    - **Winter:** A flurry of snow that accumulates on buildings and trees.
- **Interactive Animations:**
    - Cars moving along a multi-lane road.
    - A boat navigating a river in the foreground.
    - A user-controllable helicopter with adjustable speed and altitude.

### Secondary Scenes

To showcase versatility, the project includes three additional, fully-functional scenes:
- **Rocket Station Scene:** A complex industrial environment with a detailed rocket, a multi-level flyover with moving cars, and a day/night cycle. Features a multi-stage animated launch sequence for the rocket.
- **Futuristic City Scene:** A stylized sci-fi world with glowing silhouettes, a celestial sky, and multiple UFOs. The main UFO's flight path is directly controlled by the user's mouse clicks.
- **Sea Journey Scene:** A tranquil coastal landscape with a day/night cycle, a large ship traversing the water, drifting clouds, and windmills with smoothly rotating blades. An optional rain effect can be activated.

## Technical Details

- **Language:** C++
- **Graphics API:** OpenGL (Fixed-Function Pipeline)
- **Framework:** GLUT (OpenGL Utility Toolkit) for windowing, event handling, and the animation loop (`glutTimerFunc`).
- **Assets:** No external assets are used. All visuals (buildings, vehicles, characters, weather) are procedurally generated at runtime.
- **Architecture:** The application is built as a state-driven program within a single C++ source file. The system state is managed by enumerations (`ActiveScene`, `Season`) that control which drawing and logic functions are called.

## Controls

The user interface is entirely keyboard and mouse-driven.

| Key(s)        | Action                                           | Scene          |
|---------------|--------------------------------------------------|----------------|
| `C`, `S`, `R`, `F` | Switch between City, Sea, Rocket, and Future scenes | Global         |
| `1` - `6`     | Change season/theme (Evening, Night, etc.)       | City Scene     |
| `W` / `S`     | Move Helicopter/Boat Up/Down                     | City / Sea     |
| Arrow Keys    | Control Helicopter/Car speed                     | City / Rocket  |
| Mouse Clicks  | Control main UFO's flight path (Ascend/Descend)  | Futuristic City|
| `M`, `Q`, `O`, `A` | Control various animations and themes          | Sea Journey    |
| `N`, `B`      | Control themes                                   | Futuristic City|

## Getting Started

### Prerequisites

- A C++ compiler (GCC/G++ on macOS/Linux, MinGW on Windows).
- OpenGL and GLUT libraries installed.
    - **macOS:** Comes pre-installed with Xcode Command Line Tools.
    - **Windows:** Requires manual setup with a compiler like MinGW.

### Compilation on macOS

Open the Terminal, navigate to the project directory, and use the provided `Makefile`.
```bash
# Compile the project
make

# Run the executable
./CityScene
