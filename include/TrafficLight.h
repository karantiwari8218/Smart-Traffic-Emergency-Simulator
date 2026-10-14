/**
 * @file TrafficLight.h
 * @brief Represents an intelligent traffic signal with fair phase duty-cycles and EVP preemption cooldowns.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Intelligent Traffic Control & Emergency Preemption
 * Contributor: Student 3 (Traffic Control & State Machine)
 * Reviewers: Student 1, Student 2, Student 4
 */

#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @class TrafficLight
 * @brief Manages signal state machine, fair phase scheduling, and Emergency Preemption (EVP) with cooldowns.
 */
class TrafficLight {
private:
    int state;                      ///< 0 = Red, 1 = Green, 2 = Yellow
    float timer;                    ///< Elapsed time in current phase
    sf::Vector2f position;          ///< Screen coordinate of signal housing
    std::string intersectionName;   ///< Name of intersection
    bool preemptionActive;          ///< True if an emergency vehicle has preempted
    std::string preemptionVehicleId;///< Identifier of preemption holder
    float preemptionTimer;          ///< Time remaining for preemption
    float preemptionCooldown;       ///< Cooldown timer enforcing fair service for cross-streets
    int intersectionId;             ///< 1 for Intersection 1 (West), 2 for Intersection 2 (East)
    bool isAvenue;                  ///< True if this signal controls the vertical avenue

    // Standard timing durations
    static constexpr float RED_DURATION    = 5.0f;
    static constexpr float GREEN_DURATION  = 5.0f;
    static constexpr float YELLOW_DURATION = 2.0f;

public:
    TrafficLight(sf::Vector2f pos, std::string name = "Intersection", int initialState = 0, 
                 int intId = 1, bool isAve = false);

    void update(float deltaTime);
    void draw(sf::RenderWindow& window, const sf::Font* font = nullptr) const;

    bool triggerEmergencyPreemption(const std::string& vehicleId, float duration = 3.5f);
    void clearPreemption();

    bool isRed() const;
    bool isGreen() const;
    bool isYellow() const;
    bool isPreempted() const;
    bool canPreempt() const;

    void setState(int newState);
    int getState() const;
    float getTimer() const;
    void resetTimer();
    sf::Vector2f getPosition() const;
    std::string getIntersectionName() const;
    std::string getPreemptionVehicleId() const;
    int getIntersectionId() const;
    bool getIsAvenue() const;
};

#endif // TRAFFICLIGHT_H
