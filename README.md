# 🌌 Space Shooter: Procedural Game Engine Implementation
**A high-performance 2D arcade shooter built from scratch with C++ and SFML.**

This project was developed during my 1st semester at FAST-NUCES. It demonstrates a strong foundation in core programming logic, real-time game loops, and manual state management without the use of high-level Object-Oriented abstractions.

---

## 🚀 Technical Highlights
* **Trigonometric Physics:** Implemented projectile trajectories using `std::cos` and `std::sin` to calculate bullet vectors based on spaceship rotation.
* **Manual Memory Management:** Utilized fixed-size arrays and custom `struct` architectures (`GameState`, `Bullet`) to manage game objects efficiently.
* **Tuned Collision System:** Developed a custom collision detection algorithm with manual hitbox adjustments (shrinking bounds) for precise and fair gameplay.
* **Procedural Difficulty Scaling:** Engineered a leveling system that dynamically increases enemy spawn rates and movement speeds as the player's score progresses.

---

## 🎮 Features
* **Multi-Level Gameplay:** Transition from Level 0 to Level 4 with increasing difficulty.
* **Live System:** Visual heart-based life tracking with 3 lives per session.
* **Audio Engine:** Real-time triggers for shooting, explosions, and game-over states.
* **Fully Resettable State:** Clean "Restart" logic allowing players to play again without relaunching the application.

---

## 🛠️ How to Play (Download & Run)
This repository includes the full **Visual Studio Solution** so you can run it immediately.

1. **Clone/Download:** Download this repository as a ZIP file and extract it.
2. **Open Project:** Double-click the `Shooter.sln` file to open it in **Visual Studio 2022 or 2026**.
3. **Setup SFML:** Ensure you have the **SFML 2.6.x** libraries linked in your project properties (or use the provided DLLs in the root folder or use zipped folder).
4. **Compile & Run:** Press **F5** (Local Windows Debugger) to start the game!

**Controls:**
* **Arrow Keys:** Rotate and move the spaceship.
* **Spacebar:** Fire bullets.
* **R:** To restart.
* **Esc:** Exit game.

---

## 💻 Tech Stack
* **Language:** C++17
* **Library:** SFML (Simple and Fast Multimedia Library)
* **Environment:** Visual Studio 2026
