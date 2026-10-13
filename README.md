# Traffic Simulation 2.0 — Realistic Urban Corridor with 4-Lane Highway, Dual Intersections & Emergency Priority

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![Build Tool](https://img.shields.io/badge/Build-CMake%203.16+-orange.svg)](https://cmake.org/)
[![GUI Framework](https://img.shields.io/badge/Graphics-SFML%203.0.1%2F3.0.2-green.svg)](https://www.sfml-dev.org/)
[![DSA Concept](https://img.shields.io/badge/DSA-std%3A%3Aqueue%20%7C%20Priority%20Scheduling-purple.svg)]()
[![PBL Group](https://img.shields.io/badge/PBL-Team%20of%204%20Students-success.svg)]()

A Project-Based Learning (PBL) simulation modeling traffic dynamics across a realistic **urban corridor** with a **4-lane divided highway**, **two synchronized intersections**, **12 architecturally styled city buildings**, **continuous pedestrian sidewalks with walking citizens**, **lush park landscaping and median trees**, **varied civilian vehicle speeds with emergency yielding**, and **fair bidirectional two-phase traffic signal control**. Built in modern C++ (C++17) using Object-Oriented Programming (OOP), Data Structures & Algorithms (DSA), and hardware-accelerated 2D graphics through SFML 3.

---

## Project Team & Responsibilities (Group of 4 Students)

| Student Role | Assigned Module | Core Contributions & Responsibilities |
| :--- | :--- | :--- |
| **Student 1**<br>*(Team Lead & Vehicle Kinematics)* | `include/Vehicle.h`<br>`src/Vehicle.cpp` | • Designed the `VehicleType` hierarchy: **Ambulance (P4) > Fire Truck (P3) > VIP (P2) > Police (P1) > Civilian (P0)**.<br>• Implemented dynamic **highway lane-changing (overtaking)** and civilian **siren yielding**.<br>• Programmed in-car roof text labeling (`AMBULANCE`, `FIRE`, `VIP`, `POLICE`, `C1`..`C4`), tires, windshield glass, and dynamic brake lights. |
| **Student 2**<br>*(Urban Environment & Geometry)* | `include/Environment.h`<br>`src/Environment.cpp`<br>`include/Lane.h`<br>`src/Lane.cpp` | • Modeled 12 realistic city buildings (Hospital, Police HQ, Fire Dept, Glass Towers, Bank, Commercial Plaza).<br>• Designed continuous pedestrian sidewalks with curb stones and animated walking pedestrians.<br>• Built lush greenery, shaded street trees with layered canopies, and highway median barrier landscaping. |
| **Student 3**<br>*(Intelligent Signals & Fair Scheduling)* | `include/TrafficLight.h`<br>`src/TrafficLight.cpp` | • Implemented fair two-phase signal scheduling preventing vertical lane starvation.<br>• Designed the EVP Emergency Preemption cooldown system guaranteeing minimum cross-street green time.<br>• Developed conflict-free intersection arbitration preventing crossing collisions. |
| **Student 4**<br>*(DSA Queue Spawner & DevOps)* | `main.cpp`<br>`CMakeLists.txt`<br>`README.md` | • Designed the autonomous **FIFO vehicle queue spawner** (`std::queue<VehicleBlueprint>`).<br>• Distributed varied civilian speeds ($75\text{--}115\text{ px/s}$) to ensure realistic highway flow.<br>• Staggered initial vehicle positions to eliminate startup collisions. |

---

## Table of Contents
1. [Urban Environment & Realism](#1-urban-environment--realism)
2. [Fair Signal Scheduling (Solving Vertical Lane Starvation)](#2-fair-signal-scheduling)
3. [Varied Civilian Speeds & Emergency Overtaking](#3-varied-civilian-speeds--emergency-overtaking)
4. [Startup Safety & Intersection Collision Elimination](#4-startup-safety--intersection-collision-elimination)
5. [Emergency Priority Hierarchy & Livery](#5-emergency-priority-hierarchy--livery)
6. [Autonomous FIFO Queue Spawner (DSA)](#6-autonomous-fifo-queue-spawner-dsa)
7. [System Architecture & File Structure](#7-system-architecture--file-structure)
8. [Build and Execution Guide](#8-build-and-execution-guide)
9. [Viva Voce Q&A](#9-viva-voce-qa)
10. [Git Setup Guide](#10-git-setup-guide)

---

## 1. Urban Environment & Realism

The simulation models an active urban metropolitan corridor ($1320 \times 740$):
- **12 Realistic City Buildings:**
  - *Metro General Hospital:* White & sky-blue facade with rooftop Medical Red Cross emblem and medical clinic windows.
  - *Fire Rescue Station #12:* Deep crimson brick facade with arched truck bay doors and service equipment.
  - *Police Precinct HQ:* Deep slate blue stone facade with gold insignia and law court pillars.
  - *Modern Glass Skyscraper (Cyber Tower):* Navy glass tower with illuminated window grid patterns.
  - *Central Bank & Commerce:* Granite gray stone facade with neoclassical architectural lines.
  - *Grand Hotel, Retail Arcades & Apartments:* Textured brick walls, storefront awnings, and rooftop AC chillers.
- **Pedestrian Sidewalks & Walking Citizens:**
  - $16\text{px}$ wide light granite pavements with darker stone curbs bordering all roads.
  - Walking citizens (pedestrians with colored shirts) continuously strolling along pavements and pausing at zebra crossings.
- **Park Landscaping & Highway Median Trees:**
  - Highway median barrier bordered by yellow safety curbs and lush green lawns planted with round ornamental leafy trees every $65\text{px}$.
  - Shaded park trees with dual-tone leafy foliage and natural wood trunks throughout city lawns.

---

## 2. Fair Signal Scheduling (Solving Vertical Lane Starvation)

### The Problem
Previously, frequent highway emergency preemption calls continuously reset the signal to Green, locking the vertical avenues on Red permanently.

### The Solution
1. **Symmetric Two-Phase Signal Controller:**
   - Phase A: Highway Green (5.0s) $\rightarrow$ Highway Yellow (2.0s) $\rightarrow$ Avenue Red.
   - Phase B: Highway Red $\rightarrow$ Avenue Green (5.0s) $\rightarrow$ Avenue Yellow (2.0s).
2. **EVP Preemption Cooldown ($8.0\text{s}$):**
   - Once an emergency vehicle completes its preemption wave (3.5s), the signal enters a mandatory $8\text{s}$ cooldown.
   - During cooldown, the perpendicular cross-street receives its guaranteed uninterrupted Green phase, allowing vertical traffic to move continuously and smoothly!

---

## 3. Varied Civilian Speeds & Emergency Overtaking

### Diverse Speed Distribution
Civilian vehicles spawn with distinct, realistic speed capabilities:
- **Slow Cruisers (Trucks / Compacts):** $78\text{--}88\text{ px/s}$.
- **Standard Commuters (Sedans):** $90\text{--}102\text{ px/s}$.
- **Fast Express (Sport Sedans):** $105\text{--}115\text{ px/s}$.

### Siren Yielding & Multi-Lane Overtaking
- When an emergency vehicle (Ambulance, Fire, VIP, Police) approaches behind a civilian car within $130\text{px}$:
  - The civilian car detects the siren and emergency strobes.
  - **Yielding:** If traveling in the Fast Lane (Lane 0 or Lane 2), the civilian car checks the adjacent Cruising Lane and moves over. If unable to move over, it decelerates to $55\text{ px/s}$ to let the emergency vehicle pass.
  - **Overtaking:** The emergency vehicle checks the adjacent lane and smoothly glides over at $160\text{ px/s}$ to bypass the slower traffic effortlessly.

---

## 4. Startup Safety & Intersection Collision Elimination

### The Problem
At startup, initial vehicles spawned close to the intersection at high speed while cross-street signals were still initializing, resulting in collisions. Furthermore, adjacent parallel lanes were falsely triggering collision stops.

### The Solution
1. **Safe Staggered Spawning:** Initial vehicles on cross-streets start $160\text{px}$ behind their stop lines with cross-street signals set to Red. They safely brake and wait for Green.
2. **True Intersection Conflict Check:** Vehicles approaching an intersection on Green only yield if another vehicle is **physically inside the intersection box**. Vehicles waiting politely behind the stop line at a Red light do not cause false deadlocks!
3. **Lane-Aware 2D Collision Guard:** Vehicles in adjacent parallel lanes or traveling in opposite directions separated by the median are recognized as safe and are never falsely stopped.

---

## 5. Emergency Priority Hierarchy & Livery

$$\text{AMBULANCE (4)} > \text{FIRE TRUCK (3)} > \text{VIP CONVOY (2)} > \text{POLICE (1)} > \text{CIVILIAN (0)}$$

- **Ambulance:** Text `"AMBULANCE"` printed on white roof, medical Red Cross ($+$), alternating Red/Blue LED strobes.
- **Fire Truck:** Text `"FIRE"` on red roof, silver equipment ladders, pulsing Red/Amber beacons.
- **VIP Convoy:** Text `"VIP"` in gold on obsidian roof, luxury gold pinstripes, blue/white grille strobes.
- **Police Car:** Text `"POLICE"` on white roof, classic black & white cruiser livery, alternating Red/Blue roof lightbar.
- **Civilian Cars:** Short identifier codes (e.g., `C1`, `C2`, `C3`) printed neatly on the car roof.

---

## 6. Autonomous FIFO Queue Spawner (DSA)

- Uses `std::queue<VehicleBlueprint>` from the C++ Standard Template Library.
- Each of the 8 lanes has an independent FIFO queue.
- Verified headway clearance ($140\text{px}$) before injecting each vehicle.
- Vehicles exiting map boundaries are removed via `std::remove_if`, maintaining constant performance and zero memory leakage.

---

## 7. System Architecture & File Structure

```
TrafficSimulation/
├── CMakeLists.txt              # Cross-platform build configuration & SFML linking
├── README.md                   # Complete PBL documentation & viva study guide
├── .gitignore                  # Git ignore rules for build artifacts & IDE temp files
├── main.cpp                    # Autonomous queue spawner & two-phase signal manager
├── include/                    # Header declarations
│   ├── Environment.h           # Buildings, sidewalks, pedestrians, and park greenery
│   ├── Lane.h                  # Road layout, adjacent lane links, centerlines
│   ├── TrafficLight.h          # Signal FSM, EVP preemption, cooldown timers
│   └── Vehicle.h               # Kinematics, priority hierarchy, lane changing
└── src/                        # Source implementations
    ├── Environment.cpp         # Building facades, window grids, trees, pedestrian walking
    ├── Lane.cpp                # Asphalt drawing, dashed markings, zebra crosswalks
    ├── TrafficLight.cpp        # Signal countdown timers & fair duty cycles
    └── Vehicle.cpp             # Overtaking kinematics, siren yielding, in-car text
```

---

## 8. Build and Execution Guide

### Run Directly (Pre-compiled Binary)
The executable has been built and verified:
```powershell
# From the project directory:
.\build\TrafficSimulation.exe
```

### Build from Source with CMake
```powershell
cd /d "C:\C++ program\TrafficSimulation"
cmake -S . -B build
cmake --build build --config Release
.\build\Release\TrafficSimulation.exe
```

---

## 9. Viva Voce Q&A

1. **How was the vertical lane starvation issue solved?**
   *Answer:* By implementing an 8-second EVP preemption cooldown in `TrafficLight`. After an emergency vehicle completes its preemption wave, new preemptions are deferred, guaranteeing the vertical avenue receives an uninterrupted 5-second Green phase.
2. **Why were vehicles stopping completely in earlier iterations?**
   *Answer:* The universal 2D collision check was expanding bounding boxes without verifying lateral lane separation, causing cars in adjacent parallel lanes to falsely detect a collision. We added lane-aware filtering so cars in adjacent parallel lanes proceed without interference.
3. **How do emergency vehicles overtake if non-priority cars move at different speeds?**
   *Answer:* Civilian cars are assigned varied speeds ($75\text{--}115\text{ px/s}$). When an emergency vehicle approaches behind a civilian car, the civilian car yields (moves to the cruising lane or decelerates), allowing the emergency vehicle to smoothly overtake via the adjacent lane.
4. **How are city buildings, sidewalks, and greenery rendered?**
   *Answer:* The `Environment` class procedurally renders 12 architecturally styled buildings with window grids and rooftops, paved sidewalks with curbs, walking pedestrians, and landscaped trees with layered canopies and trunks.

---

## 10. Git Setup Guide

```powershell
cd /d "C:\C++ program\TrafficSimulation"
git init
git add .
git commit -m "feat: complete realistic urban traffic simulation with buildings, greenery, and fair signal cycling"
git branch -M main
git remote add origin https://github.com/<YOUR-USERNAME>/<YOUR-REPO-NAME>.git
git push -u origin main
```
