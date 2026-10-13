/**
 * @file Vehicle.h
 * @brief Autonomous vehicle agent with realistic body designs, lane changing, and priority arbitration.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Vehicle Kinematics & Emergency Priority Hierarchy
 * Contributors: Student 1 (Vehicle Kinematics) & Student 4 (Headway Algorithms & Priority Hierarchy)
 * Reviewers: Student 2, Student 3
 */

#ifndef VEHICLE_H
#define VEHICLE_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Lane.h"
#include "TrafficLight.h"

/**
 * @enum VehicleType
 * @brief Vehicle classification according to emergency priority ranking.
 */
enum class VehicleType {
    REGULAR_CAR, ///< Civilian passenger car (Priority 0)
    POLICE,      ///< Police patrol cruiser (Priority 1)
    VIP,         ///< Diplomatic / Executive motorcade (Priority 2)
    FIRE_TRUCK,  ///< Fire rescue truck (Priority 3)
    AMBULANCE    ///< High-priority medical emergency vehicle (Priority 4 - Highest)
};

/**
 * @class Vehicle
 * @brief Realistic vehicle model with time-based kinematics, dynamic lane changing, and intersection collision avoidance.
 */
class Vehicle {
private:
    std::string uniqueId;           ///< Identifier (e.g. "AMB-911", "FIRE-101", "VIP-001", "POL-204", "C1")
    VehicleType vehicleType;        ///< Vehicle category
    int priorityLevel;              ///< Priority: 4 (Ambulance) > 3 (Fire) > 2 (VIP) > 1 (Police) > 0 (Civilian)
    float baseSpeed;                ///< Nominal cruising speed (pixels/sec)
    float currentSpeed;             ///< Active translation speed
    sf::Vector2f position;          ///< Top-left bounding box screen position
    Lane* lane;                     ///< Currently occupied lane
    TravelDirection direction;      ///< Current direction of motion
    sf::Color bodyColor;            ///< Chassis paint color
    float strobeTimer;              ///< Flash timer for emergency beacons
    bool isBraking;                 ///< True when decelerating/stopped (triggers bright red brake lamps)

    // Dynamic Lane Changing parameters (Highway & 4-Lane Avenue overtaking)
    bool isChangingLane;            ///< True while lateral transition is in progress
    float targetLateral;            ///< Target lateral (Y or X) coordinate for lane shift
    Lane* targetLane;               ///< Target lane pointer
    float laneChangeCooldown;       ///< Cooldown timer preventing rapid oscillating lane changes

    // Side-by-side yielding acceleration boost
    bool isClearingWay;             ///< True when speeding up to break side-by-side block for emergency vehicle behind
    float clearWayTimer;            ///< Active duration countdown for clear-way acceleration boost


public:
    Vehicle(std::string id, VehicleType type, float speed, sf::Vector2f initialPos,
            Lane* currentLane, TravelDirection dir, sf::Color color = sf::Color(40, 110, 180));

    /**
     * @brief Advances vehicle position, performs lane changing, yields at intersections, and prevents collisions.
     * @param deltaTime Elapsed frame time in seconds
     * @param allVehicles Vector of all active vehicles for collision and headway analysis
     * @param trafficLights Vector of active traffic signals
     * @param allLanes Vector of all roadway lanes (for lane changing)
     */
    void move(float deltaTime, const std::vector<Vehicle>& allVehicles,
              std::vector<TrafficLight>& trafficLights, const std::vector<Lane>& allLanes);

    /**
     * @brief Renders the realistic car body, wheels, windshield, lamps, and on-body text/emblem.
     * @param window Target SFML RenderWindow
     * @param font Font pointer for text written directly on the car
     */
    void draw(sf::RenderWindow& window, const sf::Font* font = nullptr) const;

    // Getters & Utility
    std::string getId() const;
    VehicleType getType() const;
    int getPriority() const;
    float getSpeed() const;
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;
    TravelDirection getDirection() const;
    bool isEmergency() const;
    bool isOffScreen() const;
    bool getIsChangingLane() const;
    std::string getTypeString() const;
};

#endif // VEHICLE_H
