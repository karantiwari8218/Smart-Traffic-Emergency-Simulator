/**
 * @file Lane.cpp
 * @brief Implementation of the Lane class.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Multi-Lane Highway & Dual-Intersection Environment
 * Contributor: Student 2 (Environment & Road Geometry)
 * Reviewers: Student 1, Student 3, Student 4
 */

#include "Lane.h"
#include <cmath>
#include <algorithm>

Lane::Lane(sf::Vector2f start, sf::Vector2f end, TravelDirection direction,
           float width, int id, std::string name, bool highway, int adjacentId)
    : startPosition(start), endPosition(end), travelDirection(direction),
      roadWidth(width), laneId(id), laneName(name), isHighway(highway),
      adjacentLaneId(adjacentId) {
}

void Lane::draw(sf::RenderWindow& window) const {
    if (isHorizontal()) {
        float length = std::abs(endPosition.x - startPosition.x);
        float minX = std::min(startPosition.x, endPosition.x);

        sf::RectangleShape asphalt(sf::Vector2f(length, roadWidth));
        asphalt.setPosition(sf::Vector2f(minX, startPosition.y));
        asphalt.setFillColor(sf::Color(55, 55, 60)); // Asphalt dark gray
        window.draw(asphalt);
    } else {
        float length = std::abs(endPosition.y - startPosition.y);
        float minY = std::min(startPosition.y, endPosition.y);

        sf::RectangleShape asphalt(sf::Vector2f(roadWidth, length));
        asphalt.setPosition(sf::Vector2f(startPosition.x, minY));
        asphalt.setFillColor(sf::Color(55, 55, 60)); // Asphalt dark gray
        window.draw(asphalt);
    }
}

void Lane::drawMarkings(sf::RenderWindow& window) const {
    if (isHorizontal()) {
        // Only fast lanes (Lane 0 and Lane 2) have an internal dashed lane divider with cruising lanes
        if (laneId == 1 || laneId == 3) return;

        float minX = std::min(startPosition.x, endPosition.x);
        float maxX = std::max(startPosition.x, endPosition.x);

        const float dashLength = 24.0f;
        const float dashGap = 20.0f;
        const float dashThickness = 2.5f;
        float lineY = startPosition.y + roadWidth - dashThickness / 2.0f;

        for (float x = minX + 15.0f; x < maxX - 15.0f; x += (dashLength + dashGap)) {
            // Skip inside the dual 4-lane intersections: Int 1 [325, 475], Int 2 [825, 975]
            if ((x >= 325.0f && x <= 475.0f) || (x >= 825.0f && x <= 975.0f)) {
                continue;
            }

            sf::RectangleShape dash(sf::Vector2f(dashLength, dashThickness));
            dash.setPosition(sf::Vector2f(x, lineY));
            dash.setFillColor(sf::Color(230, 230, 235));
            window.draw(dash);
        }
    } else {
        // In 4-lane avenues, only lanes 4, 6, 8, 10 have internal white dashed dividers between lanes of the same direction
        // Lanes 5 & 9 border the double-yellow center divider; lanes 7 & 11 border the outer sidewalk curb
        if (laneId == 5 || laneId == 7 || laneId == 9 || laneId == 11) return;

        float minY = std::min(startPosition.y, endPosition.y);
        float maxY = std::max(startPosition.y, endPosition.y);

        const float dashLength = 24.0f;
        const float dashGap = 20.0f;
        const float dashThickness = 2.5f;
        float lineX = startPosition.x + roadWidth - dashThickness / 2.0f;

        for (float y = minY + 15.0f; y < maxY - 15.0f; y += (dashLength + dashGap)) {
            // Skip inside the highway crossing region [255, 455]
            if (y >= 255.0f && y <= 455.0f) {
                continue;
            }

            sf::RectangleShape dash(sf::Vector2f(dashThickness, dashLength));
            dash.setPosition(sf::Vector2f(lineX, y));
            dash.setFillColor(sf::Color(230, 230, 235));
            window.draw(dash);
        }
    }
}

sf::Vector2f Lane::getStartPosition() const {
    return startPosition;
}

sf::Vector2f Lane::getEndPosition() const {
    return endPosition;
}

TravelDirection Lane::getDirection() const {
    return travelDirection;
}

float Lane::getWidth() const {
    return roadWidth;
}

int Lane::getId() const {
    return laneId;
}

std::string Lane::getName() const {
    return laneName;
}

bool Lane::isHorizontal() const {
    return (travelDirection == TravelDirection::EASTBOUND || 
            travelDirection == TravelDirection::WESTBOUND);
}

int Lane::getAdjacentLaneId() const {
    return adjacentLaneId;
}

float Lane::getCenterLateral() const {
    if (isHorizontal()) {
        return startPosition.y + roadWidth / 2.0f;
    } else {
        return startPosition.x + roadWidth / 2.0f;
    }
}
