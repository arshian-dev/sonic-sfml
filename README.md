# Sonic SFML

A 2D Sonic-inspired platformer game built using **SFML 2.6** and **g++ 13.1**.  
Switch between Sonic, Tails, and Knuckles — each with unique abilities — and explore dynamic levels with enemies, gravity, and a boss fight.

---

## Features

- Character Switching: Play as Sonic, Tails, or Knuckles
- Unique Abilities: Flight, speed dash, wall climb, etc.
- Side-scrolling Levels: Designed with gravity and collision systems
- Enemies: Including the Crab enemy and more in progress
- Boss Fight: Strategic combat mechanics (WIP)
- Game Systems: Save manager, leaderboard manager (planned)
- Audio & Animations: Immersive retro-style feel
- Clean UI: Menus, HUDs, and transitions

---

## Getting Started

### Prerequisites
- C++ compiler: **g++ 13.1**
- Library: **SFML 2.6**
- Any IDE or terminal for compilation

### Compilation (Windows)

Run the following command in the project root:

```compile command
g++ main.cpp -Isrc -ISFML/include -LSFML/lib -o game.exe -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
