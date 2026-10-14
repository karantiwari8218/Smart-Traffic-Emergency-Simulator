/**
 * @file Lane.h
 * @brief Represents a traffic lane with geometry, markings, and adjacent lane links.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Multi-Lane Highway & Dual-Intersection Environment
 * Contributor: Student 2 (Environment & Road Geometry)
 * Reviewers: Student 1, Student 3, Student 4
 */

#ifndef LANE_H
#define LANE_H

#include <SFML/Graphics.hpp>
#include <string>

/**
 * @enum TravelDirection
 * @brief Axis and travel orientation of vehicles along a lane.
 */
enum class TravelDirection {
    EASTBOUND,  ///< Moving Right (+X)
    WESTBOUND,  ///< Moving Left (-X)
    SOUTHBOUND, ///< Moving Down (+Y)
    NORTHBOUND  ///< Moving Up (-Y)
};

/**
 * @class Lane
 * @brief Models road geometry, centerlines, lane-changing links, and asphalt rendering.
 */
class Lane {
private:
    sf::Vector2f startPosition;      ///< Starting entrance coordinate
    sf::Vector2f endPosition;        ///< Ending exit coordinate
    TravelDirection travelDirection; ///< Direction of flow
    float roadWidth;                 ///< Width of lane in pixels
    int laneId;                      ///< Unique integer ID
    std::string laneName;            ///< Designation (e.g., "Highway EB Fast")
    bool isHighway;                  ///< True if highway lane
    int adjacentLaneId;              ///< ID of adjacent lane in same direction (-1 if none)

public:
    Lane(sf::Vector2f start, sf::Vector2f end, TravelDirection direction,
         float width = 40.0f, int id = 0, std::string name = "Lane", 
         bool highway = true, int adjacentId = -1);

    void draw(sf::RenderWindow& window) const;
    void drawMarkings(sf::RenderWindow& window) const;

    sf::Vector2f getStartPosition() const;
    sf::Vector2f getEndPosition() const;
    TravelDirection getDirection() const;
    float getWidth() const;
    int getId() const;
    std::string getName() const;
    bool isHorizontal() const;
    int getAdjacentLaneId() const;
    float getCenterLateral() const;
};

#endif // LANE_H
