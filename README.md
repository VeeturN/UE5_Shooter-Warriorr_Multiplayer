# UE5 Shooter & Warrior Multiplayer

This project, developed in **Unreal Engine 5**, effectively combines two different games within a single repository. It originated as a solo learning project to master the engine's fundamentals and was subsequently expanded into a functional multiplayer shooter.

## 📺 Project Showcase

[![UE5 Shooter & Warrior Multiplayer Showcase](https://img.youtube.com/vi/bvegHTV0H2o/0.jpg)](https://www.youtube.com/watch?v=bvegHTV0H2o) <br>
*Click the image above to watch the gameplay demonstration.*

## 🎮 About the Project

The project is divided into two primary modules:

### 1. Online Shooter (Multiplayer)
A functional multiplayer shooter built using UE5's native networking architecture.
* **Network Replication:** Full multiplayer support, including synchronization of player states, projectiles, and movement.
* **Shooting System (Raycast / Line Trace):** High-precision hit detection using line traces.
* **Advanced Animations:** * Procedural spine splitting (**Layered Blend per Bone**), allowing the character to move and shoot simultaneously with fluid transitions.
  * Dedicated animations for firing, recoil, and reloading.
* **Player Mechanics:** Sprinting, Aim Down Sights (ADS), and weapon pickup system.
* **Special Effects (VFX / SFX):** Integrated muzzle flashes, impact effects, and spatial audio.

### 2. Warrior (Single-Player)
A single-player module focused on melee combat mechanics and environmental interaction.
* **Melee Combat:** Fully animated sword combat system featuring fluid attack sequences and transitions.
* **Physics & Destruction (Chaos Physics):** Advanced environmental destruction, allowing objects to shatter into realistic fragments using the UE5 Chaos physics engine.
* **Artificial Intelligence (AI):** Simple enemy AI behavior trees and reaction systems.
* **Advanced Movement:** Jumping mechanics and dedicated movement sets for melee-oriented gameplay.

## 🛠️ Technologies & Mechanics
* **Engine:** Unreal Engine 5
* **Logic:** C++ and Blueprints
* **Networking:** Built-in Unreal Engine Replication System

## 🚀 Getting Started

1. Clone the repository:
   ```bash
   git clone [https://github.com/VeeturN/UE5_Shooter-Warrior_Multiplayer.git](https://github.com/VeeturN/UE5_Shooter-Warrior_Multiplayer.git)
