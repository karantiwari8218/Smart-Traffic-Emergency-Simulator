/**
 * @file main.cpp
 * @brief Continuous queue-based simulation loop with realistic buildings, sidewalks, and greenery.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Team Contributors:
 *   - Student 1: Vehicle Kinematics & Emergency Hierarchy (Ambulance, Fire, VIP, Police)
 *   - Student 2: 4-Lane Highway & Urban City Environment (Buildings, Sidewalks, Greenery)
 *   - Student 3: Multi-Intersection Intelligent Traffic Signals (EVP Green Wave & Arbitration)
 *   - Student 4: DSA Queue Spawner, Speed Variations & Headway Collision Avoidance
 */

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <memory>
#include <iostream>
#include <cmath>
#include "Lane.h"
#include "TrafficLight.h"
#include "Vehicle.h"
#include "Environment.h"

// Struct representing a blueprint for queued vehicle instantiation
struct VehicleBlueprint {
    std::string id;
    VehicleType type;
    float speed;
    int laneId;
    sf::Color color;
};

// Master Synchronous Intersection Phase Controller with Dynamic Demand Actuation
struct IntersectionController {
    int id;
    float phaseTimer = 0.0f;
    int phase = 0; // 0: Hwy Green (3.8s max), 1: Hwy Yellow (1.0s), 2: All-Red (0.6s), 3: Ave Green (3.8s max), 4: Ave Yellow (1.0s), 5: All-Red (0.6s)
    bool isPreempted = false;
    std::string preemptVehicleId = "";
    float preemptTimer = 0.0f;
    float preemptCooldown = 0.0f;
    int preemptTarget = 0; // 0 for Hwy, 1 for Ave

    void update(float dt, TrafficLight& hwyLight, TrafficLight& aveLight, const std::vector<Vehicle>& vehicles) {
        if (preemptCooldown > 0.0f) {
            preemptCooldown -= dt;
        }

        // Instantaneous preemption for approaching emergency vehicles
        if (!isPreempted && preemptCooldown <= 0.0f) {
            if (hwyLight.isPreempted()) {
                isPreempted = true;
                preemptVehicleId = hwyLight.getPreemptionVehicleId();
                preemptTimer = 4.2f;
                preemptTarget = 0;
                hwyLight.clearPreemption();
            } else if (aveLight.isPreempted()) {
                isPreempted = true;
                preemptVehicleId = aveLight.getPreemptionVehicleId();
                preemptTimer = 4.2f;
                preemptTarget = 1;
                aveLight.clearPreemption();
            }
        }

        if (isPreempted) {
            preemptTimer -= dt;
            if (preemptTarget == 0) {
                hwyLight.setState(1); // Highway Green
                aveLight.setState(0); // Avenue Red
            } else {
                hwyLight.setState(0); // Highway Red
                aveLight.setState(1); // Avenue Green
            }

            if (preemptTimer <= 0.0f) {
                isPreempted = false;
                preemptCooldown = 1.0f; // Minimal cooldown so next emergency unit is served immediately
                phase = (preemptTarget == 0) ? 2 : 5;
                phaseTimer = 0.0f;
            }
            return;
        }

        // Smart Demand Actuation: Count vehicles approaching each direction
        int hwyDemand = 0;
        int aveDemand = 0;
        for (const auto& v : vehicles) {
            sf::Vector2f p = v.getPosition();
            if (id == 1) { // West Intersection (X=400)
                if (v.getDirection() == TravelDirection::EASTBOUND && p.x > 80.0f && p.x < 325.0f) hwyDemand++;
                else if (v.getDirection() == TravelDirection::WESTBOUND && p.x > 475.0f && p.x < 720.0f) hwyDemand++;
                else if (v.getDirection() == TravelDirection::SOUTHBOUND && p.x < 550.0f && p.y > 40.0f && p.y < 255.0f) aveDemand++;
                else if (v.getDirection() == TravelDirection::NORTHBOUND && p.x < 550.0f && p.y > 445.0f && p.y < 670.0f) aveDemand++;
            } else { // East Intersection (X=900)
                if (v.getDirection() == TravelDirection::EASTBOUND && p.x > 580.0f && p.x < 825.0f) hwyDemand++;
                else if (v.getDirection() == TravelDirection::WESTBOUND && p.x > 975.0f && p.x < 1220.0f) hwyDemand++;
                else if (v.getDirection() == TravelDirection::SOUTHBOUND && p.x >= 550.0f && p.y > 40.0f && p.y < 255.0f) aveDemand++;
                else if (v.getDirection() == TravelDirection::NORTHBOUND && p.x >= 550.0f && p.y > 445.0f && p.y < 670.0f) aveDemand++;
            }
        }

        // Dynamic Signal Switching: If the active approach is empty and the other approach has waiting cars, switch immediately!
        phaseTimer += dt;
        if (phase == 0) { // Highway Green
            hwyLight.setState(1);
            aveLight.setState(0);
            if ((phaseTimer >= 1.0f && hwyDemand == 0 && aveDemand > 0) || phaseTimer >= 3.8f) {
                phase = 1;
                phaseTimer = 0.0f;
            }
        } else if (phase == 1) { // Highway Yellow
            hwyLight.setState(2);
            aveLight.setState(0);
            if (phaseTimer >= 1.0f) { phase = 2; phaseTimer = 0.0f; }
        } else if (phase == 2) { // All-Red Clearance
            hwyLight.setState(0);
            aveLight.setState(0);
            if (phaseTimer >= 0.6f) { phase = 3; phaseTimer = 0.0f; }
        } else if (phase == 3) { // Avenue Green
            hwyLight.setState(0);
            aveLight.setState(1);
            if ((phaseTimer >= 1.0f && aveDemand == 0 && hwyDemand > 0) || phaseTimer >= 3.8f) {
                phase = 4;
                phaseTimer = 0.0f;
            }
        } else if (phase == 4) { // Avenue Yellow
            hwyLight.setState(0);
            aveLight.setState(2);
            if (phaseTimer >= 1.0f) { phase = 5; phaseTimer = 0.0f; }
        } else if (phase == 5) { // All-Red Clearance
            hwyLight.setState(0);
            aveLight.setState(0);
            if (phaseTimer >= 0.6f) { phase = 0; phaseTimer = 0.0f; }
        }
    }
};

int main() {
    // 1. Initialize High-Definition RenderWindow (1320 x 740 resolution)
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(1320, 740)),
        "Realistic 4-Lane Highway Simulation with Buildings, Greenery & Dual Intersections (PBL Project)"
    );
    window.setFramerateLimit(60);

    // 2. Load TrueType Font for in-car text, building signage, and telemetry
    sf::Font font;
    bool fontLoaded = false;
    if (font.openFromFile("C:/Windows/Fonts/segoeui.ttf") ||
        font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
        font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        fontLoaded = true;
    }
    const sf::Font* fontPtr = fontLoaded ? &font : nullptr;

    // 3. Initialize City Environment (Buildings, Sidewalks, Pedestrians, Park Greenery)
    Environment environment;

    // 4. Construct 4-Lane Highway & Dual 4-Lane Cross Avenues (12 Lanes Total)
    std::vector<Lane> lanes;

    // Eastbound Highway Lanes (Traveling Right ->)
    // Lane 0: Eastbound Fast / Priority Lane (Adjacent to Lane 1)
    lanes.emplace_back(sf::Vector2f(20.0f, 275.0f), sf::Vector2f(1300.0f, 275.0f), 
                       TravelDirection::EASTBOUND, 40.0f, 0, "Highway EB Fast", true, 1);
    // Lane 1: Eastbound Cruising Lane (Adjacent to Lane 0)
    lanes.emplace_back(sf::Vector2f(20.0f, 315.0f), sf::Vector2f(1300.0f, 315.0f), 
                       TravelDirection::EASTBOUND, 40.0f, 1, "Highway EB Cruising", true, 0);

    // Westbound Highway Lanes (Traveling Left <-)
    // Lane 2: Westbound Fast / Priority Lane (Adjacent to Lane 3)
    lanes.emplace_back(sf::Vector2f(1300.0f, 365.0f), sf::Vector2f(20.0f, 365.0f), 
                       TravelDirection::WESTBOUND, 40.0f, 2, "Highway WB Fast", true, 3);
    // Lane 3: Westbound Cruising Lane (Adjacent to Lane 2)
    lanes.emplace_back(sf::Vector2f(1300.0f, 405.0f), sf::Vector2f(20.0f, 405.0f), 
                       TravelDirection::WESTBOUND, 40.0f, 3, "Highway WB Cruising", true, 2);

    // Cross-Street Avenue 1 (West Intersection, X=400: 4-Lane Avenue from X=330 to 470)
    // Lane 4: Ave 1 Southbound Cruising (Outer lane, X: 330 -> 365)
    lanes.emplace_back(sf::Vector2f(330.0f, 45.0f), sf::Vector2f(330.0f, 685.0f), 
                       TravelDirection::SOUTHBOUND, 35.0f, 4, "Ave 1 SB Cruising", false, 5);
    // Lane 5: Ave 1 Southbound Fast (Inner lane, X: 365 -> 400)
    lanes.emplace_back(sf::Vector2f(365.0f, 45.0f), sf::Vector2f(365.0f, 685.0f), 
                       TravelDirection::SOUTHBOUND, 35.0f, 5, "Ave 1 SB Fast", false, 4);
    // Lane 6: Ave 1 Northbound Fast (Inner lane, X: 400 -> 435)
    lanes.emplace_back(sf::Vector2f(400.0f, 685.0f), sf::Vector2f(400.0f, 45.0f), 
                       TravelDirection::NORTHBOUND, 35.0f, 6, "Ave 1 NB Fast", false, 7);
    // Lane 7: Ave 1 Northbound Cruising (Outer lane, X: 435 -> 470)
    lanes.emplace_back(sf::Vector2f(435.0f, 685.0f), sf::Vector2f(435.0f, 45.0f), 
                       TravelDirection::NORTHBOUND, 35.0f, 7, "Ave 1 NB Cruising", false, 6);

    // Cross-Street Avenue 2 (East Intersection, X=900: 4-Lane Avenue from X=830 to 970)
    // Lane 8: Ave 2 Southbound Cruising (Outer lane, X: 830 -> 865)
    lanes.emplace_back(sf::Vector2f(830.0f, 45.0f), sf::Vector2f(830.0f, 685.0f), 
                       TravelDirection::SOUTHBOUND, 35.0f, 8, "Ave 2 SB Cruising", false, 9);
    // Lane 9: Ave 2 Southbound Fast (Inner lane, X: 865 -> 900)
    lanes.emplace_back(sf::Vector2f(865.0f, 45.0f), sf::Vector2f(865.0f, 685.0f), 
                       TravelDirection::SOUTHBOUND, 35.0f, 9, "Ave 2 SB Fast", false, 8);
    // Lane 10: Ave 2 Northbound Fast (Inner lane, X: 900 -> 935)
    lanes.emplace_back(sf::Vector2f(900.0f, 685.0f), sf::Vector2f(900.0f, 45.0f), 
                       TravelDirection::NORTHBOUND, 35.0f, 10, "Ave 2 NB Fast", false, 11);
    // Lane 11: Ave 2 Northbound Cruising (Outer lane, X: 935 -> 970)
    lanes.emplace_back(sf::Vector2f(935.0f, 685.0f), sf::Vector2f(935.0f, 45.0f), 
                       TravelDirection::NORTHBOUND, 35.0f, 11, "Ave 2 NB Cruising", false, 10);

    // 5. Intelligent Traffic Signals with Fair Two-Phase Cycling
    // Intersection 1 (West)
    TrafficLight lightWestHighway(sf::Vector2f(295.0f, 185.0f), "INT-1 Hwy (West)", 1, 1, false);
    TrafficLight lightWestStreet(sf::Vector2f(495.0f, 185.0f), "INT-1 Ave (West)", 0, 1, true);

    // Intersection 2 (East)
    TrafficLight lightEastHighway(sf::Vector2f(795.0f, 185.0f), "INT-2 Hwy (East)", 1, 2, false);
    TrafficLight lightEastStreet(sf::Vector2f(995.0f, 185.0f), "INT-2 Ave (East)", 0, 2, true);

    std::vector<TrafficLight> trafficLights = {
        lightWestHighway, lightWestStreet, lightEastHighway, lightEastStreet
    };

    // Master Intersection Controllers (Intersection 2 starts offset in phase 3 for natural traffic dynamics)
    IntersectionController intCtrl1{1, 0.0f, 0};
    IntersectionController intCtrl2{2, 0.0f, 3};

    // 6. Active Vehicle Fleet
    std::vector<Vehicle> vehicles;

    // 7. Autonomous FIFO Vehicle Queue System (DSA queue)
    std::vector<std::queue<VehicleBlueprint>> laneQueues(lanes.size());
    int carSequenceCounter = 1;

    // Function to replenish queues with calibrated speeds and priority hierarchy across all 12 lanes
    auto replenishQueue = [&](int laneIdx) {
        if (laneIdx == 0) { // EB Fast Lane: Fast civilian cars + Ambulance / VIP
            laneQueues[laneIdx].push({"AMB-" + std::to_string(carSequenceCounter++), VehicleType::AMBULANCE, 128.0f, laneIdx, sf::Color(245, 245, 250)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 76.0f, laneIdx, sf::Color(35, 110, 210)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 70.0f, laneIdx, sf::Color(190, 45, 45)});
            laneQueues[laneIdx].push({"VIP-" + std::to_string(carSequenceCounter++), VehicleType::VIP, 112.0f, laneIdx, sf::Color(20, 20, 22)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 78.0f, laneIdx, sf::Color(40, 140, 190)});
        } else if (laneIdx == 1) { // EB Cruising Lane
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 68.0f, laneIdx, sf::Color(175, 180, 190)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 74.0f, laneIdx, sf::Color(30, 140, 75)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 76.0f, laneIdx, sf::Color(215, 125, 30)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 70.0f, laneIdx, sf::Color(140, 60, 140)});
        } else if (laneIdx == 2) { // WB Fast Lane
            laneQueues[laneIdx].push({"FIRE-" + std::to_string(carSequenceCounter++), VehicleType::FIRE_TRUCK, 118.0f, laneIdx, sf::Color(215, 25, 25)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 76.0f, laneIdx, sf::Color(45, 120, 215)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 68.0f, laneIdx, sf::Color(220, 225, 235)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 74.0f, laneIdx, sf::Color(180, 50, 50)});
        } else if (laneIdx == 3) { // WB Cruising Lane
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 68.0f, laneIdx, sf::Color(165, 75, 125)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 72.0f, laneIdx, sf::Color(80, 85, 170)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 76.0f, laneIdx, sf::Color(40, 140, 80)});
        } else if (laneIdx == 4) { // Ave 1 SB Cruising Lane
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 68.0f, laneIdx, sf::Color(55, 125, 200)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 72.0f, laneIdx, sf::Color(185, 190, 200)});
        } else if (laneIdx == 5) { // Ave 1 SB Fast Lane
            laneQueues[laneIdx].push({"POL-" + std::to_string(carSequenceCounter++), VehicleType::POLICE, 108.0f, laneIdx, sf::Color(20, 20, 25)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 74.0f, laneIdx, sf::Color(210, 60, 60)});
        } else if (laneIdx == 6) { // Ave 1 NB Fast Lane
            laneQueues[laneIdx].push({"AMB-" + std::to_string(carSequenceCounter++), VehicleType::AMBULANCE, 128.0f, laneIdx, sf::Color(245, 245, 250)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 76.0f, laneIdx, sf::Color(30, 135, 70)});
        } else if (laneIdx == 7) { // Ave 1 NB Cruising Lane
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 68.0f, laneIdx, sf::Color(140, 70, 180)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 72.0f, laneIdx, sf::Color(200, 100, 40)});
        } else if (laneIdx == 8) { // Ave 2 SB Cruising Lane
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 70.0f, laneIdx, sf::Color(215, 115, 30)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 74.0f, laneIdx, sf::Color(150, 60, 150)});
        } else if (laneIdx == 9) { // Ave 2 SB Fast Lane
            laneQueues[laneIdx].push({"VIP-" + std::to_string(carSequenceCounter++), VehicleType::VIP, 112.0f, laneIdx, sf::Color(20, 20, 22)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 76.0f, laneIdx, sf::Color(40, 115, 210)});
        } else if (laneIdx == 10) { // Ave 2 NB Fast Lane
            laneQueues[laneIdx].push({"FIRE-" + std::to_string(carSequenceCounter++), VehicleType::FIRE_TRUCK, 118.0f, laneIdx, sf::Color(215, 25, 25)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 74.0f, laneIdx, sf::Color(50, 160, 90)});
        } else if (laneIdx == 11) { // Ave 2 NB Cruising Lane
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 68.0f, laneIdx, sf::Color(190, 80, 80)});
            laneQueues[laneIdx].push({"C" + std::to_string(carSequenceCounter++), VehicleType::REGULAR_CAR, 72.0f, laneIdx, sf::Color(80, 120, 200)});
        }
    };

    // Pre-fill all queues initially
    for (size_t i = 0; i < lanes.size(); ++i) {
        replenishQueue(static_cast<int>(i));
    }

    // Helper to compute starting position for newly spawned vehicles
    auto getSpawnPosition = [&](const Lane& lane, VehicleType type) -> sf::Vector2f {
        float w = (type == VehicleType::AMBULANCE || type == VehicleType::FIRE_TRUCK) ? 28.0f : 24.0f;

        if (lane.getDirection() == TravelDirection::EASTBOUND) {
            return sf::Vector2f(-75.0f, lane.getStartPosition().y + (lane.getWidth() - w) / 2.0f);
        } else if (lane.getDirection() == TravelDirection::WESTBOUND) {
            return sf::Vector2f(1380.0f, lane.getStartPosition().y + (lane.getWidth() - w) / 2.0f);
        } else if (lane.getDirection() == TravelDirection::SOUTHBOUND) {
            return sf::Vector2f(lane.getStartPosition().x + (lane.getWidth() - w) / 2.0f, -75.0f);
        } else { // NORTHBOUND
            return sf::Vector2f(lane.getStartPosition().x + (lane.getWidth() - w) / 2.0f, 760.0f);
        }
    };

    // Initial vehicle injection with SAFE STAGGERING (16 vehicles across lanes for active, lively traffic)
    std::vector<int> initialLaneSelection = {0, 0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    int staggerCounter = 0;
    for (int laneIdx : initialLaneSelection) {
        if (!laneQueues[laneIdx].empty()) {
            auto bp = laneQueues[laneIdx].front();
            laneQueues[laneIdx].pop();
            sf::Vector2f pos = getSpawnPosition(lanes[laneIdx], bp.type);

            // Stagger safely so vehicles start in motion well behind stop lines
            if (lanes[laneIdx].getDirection() == TravelDirection::EASTBOUND) {
                pos.x = (staggerCounter % 2 == 0) ? 60.0f : 170.0f;
            } else if (lanes[laneIdx].getDirection() == TravelDirection::WESTBOUND) {
                pos.x = (staggerCounter % 2 == 0) ? 1220.0f : 1100.0f;
            } else if (lanes[laneIdx].getDirection() == TravelDirection::SOUTHBOUND) {
                pos.y = (staggerCounter % 2 == 0) ? 75.0f : 145.0f;
            } else if (lanes[laneIdx].getDirection() == TravelDirection::NORTHBOUND) {
                pos.y = (staggerCounter % 2 == 0) ? 630.0f : 560.0f;
            }
            staggerCounter++;

            vehicles.emplace_back(bp.id, bp.type, bp.speed, pos, &lanes[laneIdx], lanes[laneIdx].getDirection(), bp.color);
        }
    }

    // 8. Simulation Clock & Spawner Timers
    sf::Clock clock;
    float spawnTimer = 0.0f;
    static size_t nextSpawnLane = 0;
    const size_t MAX_SIMULATION_VEHICLES = 32; // Balanced, vibrant traffic (non-chaotic, never empty!)

    std::cout << "==========================================================================" << std::endl;
    std::cout << " Enhanced Realistic Traffic Simulation 2.0 Started!                       " << std::endl;
    std::cout << " Features: 4-Lane Highway & Dual 4-Lane Avenues (12 Multi-Lanes Total)    " << std::endl;
    std::cout << " Capacity: 32 Vehicles Active | Smart Actuated Signals (No Empty Waiting) " << std::endl;
    std::cout << " Priority: Visible Overtaking & Emergency Speed Dominance                 " << std::endl;
    std::cout << " Hierarchy: AMBULANCE (128) > FIRE (118) > VIP (112) > POLICE (108)       " << std::endl;
    std::cout << "==========================================================================" << std::endl;

    // 9. Continuous Autonomous Simulation Loop
    while (window.isOpen()) {
        // Event processing
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        float deltaTime = clock.restart().asSeconds();
        if (deltaTime > 0.08f) deltaTime = 0.08f; // Clamp delta spikes

        // ---------------------------------------------------------------------
        // Step A: Update Urban Environment (Pedestrians, Trees)
        // ---------------------------------------------------------------------
        environment.update(deltaTime);

        // ---------------------------------------------------------------------
        // Step B: Master Synchronous Smart Demand-Actuated Intersection Scheduling
        // ---------------------------------------------------------------------
        intCtrl1.update(deltaTime, trafficLights[0], trafficLights[1], vehicles);
        intCtrl2.update(deltaTime, trafficLights[2], trafficLights[3], vehicles);

        // ---------------------------------------------------------------------
        // Step C: Update Vehicles (Motion, Yielding, Overtaking, Collision Guard)
        // ---------------------------------------------------------------------
        for (auto& vehicle : vehicles) {
            vehicle.move(deltaTime, vehicles, trafficLights, lanes);
        }

        // ---------------------------------------------------------------------
        // Step D: Remove Off-Screen Vehicles
        // ---------------------------------------------------------------------
        vehicles.erase(
            std::remove_if(vehicles.begin(), vehicles.end(), [](const Vehicle& v) {
                return v.isOffScreen();
            }),
            vehicles.end()
        );

        // ---------------------------------------------------------------------
        // Step E: Autonomous FIFO Spawner (Smart 32-Vehicle Cap & Fair Round-Robin)
        // ---------------------------------------------------------------------
        spawnTimer += deltaTime;
        if (spawnTimer >= 1.2f && vehicles.size() < MAX_SIMULATION_VEHICLES) {
            spawnTimer = 0.0f;

            // Attempt to spawn one vehicle across lanes in fair round-robin order
            for (size_t attempt = 0; attempt < lanes.size(); ++attempt) {
                size_t laneIdx = nextSpawnLane;
                nextSpawnLane = (nextSpawnLane + 1) % lanes.size();

                if (laneQueues[laneIdx].empty()) {
                    replenishQueue(static_cast<int>(laneIdx));
                }

                if (!laneQueues[laneIdx].empty()) {
                    const auto& bp = laneQueues[laneIdx].front();
                    const auto& lane = lanes[laneIdx];
                    sf::Vector2f candidateSpawn = getSpawnPosition(lane, bp.type);

                    // Check entrance headway clearance (at least 135px safe headway)
                    bool entranceClear = true;
                    for (const auto& v : vehicles) {
                        if (v.getDirection() != lane.getDirection()) continue;

                        float lateralDiff = lane.isHorizontal() 
                                            ? std::abs(v.getPosition().y - candidateSpawn.y)
                                            : std::abs(v.getPosition().x - candidateSpawn.x);
                        if (lateralDiff > 20.0f) continue;

                        float dist = std::hypot(v.getPosition().x - candidateSpawn.x, v.getPosition().y - candidateSpawn.y);
                        if (dist < 135.0f) {
                            entranceClear = false;
                            break;
                        }
                    }

                    if (entranceClear) {
                        laneQueues[laneIdx].pop();
                        vehicles.emplace_back(bp.id, bp.type, bp.speed, candidateSpawn, 
                                              &lanes[laneIdx], lane.getDirection(), bp.color);
                        break; // Spawned 1 vehicle, maintaining vibrant, non-chaotic traffic!
                    }
                }
            }
        }

        // =====================================================================
        // RENDERING PIPELINE
        // =====================================================================
        window.clear(sf::Color(44, 76, 52)); // Green park lawn base

        // 1. Draw Sidewalks and Green Lawn Ground
        environment.drawSidewalksAndGreenery(window);

        // 2. Draw Road Asphalt Surfaces
        for (const auto& lane : lanes) {
            lane.draw(window);
        }

        // 3. Galvanized Steel Outer Highway Guardrails
        auto drawGuardrail = [&](float startY) {
            sf::RectangleShape rail(sf::Vector2f(1280.0f, 4.0f));
            rail.setPosition(sf::Vector2f(20.0f, startY));
            rail.setFillColor(sf::Color(170, 175, 185));
            window.draw(rail);

            for (float gx = 25.0f; gx < 1300.0f; gx += 40.0f) {
                if ((gx >= 325.0f && gx <= 475.0f) || (gx >= 825.0f && gx <= 975.0f)) continue; // Avenue Openings
                sf::RectangleShape post(sf::Vector2f(3.0f, 6.0f));
                post.setPosition(sf::Vector2f(gx, startY - 1.0f));
                post.setFillColor(sf::Color(90, 95, 105));
                window.draw(post);
            }
        };
        drawGuardrail(255.0f); // Top Highway edge
        drawGuardrail(441.0f); // Bottom Highway edge

        // 4. Highway 3D Concrete Median Barrier (Discontinuous: Cut at intersections!)
        auto drawMedianSegment = [&](float startX, float segWidth) {
            // Yellow safety curb base
            sf::RectangleShape curb(sf::Vector2f(segWidth, 10.0f));
            curb.setPosition(sf::Vector2f(startX, 355.0f));
            curb.setFillColor(sf::Color(225, 190, 30));
            window.draw(curb);

            // Concrete Jersey barrier body
            sf::RectangleShape barrier(sf::Vector2f(segWidth, 6.0f));
            barrier.setPosition(sf::Vector2f(startX, 357.0f));
            barrier.setFillColor(sf::Color(105, 110, 115));
            window.draw(barrier);

            // Top bevel highlight
            sf::RectangleShape bevel(sf::Vector2f(segWidth, 1.5f));
            bevel.setPosition(sf::Vector2f(startX, 357.0f));
            bevel.setFillColor(sf::Color(155, 160, 168));
            window.draw(bevel);
        };

        drawMedianSegment(20.0f, 300.0f);   // Section 1: West -> Int 1 (X: 20 -> 320)
        drawMedianSegment(480.0f, 340.0f);  // Section 2: Int 1 -> Int 2 (X: 480 -> 820)
        drawMedianSegment(980.0f, 320.0f);  // Section 3: Int 2 -> East (X: 980 -> 1300)

        // Crash Cushion Impact Attenuator Barrels at Median Openings
        auto drawCrashCushion = [&](float cx) {
            for (int b = 0; b < 3; ++b) {
                float bx = (cx < 600.0f) ? (cx - b * 7.0f) : (cx + b * 7.0f);
                sf::CircleShape barrel(3.5f);
                barrel.setPosition(sf::Vector2f(bx - 3.5f, 356.5f));
                barrel.setFillColor(sf::Color(245, 210, 25));
                barrel.setOutlineThickness(1.0f);
                barrel.setOutlineColor(sf::Color(30, 30, 30));
                window.draw(barrel);
            }
        };
        drawCrashCushion(317.0f); // West opening of Int 1
        drawCrashCushion(483.0f); // East opening of Int 1
        drawCrashCushion(817.0f); // West opening of Int 2
        drawCrashCushion(983.0f); // East opening of Int 2

        // 5. Anti-Gridlock Yellow Box Junctions (140px wide for 4-lane avenues)
        auto drawBoxJunction = [&](float minX, float maxX) {
            sf::RectangleShape boxBorder(sf::Vector2f(maxX - minX, 190.0f));
            boxBorder.setPosition(sf::Vector2f(minX, 255.0f));
            boxBorder.setFillColor(sf::Color::Transparent);
            boxBorder.setOutlineThickness(1.5f);
            boxBorder.setOutlineColor(sf::Color(235, 195, 30, 180));
            window.draw(boxBorder);

            for (float d = -190.0f; d < (maxX - minX); d += 22.0f) {
                sf::Vertex line[2];
                line[0].position = sf::Vector2f(minX + std::max(0.0f, d), 255.0f + std::max(0.0f, -d));
                line[0].color = sf::Color(235, 195, 30, 95);
                line[1].position = sf::Vector2f(minX + std::min(maxX - minX, d + 190.0f), 255.0f + std::min(190.0f, 190.0f + d));
                line[1].color = sf::Color(235, 195, 30, 95);
                window.draw(line, 2, sf::PrimitiveType::Lines);
            }
        };
        drawBoxJunction(330.0f, 470.0f); // Int 1 Box Junction (140px wide)
        drawBoxJunction(830.0f, 970.0f); // Int 2 Box Junction (140px wide)

        // 6. Dashed Lane Lines & Markings
        for (const auto& lane : lanes) {
            lane.drawMarkings(window);
        }

        // Avenue Double-Yellow Center Dividers Outside Intersection
        auto drawAvenueCenterDivider = [&](float cx) {
            sf::RectangleShape y1(sf::Vector2f(2.0f, 210.0f));
            y1.setFillColor(sf::Color(240, 200, 30));
            y1.setPosition(sf::Vector2f(cx - 2.5f, 45.0f));
            window.draw(y1);
            y1.setPosition(sf::Vector2f(cx - 2.5f, 475.0f));
            window.draw(y1);

            sf::RectangleShape y2(sf::Vector2f(2.0f, 210.0f));
            y2.setFillColor(sf::Color(240, 200, 30));
            y2.setPosition(sf::Vector2f(cx + 1.5f, 45.0f));
            window.draw(y2);
            y2.setPosition(sf::Vector2f(cx + 1.5f, 475.0f));
            window.draw(y2);
        };
        drawAvenueCenterDivider(400.0f); // Ave 1 Center Divider
        drawAvenueCenterDivider(900.0f); // Ave 2 Center Divider

        // 7. Pedestrian Zebra Crosswalks (Spanning 140px on Avenues)
        auto drawCrosswalkH = [&](float startX, float startY) {
            for (float y = startY; y < startY + 80.0f; y += 12.0f) {
                sf::RectangleShape stripe(sf::Vector2f(18.0f, 6.0f));
                stripe.setPosition(sf::Vector2f(startX, y));
                stripe.setFillColor(sf::Color(235, 235, 240));
                window.draw(stripe);
            }
        };
        auto drawCrosswalkV = [&](float startX, float startY) {
            for (float x = startX; x < startX + 140.0f; x += 12.0f) {
                sf::RectangleShape stripe(sf::Vector2f(6.0f, 18.0f));
                stripe.setPosition(sf::Vector2f(x, startY));
                stripe.setFillColor(sf::Color(235, 235, 240));
                window.draw(stripe);
            }
        };

        // Int 1 Crosswalks
        drawCrosswalkH(315.0f, 275.0f);
        drawCrosswalkH(315.0f, 365.0f);
        drawCrosswalkH(485.0f, 275.0f);
        drawCrosswalkH(485.0f, 365.0f);
        drawCrosswalkV(330.0f, 240.0f);
        drawCrosswalkV(330.0f, 445.0f);

        // Int 2 Crosswalks
        drawCrosswalkH(815.0f, 275.0f);
        drawCrosswalkH(815.0f, 365.0f);
        drawCrosswalkH(985.0f, 275.0f);
        drawCrosswalkH(985.0f, 365.0f);
        drawCrosswalkV(830.0f, 240.0f);
        drawCrosswalkV(830.0f, 445.0f);

        // 8. Solid White Stop Lines & Painted Road Stencils
        auto drawStopLineH = [&](float x, float y, float h) {
            sf::RectangleShape sl(sf::Vector2f(4.0f, h));
            sl.setPosition(sf::Vector2f(x, y));
            sl.setFillColor(sf::Color(250, 250, 255));
            window.draw(sl);
        };
        auto drawStopLineV = [&](float x, float y, float w) {
            sf::RectangleShape sl(sf::Vector2f(w, 4.0f));
            sl.setPosition(sf::Vector2f(x, y));
            sl.setFillColor(sf::Color(250, 250, 255));
            window.draw(sl);
        };

        drawStopLineH(310.0f, 275.0f, 80.0f);
        drawStopLineH(490.0f, 365.0f, 80.0f);
        drawStopLineV(330.0f, 240.0f, 70.0f);
        drawStopLineV(400.0f, 460.0f, 70.0f);

        drawStopLineH(810.0f, 275.0f, 80.0f);
        drawStopLineH(990.0f, 365.0f, 80.0f);
        drawStopLineV(830.0f, 240.0f, 70.0f);
        drawStopLineV(900.0f, 460.0f, 70.0f);

        // Thermoplastic "STOP" Stencils Painted on Asphalt
        if (fontPtr != nullptr) {
            auto drawStopStencil = [&](float sx, float sy, float rot) {
                sf::Text stText(*fontPtr, "STOP", 10);
                stText.setStyle(sf::Text::Bold);
                stText.setFillColor(sf::Color(230, 235, 240, 180));
                stText.setPosition(sf::Vector2f(sx, sy));
                stText.setRotation(sf::degrees(rot));
                window.draw(stText);
            };

            // Highway EB stop stencils (West of stop line)
            drawStopStencil(280.0f, 287.0f, 0.0f);
            drawStopStencil(280.0f, 327.0f, 0.0f);
            drawStopStencil(780.0f, 287.0f, 0.0f);
            drawStopStencil(780.0f, 327.0f, 0.0f);

            // Highway WB stop stencils (East of stop line)
            drawStopStencil(510.0f, 377.0f, 0.0f);
            drawStopStencil(510.0f, 417.0f, 0.0f);
            drawStopStencil(1010.0f, 377.0f, 0.0f);
            drawStopStencil(1010.0f, 417.0f, 0.0f);

            // Avenue SB stop stencils
            drawStopStencil(350.0f, 215.0f, 0.0f);
            drawStopStencil(850.0f, 215.0f, 0.0f);

            // Avenue NB stop stencils
            drawStopStencil(420.0f, 475.0f, 0.0f);
            drawStopStencil(920.0f, 475.0f, 0.0f);
        }

        // 7. Draw City Buildings and Trees
        environment.drawBuildingsAndTrees(window, fontPtr);

        // 8. Draw Walking Pedestrians
        environment.drawPedestrians(window);

        // 9. Draw Traffic Light Towers
        for (const auto& light : trafficLights) {
            light.draw(window, fontPtr);
        }

        // 10. Draw All Vehicles (with in-car decals and labels)
        for (const auto& vehicle : vehicles) {
            vehicle.draw(window, fontPtr);
        }

        // 11. Telemetry HUD Overlay
        sf::RectangleShape topBanner(sf::Vector2f(1320.0f, 42.0f));
        topBanner.setPosition(sf::Vector2f(0.0f, 0.0f));
        topBanner.setFillColor(sf::Color(20, 24, 30, 245));
        window.draw(topBanner);

        sf::RectangleShape bottomPanel(sf::Vector2f(1320.0f, 42.0f));
        bottomPanel.setPosition(sf::Vector2f(0.0f, 698.0f));
        bottomPanel.setFillColor(sf::Color(20, 24, 30, 245));
        window.draw(bottomPanel);

        if (fontPtr != nullptr) {
            sf::Text title(*fontPtr, "REALISTIC TRAFFIC SIMULATION 2.0 | 4-LANE HIGHWAY & DUAL 4-LANE AVENUES", 13);
            title.setStyle(sf::Text::Bold);
            title.setFillColor(sf::Color::White);
            title.setPosition(sf::Vector2f(20.0f, 6.0f));
            window.draw(title);

            sf::Text subTitle(*fontPtr, "12 Active Lanes | Max 20 Fleet | Yield-to-Emergency Speed Boost | Priority: AMBULANCE > FIRE > VIP > POLICE", 10);
            subTitle.setFillColor(sf::Color(160, 185, 210));
            subTitle.setPosition(sf::Vector2f(20.0f, 24.0f));
            window.draw(subTitle);

            // Active vehicles telemetry
            sf::Text stats(*fontPtr, "FLEET: " + std::to_string(vehicles.size()) + " / 20 MAX | ALL-LANE FAIR INJECTION", 11);
            stats.setStyle(sf::Text::Bold);
            stats.setFillColor(sf::Color(50, 230, 120));
            stats.setPosition(sf::Vector2f(830.0f, 12.0f));
            window.draw(stats);

            // Bottom telemetry
            sf::Text bottomText(*fontPtr, "CITY BLOCKS: HOSPITAL | POLICE HQ | FIRE DEPT | TECH TOWERS | FINANCIAL DISTRICT | RETAIL ARCADE", 10);
            bottomText.setFillColor(sf::Color(225, 230, 240));
            bottomText.setPosition(sf::Vector2f(20.0f, 708.0f));
            window.draw(bottomText);

            sf::Text flowText(*fontPtr, "STATUS: FAIR MULTI-LANE DUAL-INTERSECTION SERVICE ACTIVE", 11);
            flowText.setStyle(sf::Text::Bold);
            flowText.setFillColor(sf::Color(245, 205, 75));
            flowText.setPosition(sf::Vector2f(810.0f, 708.0f));
            window.draw(flowText);
        }

        // Present completed frame
        window.display();
    }

    std::cout << "Traffic Simulation terminated successfully." << std::endl;
    return 0;
}
