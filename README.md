# Changing Seasons: Urban Scenery Transformation

A comprehensive 2D computer graphics project developed for the "Computer Graphics" course (Spring 24-25) at American International University – Bangladesh. This application demonstrates fundamental graphics principles through the creation of multiple dynamic and interactive scenes using C++ and the OpenGL Utility Toolkit (GLUT).

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

## Getting Started

### The Urban City Scene
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/c4c99f12-ca3f-4113-b2a3-1a652c0f6f73" />
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/a45cfec7-5c04-4a5c-9900-c9d8b7f7d06c" />
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/f22431ba-56cc-4a62-9a61-5463bd4ff7dd" />
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/f3dba83a-2faf-4ab9-8ed4-1b413157baf7" />
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/de6e35fd-f675-4660-846c-349102847ab6" />
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/12582fcf-9dff-447a-b8a5-71688f3643ca" />

### Rocket Station Scene
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/b9454d3b-22da-41b5-879a-acfe60535aeb" />

<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/f3dea898-cb81-4e5f-90f8-7d13143b5a2c" />

### Futuristic City Scene
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/98b0ee63-4c09-4779-9a5a-6aad1e325a92" />

<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/29b17515-552d-442a-a13a-04f82d97d1bc" />

### Sea Journey Scene
<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/ae9cc5d4-fd5d-4e90-9f49-2b5ffd0cd825" />

<img width="1440" height="900" alt="Image" src="https://github.com/user-attachments/assets/bcbe5f58-6bb4-43d8-9597-739215f2faba" />


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
