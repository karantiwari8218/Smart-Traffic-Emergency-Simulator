/**
 * @file Environment.h
 * @brief Represents the urban city environment including buildings, sidewalks, pedestrians, and greenery.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Multi-Lane Highway & Urban Environment Simulation
 * Contributor: Student 2 (Environment & Road Geometry)
 * Reviewers: Student 1, Student 3, Student 4
 */

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "Lane.h"

/**
 * @struct Building
 * @brief Represents a city building with facade styling, rooftop architecture, and window grids.
 */
struct Building {
    sf::FloatRect bounds;
    sf::Color facadeColor;
    sf::Color roofColor;
    std::string name;
    sf::Color nameColor;
    int style; // 0 = Hospital, 1 = Fire Dept, 2 = Police HQ, 3 = Modern Glass, 4 = Commercial Plaza, 5 = Apartments
};

/**
 * @struct StreetTree
 * @brief Represents an ornamental urban tree with leafy canopy and trunk.
 */
struct StreetTree {
    sf::Vector2f position;
    float radius;
};

/**
 * @struct Pedestrian
 * @brief Represents a walking citizen on the sidewalks.
 */
struct Pedestrian {
    sf::Vector2f position;
    TravelDirection direction;
    float speed;
    sf::Color shirtColor;
    float minCoord;
    float maxCoord;
};

/**
 * @class Environment
 * @brief Manages procedural rendering of city blocks, buildings, sidewalks, pedestrians, and park greenery.
 */
class Environment {
private:
    std::vector<Building> buildings;
    std::vector<StreetTree> trees;
    std::vector<Pedestrian> pedestrians;
    float beaconTimer;

    void initBuildings();
    void initGreenery();
    void initPedestrians();

public:
    Environment();

    void update(float deltaTime);
    void drawSidewalksAndGreenery(sf::RenderWindow& window) const;
    void drawBuildingsAndTrees(sf::RenderWindow& window, const sf::Font* font = nullptr) const;
    void drawPedestrians(sf::RenderWindow& window) const;
};

#endif // ENVIRONMENT_H

