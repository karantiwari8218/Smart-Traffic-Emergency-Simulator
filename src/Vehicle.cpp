/**
 * @file Vehicle.cpp
 * @brief Implementation of the Vehicle class with realistic on-body designs, lane changing, and priority arbitration.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Vehicle Kinematics & Emergency Priority Hierarchy
 * Contributors: Student 1 (Vehicle Kinematics) & Student 4 (Headway Algorithms & Priority Hierarchy)
 * Reviewers: Student 2, Student 3
 */

#include "Vehicle.h"
#include <cmath>
#include <limits>
#include <algorithm>

Vehicle::Vehicle(std::string id, VehicleType type, float speed, sf::Vector2f initialPos,
                 Lane* currentLane, TravelDirection dir, sf::Color color)
    : uniqueId(id), vehicleType(type), baseSpeed(speed), currentSpeed(speed),
      position(initialPos), lane(currentLane), direction(dir), bodyColor(color),
      strobeTimer(0.0f), isBraking(false), isChangingLane(false),
      targetLateral(0.0f), targetLane(nullptr), laneChangeCooldown(0.0f),
      isClearingWay(false), clearWayTimer(0.0f) {
    
    // Explicit priority ranking: Ambulance (4) > Fire (3) > VIP (2) > Police (1) > Civilian (0)
    switch (vehicleType) {
        case VehicleType::AMBULANCE:
            priorityLevel = 4; // Highest emergency life-saving priority
            break;
        case VehicleType::FIRE_TRUCK:
            priorityLevel = 3; // Critical fire & disaster rescue
            break;
        case VehicleType::VIP:
            priorityLevel = 2; // Executive / diplomatic motorcade
            break;
        case VehicleType::POLICE:
            priorityLevel = 1; // Law enforcement patrol / pursuit
            break;
        case VehicleType::REGULAR_CAR:
        default:
            priorityLevel = 0; // Standard civilian traffic
            break;
    }
}

void Vehicle::move(float deltaTime, const std::vector<Vehicle>& allVehicles,
                   std::vector<TrafficLight>& trafficLights, const std::vector<Lane>& allLanes) {
    strobeTimer += deltaTime;
    currentSpeed = baseSpeed;
    isBraking = false;

    if (laneChangeCooldown > 0.0f) {
        laneChangeCooldown -= deltaTime;
    }

    // Handle clear-way speed boost timer (Allows civilian car to surge ahead and break side-by-side block)
    if (clearWayTimer > 0.0f) {
        clearWayTimer -= deltaTime;
        if (clearWayTimer <= 0.0f) {
            isClearingWay = false;
        } else {
            currentSpeed = baseSpeed * 1.50f; // Sprints ahead to create opening for emergency vehicle!
        }
    }

    bool isHorizontal = (direction == TravelDirection::EASTBOUND || direction == TravelDirection::WESTBOUND);
    bool isPositive = (direction == TravelDirection::EASTBOUND || direction == TravelDirection::SOUTHBOUND);
    float currentCoord = isHorizontal ? position.x : position.y;

    float carLength = (vehicleType == VehicleType::AMBULANCE || vehicleType == VehicleType::FIRE_TRUCK) ? 66.0f : 58.0f;
    float carWidth  = (vehicleType == VehicleType::AMBULANCE || vehicleType == VehicleType::FIRE_TRUCK) ? 30.0f : 26.0f;

    // =========================================================================
    // 1. Dynamic Lane Changing & Civilian Yielding (Highway & 4-Lane Avenues)
    // =========================================================================
    if (isChangingLane) {
        if (isHorizontal) {
            float diff = targetLateral - position.y;
            if (std::abs(diff) <= 2.5f) {
                position.y = targetLateral;
                lane = targetLane;
                isChangingLane = false;
                laneChangeCooldown = 2.0f; // Prevent rapid oscillating lane switches
            } else {
                position.y += (diff > 0.0f ? 1.0f : -1.0f) * 110.0f * deltaTime;
            }
        } else {
            float diff = targetLateral - position.x;
            if (std::abs(diff) <= 2.5f) {
                position.x = targetLateral;
                lane = targetLane;
                isChangingLane = false;
                laneChangeCooldown = 2.0f; // Prevent rapid oscillating lane switches
            } else {
                position.x += (diff > 0.0f ? 1.0f : -1.0f) * 110.0f * deltaTime;
            }
        }
    } else if (lane != nullptr && lane->getAdjacentLaneId() >= 0) {
        int adjId = lane->getAdjacentLaneId();

        // A) Civilian Vehicle Yielding & Clear-The-Way Acceleration
        // Do not yield if already stationary in an intersection/signal queue
        if (priorityLevel == 0 && laneChangeCooldown <= 0.0f && currentSpeed > 15.0f) {
            bool emergencyBehind = false;
            for (const auto& other : allVehicles) {
                if (other.getId() == uniqueId || other.getDirection() != direction) continue;
                if (other.getPriority() < 1) continue; // Only emergency vehicles

                float latDiff = isHorizontal ? std::abs(other.getPosition().y - position.y)
                                             : std::abs(other.getPosition().x - position.x);
                if (latDiff > 20.0f) continue; // Same lane check

                float dist = 0.0f;
                if (isHorizontal) {
                    dist = isPositive ? (position.x - other.getPosition().x) : (other.getPosition().x - position.x);
                } else {
                    dist = isPositive ? (position.y - other.getPosition().y) : (other.getPosition().y - position.y);
                }

                if (dist > 0.0f && dist < 145.0f) {
                    emergencyBehind = true;
                    break;
                }
            }

            if (emergencyBehind && adjId >= 0 && adjId < static_cast<int>(allLanes.size())) {
                const Lane& adjLane = allLanes[adjId];
                float candLateral = adjLane.getCenterLateral() - carWidth / 2.0f;
                bool adjClear = true;

                for (const auto& other : allVehicles) {
                    if (other.getId() == uniqueId) continue;
                    float otherLat = isHorizontal ? other.getPosition().y : other.getPosition().x;
                    if (std::abs(otherLat - candLateral) > 22.0f) continue;

                    float dist = 0.0f;
                    if (isHorizontal) {
                        dist = isPositive ? (other.getPosition().x - position.x) : (position.x - other.getPosition().x);
                    } else {
                        dist = isPositive ? (other.getPosition().y - position.y) : (position.y - other.getPosition().y);
                    }

                    if (dist > -40.0f && dist < 45.0f) {
                        adjClear = false;
                        break;
                    }

                    // If adjacent lane has a stopped vehicle ahead, do not pull over into it
                    if (dist > 0.0f && dist < 85.0f && other.getSpeed() < 15.0f) {
                        adjClear = false;
                        break;
                    }
                }

                if (adjClear) {
                    // Open gap exists in adjacent lane: immediately pull over to clear the path!
                    isChangingLane = true;
                    targetLateral = candLateral;
                    targetLane = const_cast<Lane*>(&adjLane);
                    isClearingWay = false;
                    clearWayTimer = 0.0f;
                    laneChangeCooldown = 2.0f;
                } else {
                    // ROAD IS INTACT / BLOCKED SIDE-BY-SIDE:
                    // Emergency vehicle is behind and there's no space to pull over!
                    // Accelerate fast to pull ahead of the adjacent vehicle and break the bottleneck!
                    isClearingWay = true;
                    clearWayTimer = 2.4f;
                    currentSpeed = baseSpeed * 1.55f;
                }
            }
        }

        // B) Dynamic Overtaking (Emergency Vehicles & Faster Traffic)
        // Must NOT attempt overtaking if stationary/still in an intersection/signal queue (currentSpeed > 20.0f required)
        if (!isChangingLane && laneChangeCooldown <= 0.0f && currentSpeed > 20.0f && adjId >= 0 && adjId < static_cast<int>(allLanes.size())) {
            bool shouldOvertake = false;
            // Case 1: Emergency vehicle approaching any leading vehicle
            if (priorityLevel >= 1) {
                for (const auto& other : allVehicles) {
                    if (other.getId() == uniqueId || other.getDirection() != direction) continue;
                    float latDiff = isHorizontal ? std::abs(other.getPosition().y - position.y)
                                                 : std::abs(other.getPosition().x - position.x);
                    if (latDiff > 20.0f) continue;

                    float dist = 0.0f;
                    if (isHorizontal) {
                        dist = isPositive ? (other.getPosition().x - position.x) : (position.x - other.getPosition().x);
                    } else {
                        dist = isPositive ? (other.getPosition().y - position.y) : (position.y - other.getPosition().y);
                    }

                    if (dist > 0.0f && dist < 140.0f) {
                        shouldOvertake = true;
                        break;
                    }
                }
            } 
            // Case 2: Faster civilian vehicle trailing a slower vehicle
            else if (priorityLevel == 0 && !isClearingWay) {
                for (const auto& other : allVehicles) {
                    if (other.getId() == uniqueId || other.getDirection() != direction) continue;
                    float latDiff = isHorizontal ? std::abs(other.getPosition().y - position.y)
                                                 : std::abs(other.getPosition().x - position.x);
                    if (latDiff > 20.0f) continue;

                    float dist = 0.0f;
                    if (isHorizontal) {
                        dist = isPositive ? (other.getPosition().x - position.x) : (position.x - other.getPosition().x);
                    } else {
                        dist = isPositive ? (other.getPosition().y - position.y) : (position.y - other.getPosition().y);
                    }

                    if (dist > 0.0f && dist < 95.0f && other.getSpeed() < currentSpeed - 6.0f) {
                        shouldOvertake = true;
                        break;
                    }
                }
            }

            if (shouldOvertake) {
                const Lane& adjLane = allLanes[adjId];
                float candidateTargetLat = adjLane.getCenterLateral() - carWidth / 2.0f;

                bool targetLaneClear = true;
                for (const auto& other : allVehicles) {
                    if (other.getId() == uniqueId) continue;
                    float otherLat = isHorizontal ? other.getPosition().y : other.getPosition().x;
                    if (std::abs(otherLat - candidateTargetLat) > 22.0f) continue;

                    float dist = 0.0f;
                    if (isHorizontal) {
                        dist = isPositive ? (other.getPosition().x - position.x) : (position.x - other.getPosition().x);
                    } else {
                        dist = isPositive ? (other.getPosition().y - position.y) : (position.y - other.getPosition().y);
                    }

                    // Direct collision clearance buffer
                    if (dist > -45.0f && dist < 50.0f) {
                        targetLaneClear = false;
                        break;
                    }

                    // If target lane also has a stationary / queued car ahead (e.g. stopped at red light / intersection),
                    // switching lanes is futile and causes stationary oscillation!
                    if (dist > 0.0f && dist < 120.0f && other.getSpeed() < 15.0f) {
                        targetLaneClear = false;
                        break;
                    }
                }

                if (targetLaneClear) {
                    isChangingLane = true;
                    targetLateral = candidateTargetLat;
                    targetLane = const_cast<Lane*>(&adjLane);
                    laneChangeCooldown = 2.0f;
                }
            }
        }
    }

    // =========================================================================
    // 2. Intersection Stop Lines & Conflict Arbitration
    // =========================================================================
    float frontCoord = 0.0f;
    if (direction == TravelDirection::EASTBOUND) {
        frontCoord = position.x + carLength;
    } else if (direction == TravelDirection::WESTBOUND) {
        frontCoord = position.x;
    } else if (direction == TravelDirection::SOUTHBOUND) {
        frontCoord = position.y + carLength;
    } else { // NORTHBOUND
        frontCoord = position.y;
    }

    struct IntDef {
        int id;
        float boxMinX, boxMaxX, boxMinY, boxMaxY;
        float stopLine;
    };

    std::vector<IntDef> intersections;
    // Intersection 1 (West, X=400): 4-Lane Avenue spans [330, 470], Highway spans [255, 445]
    // Intersection 2 (East, X=900): 4-Lane Avenue spans [830, 970], Highway spans [255, 445]
    if (direction == TravelDirection::EASTBOUND) {
        intersections.push_back({1, 328.0f, 472.0f, 252.0f, 448.0f, 310.0f});
        intersections.push_back({2, 828.0f, 972.0f, 252.0f, 448.0f, 810.0f});
    } else if (direction == TravelDirection::WESTBOUND) {
        intersections.push_back({2, 828.0f, 972.0f, 252.0f, 448.0f, 990.0f});
        intersections.push_back({1, 328.0f, 472.0f, 252.0f, 448.0f, 490.0f});
    } else if (direction == TravelDirection::SOUTHBOUND) {
        int intId = position.x < 600.0f ? 1 : 2;
        float bMinX = intId == 1 ? 328.0f : 828.0f;
        float bMaxX = intId == 1 ? 472.0f : 972.0f;
        intersections.push_back({intId, bMinX, bMaxX, 252.0f, 448.0f, 240.0f});
    } else if (direction == TravelDirection::NORTHBOUND) {
        int intId = position.x < 600.0f ? 1 : 2;
        float bMinX = intId == 1 ? 328.0f : 828.0f;
        float bMaxX = intId == 1 ? 472.0f : 972.0f;
        intersections.push_back({intId, bMinX, bMaxX, 252.0f, 448.0f, 460.0f});
    }

    for (const auto& inter : intersections) {
        float distToStopLine = isPositive ? (inter.stopLine - frontCoord) : (frontCoord - inter.stopLine);

        // Within 140px of approaching the stop line
        if (distToStopLine > 0.0f && distToStopLine < 140.0f) {
            // Find the signal controlling THIS vehicle's approach
            TrafficLight* mySignal = nullptr;
            for (auto& light : trafficLights) {
                if (light.getIntersectionId() == inter.id) {
                    if (isHorizontal && !light.getIsAvenue()) {
                        mySignal = &light;
                        break;
                    } else if (!isHorizontal && light.getIsAvenue()) {
                        mySignal = &light;
                        break;
                    }
                }
            }

            // A) Emergency Preemption: Only if priority >= 1 and signal is not currently green
            if (priorityLevel >= 1 && mySignal != nullptr) {
                if (!mySignal->isGreen() && mySignal->canPreempt()) {
                    mySignal->triggerEmergencyPreemption(uniqueId, 4.5f);
                }
            }

            // 2D Intersection Box Conflict Arbitration:
            // Check if ANY orthogonal vehicle is physically inside the active intersection conflict box
            bool boxOccupiedByCrossing = false;
            for (const auto& other : allVehicles) {
                if (other.getId() == uniqueId) continue;
                bool otherIsHorizontal = (other.getDirection() == TravelDirection::EASTBOUND || 
                                          other.getDirection() == TravelDirection::WESTBOUND);
                if (otherIsHorizontal == isHorizontal) continue; // Orthogonal only

                sf::FloatRect otherBounds = other.getBounds();
                bool otherInBox = (otherBounds.position.x + otherBounds.size.x > inter.boxMinX + 3.0f &&
                                   otherBounds.position.x < inter.boxMaxX - 3.0f &&
                                   otherBounds.position.y + otherBounds.size.y > inter.boxMinY + 3.0f &&
                                   otherBounds.position.y < inter.boxMaxY - 3.0f);

                if (otherInBox) {
                    boxOccupiedByCrossing = true;
                    break;
                }
            }

            // B) Signal Compliance & Emergency Right-of-Way:
            bool signalHalt = false;
            if (mySignal != nullptr) {
                if (priorityLevel >= 1) {
                    // Emergency vehicles have legal right-of-way!
                    // If crossing box is clear of cross traffic, they proceed through red/yellow without waiting!
                    if (boxOccupiedByCrossing) {
                        signalHalt = true; // Only yield if another car is physically inside the box
                    }
                } else {
                    // Civilian vehicles obey red/yellow signals
                    if (mySignal->isRed()) {
                        signalHalt = true;
                    } else if (mySignal->isYellow()) {
                        if (distToStopLine > 32.0f) {
                            signalHalt = true; // Safe to stop for yellow
                        }
                    }
                }
            }

            if (signalHalt || boxOccupiedByCrossing) {
                float tentativeFront = isPositive ? (frontCoord + currentSpeed * deltaTime)
                                                  : (frontCoord - currentSpeed * deltaTime);
                bool wouldCross = isPositive ? (tentativeFront > inter.stopLine - 5.0f)
                                             : (tentativeFront < inter.stopLine + 5.0f);
                if (wouldCross || distToStopLine < 10.0f) {
                    currentSpeed = std::max(0.0f, (distToStopLine - 5.0f) / std::max(0.04f, deltaTime));
                    isBraking = true;
                }
            }
        }
    }

    // =========================================================================
    // 3. Pairwise Headway Checking (O(n^2)) - True Bumper-to-Bumper Distance
    // =========================================================================
    float nearestBumperGap = std::numeric_limits<float>::max();
    float leaderSpeed = baseSpeed;

    for (const auto& other : allVehicles) {
        if (other.getId() == uniqueId || other.getDirection() != direction) continue;

        // Same lateral lane
        float lateralDiff = isHorizontal ? std::abs(other.getPosition().y - position.y)
                                         : std::abs(other.getPosition().x - position.x);
        if (lateralDiff > 22.0f) continue;

        sf::FloatRect otherBounds = other.getBounds();
        float bumperGap = 0.0f;
        if (direction == TravelDirection::EASTBOUND) {
            bumperGap = otherBounds.position.x - (position.x + carLength);
        } else if (direction == TravelDirection::WESTBOUND) {
            bumperGap = position.x - (otherBounds.position.x + otherBounds.size.x);
        } else if (direction == TravelDirection::SOUTHBOUND) {
            bumperGap = otherBounds.position.y - (position.y + carLength);
        } else { // NORTHBOUND
            bumperGap = position.y - (otherBounds.position.y + otherBounds.size.y);
        }

        if (bumperGap > 0.0f && bumperGap < nearestBumperGap) {
            nearestBumperGap = bumperGap;
            leaderSpeed = other.getSpeed();
        }
    }

    const float safeBumperCushion = 32.0f; // Minimum bumper-to-bumper cushion when stopped
    const float decelZone = 120.0f;        // Smooth deceleration zone

    if (nearestBumperGap < 1e7f) {
        if (nearestBumperGap <= safeBumperCushion) {
            currentSpeed = 0.0f;
            isBraking = true;
        } else if (nearestBumperGap < decelZone) {
            // Intelligent Driver Model headway speed matching
            float fraction = (nearestBumperGap - safeBumperCushion) / (decelZone - safeBumperCushion);
            float desiredSpeed = std::max(0.0f, leaderSpeed * fraction);
            if (currentSpeed > desiredSpeed) {
                currentSpeed = std::max(desiredSpeed, currentSpeed - 130.0f * deltaTime);
                isBraking = true;
            }
        }

        // Clamp tentative forward translation so bumper never penetrates the safe cushion
        float maxStep = std::max(0.0f, nearestBumperGap - safeBumperCushion);
        if (currentSpeed * deltaTime > maxStep) {
            currentSpeed = maxStep / std::max(0.02f, deltaTime);
            isBraking = true;
        }
    }

    // =========================================================================
    // 4. Universal 2D Bounding Box Collision Guard (with In-Lane Following Rules)
    // =========================================================================
    for (const auto& other : allVehicles) {
        if (other.getId() == uniqueId) continue;

        bool otherIsHorizontal = (other.getDirection() == TravelDirection::EASTBOUND || 
                                  other.getDirection() == TravelDirection::WESTBOUND);

        // A) Parallel highway lanes (same direction): only check if same lane or changing lane
        if (otherIsHorizontal == isHorizontal) {
            if (other.getDirection() == direction) {
                float lateralDiff = isHorizontal ? std::abs(other.getPosition().y - position.y)
                                                 : std::abs(other.getPosition().x - position.x);
                if (lateralDiff > 22.0f && !isChangingLane && !other.getIsChangingLane()) {
                    continue; // In adjacent parallel lane, completely safe!
                }
            } else {
                continue; // Opposing flow separated by median, safe!
            }
        }

        sf::FloatRect otherBounds = other.getBounds();
        sf::FloatRect nextBounds = getBounds();
        if (isHorizontal) {
            nextBounds.position.x += (isPositive ? 1.0f : -1.0f) * currentSpeed * deltaTime;
        } else {
            nextBounds.position.y += (isPositive ? 1.0f : -1.0f) * currentSpeed * deltaTime;
        }

        float overlapBuffer = 2.5f;
        sf::FloatRect expandedBounds(
            sf::Vector2f(nextBounds.position.x - overlapBuffer, nextBounds.position.y - overlapBuffer),
            sf::Vector2f(nextBounds.size.x + overlapBuffer * 2.0f, nextBounds.size.y + overlapBuffer * 2.0f)
        );

        if (expandedBounds.findIntersection(otherBounds).has_value()) {
            bool shouldYield = false;

            // Check if both vehicles are moving in the SAME lane and direction
            bool sameDirection = (other.getDirection() == direction);
            float lateralDiff = isHorizontal ? std::abs(other.getPosition().y - position.y)
                                             : std::abs(other.getPosition().x - position.x);
            bool sameLane = sameDirection && (lateralDiff <= 22.0f);

            if (sameLane) {
                // Rule 1: In the same lane, the trailing vehicle behind MUST yield to the leading vehicle!
                // Emergency vehicles never crash into cars in front of them in the same lane.
                bool isBehind = false;
                if (direction == TravelDirection::EASTBOUND) {
                    isBehind = (position.x < other.getPosition().x);
                } else if (direction == TravelDirection::WESTBOUND) {
                    isBehind = (position.x > other.getPosition().x);
                } else if (direction == TravelDirection::SOUTHBOUND) {
                    isBehind = (position.y < other.getPosition().y);
                } else { // NORTHBOUND
                    isBehind = (position.y > other.getPosition().y);
                }

                if (isBehind) {
                    shouldYield = true;
                } else {
                    shouldYield = false; // Leading vehicle continues driving forward
                }
            } else {
                // Rule 2: Orthogonal intersections or lane-merge conflict arbitration
                if (this->priorityLevel < other.getPriority()) {
                    shouldYield = true;
                } else if (this->priorityLevel > other.getPriority()) {
                    shouldYield = false;
                } else {
                    // Tie-breaker: Cross-street yields to Highway, else deterministic ID tie-breaker
                    if (!isHorizontal && otherIsHorizontal) {
                        shouldYield = true;
                    } else if (isHorizontal && !otherIsHorizontal) {
                        shouldYield = false;
                    } else {
                        shouldYield = (this->uniqueId < other.getId());
                    }
                }
            }

            if (shouldYield) {
                currentSpeed = 0.0f;
                isBraking = true;
                break;
            }
        }
    }

    // =========================================================================
    // 5. Update Translation
    // =========================================================================
    float moveDist = currentSpeed * deltaTime;
    if (isHorizontal) {
        position.x += isPositive ? moveDist : -moveDist;
    } else {
        position.y += isPositive ? moveDist : -moveDist;
    }
}

void Vehicle::draw(sf::RenderWindow& window, const sf::Font* font) const {
    bool isHorizontal = (direction == TravelDirection::EASTBOUND || direction == TravelDirection::WESTBOUND);
    bool isReverse = (direction == TravelDirection::WESTBOUND || direction == TravelDirection::NORTHBOUND);

    float carLength = (vehicleType == VehicleType::AMBULANCE || vehicleType == VehicleType::FIRE_TRUCK) ? 66.0f : 58.0f;
    float carWidth  = (vehicleType == VehicleType::AMBULANCE || vehicleType == VehicleType::FIRE_TRUCK) ? 30.0f : 26.0f;

    bool strobePhase = (std::fmod(strobeTimer, 0.28f) < 0.14f);

    if (isHorizontal) {
        float drawW = carLength;
        float drawH = carWidth;

        // 1. Soft Realistic Ground Drop Shadow (Provides 3D ground elevation)
        sf::RectangleShape shadow(sf::Vector2f(drawW + 4.0f, drawH + 3.0f));
        shadow.setPosition(sf::Vector2f(position.x + 2.0f, position.y + 2.5f));
        shadow.setFillColor(sf::Color(15, 18, 22, 110));
        window.draw(shadow);

        // 2. Volumetric Forward Headlight Beams (Illuminating road surface)
        auto drawHeadlightCone = [&](float startY) {
            sf::ConvexShape beam;
            beam.setPointCount(4);
            if (direction == TravelDirection::EASTBOUND) {
                beam.setPoint(0, sf::Vector2f(position.x + drawW, startY));
                beam.setPoint(1, sf::Vector2f(position.x + drawW + 36.0f, startY - 7.0f));
                beam.setPoint(2, sf::Vector2f(position.x + drawW + 36.0f, startY + 11.0f));
                beam.setPoint(3, sf::Vector2f(position.x + drawW, startY + 4.0f));
            } else {
                beam.setPoint(0, sf::Vector2f(position.x, startY));
                beam.setPoint(1, sf::Vector2f(position.x - 36.0f, startY - 7.0f));
                beam.setPoint(2, sf::Vector2f(position.x - 36.0f, startY + 11.0f));
                beam.setPoint(3, sf::Vector2f(position.x, startY + 4.0f));
            }
            beam.setFillColor(sf::Color(255, 248, 190, 42));
            window.draw(beam);
        };
        drawHeadlightCone(position.y + 3.0f);
        drawHeadlightCone(position.y + drawH - 7.0f);

        // 3. Alloy Wheels & Rubber Tires (4 Wheels)
        auto drawWheelH = [&](float wx, float wy) {
            // Dark Rubber Tire
            sf::RectangleShape tire(sf::Vector2f(11.0f, 3.5f));
            tire.setFillColor(sf::Color(20, 20, 22));
            tire.setPosition(sf::Vector2f(wx, wy));
            window.draw(tire);

            // Metallic Alloy Rim Center
            sf::RectangleShape rim(sf::Vector2f(6.0f, 2.0f));
            rim.setFillColor(sf::Color(185, 190, 200));
            rim.setPosition(sf::Vector2f(wx + 2.5f, wy + 0.75f));
            window.draw(rim);
        };

        float frontWheelX = isReverse ? (position.x + 8.0f) : (position.x + drawW - 19.0f);
        float rearWheelX  = isReverse ? (position.x + drawW - 19.0f) : (position.x + 8.0f);
        drawWheelH(frontWheelX, position.y - 2.0f);
        drawWheelH(frontWheelX, position.y + drawH - 1.5f);
        drawWheelH(rearWheelX,  position.y - 2.0f);
        drawWheelH(rearWheelX,  position.y + drawH - 1.5f);

        // 4. Main Vehicle Body Chassis
        sf::RectangleShape body(sf::Vector2f(drawW, drawH));
        body.setPosition(position);
        body.setFillColor(bodyColor);
        body.setOutlineThickness(1.5f);
        body.setOutlineColor(sf::Color(25, 28, 32));
        window.draw(body);

        // Metallic Roof Gloss Highlight (Subtle curvature shine)
        sf::RectangleShape glossHighlight(sf::Vector2f(drawW - 16.0f, 2.0f));
        glossHighlight.setPosition(sf::Vector2f(position.x + 8.0f, position.y + 2.5f));
        glossHighlight.setFillColor(sf::Color(255, 255, 255, 65));
        window.draw(glossHighlight);

        // Side-view Mirrors
        float mirrorX = isReverse ? (position.x + 14.0f) : (position.x + drawW - 16.0f);
        sf::RectangleShape mirrorTop(sf::Vector2f(3.0f, 2.5f));
        mirrorTop.setPosition(sf::Vector2f(mirrorX, position.y - 2.5f));
        mirrorTop.setFillColor(bodyColor);
        window.draw(mirrorTop);

        sf::RectangleShape mirrorBot(sf::Vector2f(3.0f, 2.5f));
        mirrorBot.setPosition(sf::Vector2f(mirrorX, position.y + drawH));
        mirrorBot.setFillColor(bodyColor);
        window.draw(mirrorBot);

        // 5. Authentic Vehicle Liveries & Specialized Architecture
        if (vehicleType == VehicleType::AMBULANCE) {
            // High-top paramedic white roof with fluorescent red emergency side flash
            sf::RectangleShape redStripe(sf::Vector2f(drawW, 6.0f));
            redStripe.setPosition(sf::Vector2f(position.x, position.y + (drawH - 6.0f) / 2.0f));
            redStripe.setFillColor(sf::Color(220, 25, 25));
            window.draw(redStripe);

            // Circular Medical Emblem Badge
            sf::CircleShape badge(8.0f);
            badge.setPosition(sf::Vector2f(position.x + (drawW - 16.0f) / 2.0f, position.y + (drawH - 16.0f) / 2.0f));
            badge.setFillColor(sf::Color(245, 245, 250));
            badge.setOutlineThickness(1.0f);
            badge.setOutlineColor(sf::Color(210, 30, 30));
            window.draw(badge);

            // Red Cross inside badge
            sf::RectangleShape crossH(sf::Vector2f(10.0f, 3.5f));
            crossH.setPosition(sf::Vector2f(position.x + (drawW - 10.0f) / 2.0f, position.y + (drawH - 3.5f) / 2.0f));
            crossH.setFillColor(sf::Color(220, 20, 20));
            window.draw(crossH);

            sf::RectangleShape crossV(sf::Vector2f(3.5f, 10.0f));
            crossV.setPosition(sf::Vector2f(position.x + (drawW - 3.5f) / 2.0f, position.y + (drawH - 10.0f) / 2.0f));
            crossV.setFillColor(sf::Color(220, 20, 20));
            window.draw(crossV);
        } else if (vehicleType == VehicleType::FIRE_TRUCK) {
            // Chrome front grille / bumper
            float bumperX = isReverse ? position.x : (position.x + drawW - 3.0f);
            sf::RectangleShape bumper(sf::Vector2f(3.0f, drawH - 4.0f));
            bumper.setPosition(sf::Vector2f(bumperX, position.y + 2.0f));
            bumper.setFillColor(sf::Color(225, 230, 235));
            window.draw(bumper);

            // Dual rooftop aluminum ladders
            sf::RectangleShape ladder1(sf::Vector2f(drawW * 0.62f, 2.5f));
            ladder1.setPosition(sf::Vector2f(position.x + 8.0f, position.y + 3.5f));
            ladder1.setFillColor(sf::Color(215, 222, 230));
            ladder1.setOutlineThickness(0.5f);
            ladder1.setOutlineColor(sf::Color(80, 85, 95));
            window.draw(ladder1);

            sf::RectangleShape ladder2(sf::Vector2f(drawW * 0.62f, 2.5f));
            ladder2.setPosition(sf::Vector2f(position.x + 8.0f, position.y + drawH - 6.0f));
            ladder2.setFillColor(sf::Color(215, 222, 230));
            ladder2.setOutlineThickness(0.5f);
            ladder2.setOutlineColor(sf::Color(80, 85, 95));
            window.draw(ladder2);
        } else if (vehicleType == VehicleType::POLICE) {
            // Classic Interceptor Black & White Pursuit Livery
            sf::RectangleShape whiteCabin(sf::Vector2f(drawW * 0.48f, drawH));
            whiteCabin.setPosition(sf::Vector2f(position.x + drawW * 0.26f, position.y));
            whiteCabin.setFillColor(sf::Color(245, 245, 250));
            window.draw(whiteCabin);

            // Steel Front Push-Bumper (Bull-Bar)
            float pushBarX = isReverse ? (position.x - 2.5f) : (position.x + drawW);
            sf::RectangleShape pushBar(sf::Vector2f(2.5f, drawH - 6.0f));
            pushBar.setPosition(sf::Vector2f(pushBarX, position.y + 3.0f));
            pushBar.setFillColor(sf::Color(20, 20, 25));
            window.draw(pushBar);

            // Gold Star Police Badge on door/roof
            sf::CircleShape policeStar(3.5f, 5);
            policeStar.setPosition(sf::Vector2f(position.x + (drawW - 7.0f) / 2.0f, position.y + (drawH - 7.0f) / 2.0f));
            policeStar.setFillColor(sf::Color(225, 195, 45));
            window.draw(policeStar);
        } else if (vehicleType == VehicleType::VIP) {
            // Luxury gold perimeter trim & executive chrome
            sf::RectangleShape goldTrim(sf::Vector2f(drawW - 6.0f, drawH - 4.0f));
            goldTrim.setPosition(sf::Vector2f(position.x + 3.0f, position.y + 2.0f));
            goldTrim.setFillColor(sf::Color::Transparent);
            goldTrim.setOutlineThickness(1.0f);
            goldTrim.setOutlineColor(sf::Color(220, 195, 80));
            window.draw(goldTrim);
        }

        // 6. Tinted Windshield Glass with Sunlight Reflection Glints
        float windX = isReverse ? (position.x + 10.0f) : (position.x + drawW - 17.0f);
        sf::RectangleShape windshield(sf::Vector2f(7.0f, drawH - 6.0f));
        windshield.setPosition(sf::Vector2f(windX, position.y + 3.0f));
        windshield.setFillColor(sf::Color(140, 195, 230, 225));
        windshield.setOutlineThickness(1.0f);
        windshield.setOutlineColor(sf::Color(25, 30, 35));
        window.draw(windshield);

        // Windshield glass glint
        sf::RectangleShape glint(sf::Vector2f(2.0f, drawH - 10.0f));
        glint.setPosition(sf::Vector2f(windX + 2.0f, position.y + 5.0f));
        glint.setFillColor(sf::Color(255, 255, 255, 130));
        window.draw(glint);

        // Rear Windshield
        float rearX = isReverse ? (position.x + drawW - 11.0f) : (position.x + 5.0f);
        sf::RectangleShape rearWindow(sf::Vector2f(5.0f, drawH - 6.0f));
        rearWindow.setPosition(sf::Vector2f(rearX, position.y + 3.0f));
        rearWindow.setFillColor(sf::Color(130, 180, 215, 190));
        rearWindow.setOutlineThickness(1.0f);
        rearWindow.setOutlineColor(sf::Color(25, 30, 35));
        window.draw(rearWindow);

        // 7. Headlights & Taillights
        float frontX = isReverse ? position.x : (position.x + drawW - 3.0f);
        float backX  = isReverse ? (position.x + drawW - 3.0f) : position.x;

        // Front Headlamps (Crystal White)
        sf::RectangleShape headTop(sf::Vector2f(3.0f, 4.0f));
        headTop.setPosition(sf::Vector2f(frontX, position.y + 2.5f));
        headTop.setFillColor(sf::Color(255, 250, 190));
        window.draw(headTop);

        sf::RectangleShape headBot(sf::Vector2f(3.0f, 4.0f));
        headBot.setPosition(sf::Vector2f(frontX, position.y + drawH - 6.5f));
        headBot.setFillColor(sf::Color(255, 250, 190));
        window.draw(headBot);

        // Dynamic Brake Lamps (intense neon glowing flare when decelerating!)
        if (isBraking) {
            // Radiant Brake Halo
            sf::CircleShape brakeHalo(8.0f);
            brakeHalo.setPosition(sf::Vector2f(backX - 4.0f, position.y - 2.0f));
            brakeHalo.setFillColor(sf::Color(255, 20, 20, 75));
            window.draw(brakeHalo);
            brakeHalo.setPosition(sf::Vector2f(backX - 4.0f, position.y + drawH - 14.0f));
            window.draw(brakeHalo);
        }

        sf::Color brakeColor = isBraking ? sf::Color(255, 30, 30) : sf::Color(160, 20, 20);
        sf::RectangleShape brakeTop(sf::Vector2f(3.0f, 4.5f));
        brakeTop.setPosition(sf::Vector2f(backX, position.y + 2.5f));
        brakeTop.setFillColor(brakeColor);
        window.draw(brakeTop);

        sf::RectangleShape brakeBot(sf::Vector2f(3.0f, 4.5f));
        brakeBot.setPosition(sf::Vector2f(backX, position.y + drawH - 7.0f));
        brakeBot.setFillColor(brakeColor);
        window.draw(brakeBot);

        // 8. Flashing Rooftop Emergency Strobes with Radiant Light Halos
        if (isEmergency()) {
            float strobeX = position.x + (drawW - 10.0f) / 2.0f;
            sf::Color colL = (vehicleType == VehicleType::FIRE_TRUCK)
                             ? (strobePhase ? sf::Color(255, 30, 30) : sf::Color(255, 180, 0))
                             : (strobePhase ? sf::Color(255, 30, 30) : sf::Color(30, 90, 255));
            sf::Color colR = (vehicleType == VehicleType::FIRE_TRUCK)
                             ? (strobePhase ? sf::Color(255, 180, 0) : sf::Color(255, 30, 30))
                             : (strobePhase ? sf::Color(30, 90, 255) : sf::Color(255, 30, 30));

            // Pulsating Light Halo
            sf::CircleShape halo(11.0f);
            halo.setPosition(sf::Vector2f(strobeX - 6.0f, position.y + (drawH - 22.0f) / 2.0f));
            halo.setFillColor(sf::Color(colL.r, colL.g, colL.b, 60));
            window.draw(halo);

            // Aerodynamic Siren Housing Bar
            sf::RectangleShape lightBar(sf::Vector2f(10.0f, drawH - 8.0f));
            lightBar.setPosition(sf::Vector2f(strobeX, position.y + 4.0f));
            lightBar.setFillColor(sf::Color(25, 25, 30));
            window.draw(lightBar);

            sf::CircleShape lampL(3.5f);
            lampL.setPosition(sf::Vector2f(strobeX + 1.5f, position.y + 4.5f));
            lampL.setFillColor(colL);
            window.draw(lampL);

            sf::CircleShape lampR(3.5f);
            lampR.setPosition(sf::Vector2f(strobeX + 1.5f, position.y + drawH - 11.5f));
            lampR.setFillColor(colR);
            window.draw(lampR);
        } else if (vehicleType == VehicleType::VIP) {
            sf::CircleShape vipStrobe(2.5f);
            vipStrobe.setPosition(sf::Vector2f(frontX, position.y + (drawH - 5.0f) / 2.0f));
            vipStrobe.setFillColor(strobePhase ? sf::Color(100, 180, 255) : sf::Color(240, 240, 255));
            window.draw(vipStrobe);
        }

        // 9. On-Roof Identification Badging & Typography
        if (font != nullptr) {
            std::string onCarText = "";
            sf::Color textColor = sf::Color::White;
            unsigned int textSize = 8;

            if (vehicleType == VehicleType::AMBULANCE) {
                onCarText = "AMBULANCE";
                textColor = sf::Color(220, 20, 20);
                textSize = 8;
            } else if (vehicleType == VehicleType::FIRE_TRUCK) {
                onCarText = "FIRE DEPT";
                textColor = sf::Color(255, 230, 80);
                textSize = 8;
            } else if (vehicleType == VehicleType::POLICE) {
                onCarText = "POLICE";
                textColor = sf::Color(20, 20, 25);
                textSize = 8;
            } else if (vehicleType == VehicleType::VIP) {
                onCarText = "VIP";
                textColor = sf::Color(255, 215, 0);
                textSize = 9;
            } else {
                onCarText = uniqueId;
                textColor = sf::Color(240, 242, 248);
                textSize = 8;
            }

            sf::Text textMesh(*font, onCarText, textSize);
            textMesh.setStyle(sf::Text::Bold);
            textMesh.setFillColor(textColor);
            float textOffsetLeft = (vehicleType == VehicleType::AMBULANCE) ? 4.0f : ((drawW - 26.0f) / 2.0f);
            textMesh.setPosition(sf::Vector2f(position.x + textOffsetLeft, position.y + (drawH - 12.0f) / 2.0f));
            window.draw(textMesh);
        }
    } else {
        // Vertical Vehicles (Southbound / Northbound)
        float drawW = carWidth;
        float drawH = carLength;

        // 1. Drop Shadow
        sf::RectangleShape shadow(sf::Vector2f(drawW + 3.0f, drawH + 4.0f));
        shadow.setPosition(sf::Vector2f(position.x + 2.5f, position.y + 2.0f));
        shadow.setFillColor(sf::Color(15, 18, 22, 110));
        window.draw(shadow);

        // 2. Volumetric Forward Headlight Beams
        auto drawHeadlightConeV = [&](float startX) {
            sf::ConvexShape beam;
            beam.setPointCount(4);
            if (direction == TravelDirection::SOUTHBOUND) {
                beam.setPoint(0, sf::Vector2f(startX, position.y + drawH));
                beam.setPoint(1, sf::Vector2f(startX - 7.0f, position.y + drawH + 36.0f));
                beam.setPoint(2, sf::Vector2f(startX + 11.0f, position.y + drawH + 36.0f));
                beam.setPoint(3, sf::Vector2f(startX + 4.0f, position.y + drawH));
            } else {
                beam.setPoint(0, sf::Vector2f(startX, position.y));
                beam.setPoint(1, sf::Vector2f(startX - 7.0f, position.y - 36.0f));
                beam.setPoint(2, sf::Vector2f(startX + 11.0f, position.y - 36.0f));
                beam.setPoint(3, sf::Vector2f(startX + 4.0f, position.y));
            }
            beam.setFillColor(sf::Color(255, 248, 190, 42));
            window.draw(beam);
        };
        drawHeadlightConeV(position.x + 3.0f);
        drawHeadlightConeV(position.x + drawW - 7.0f);

        // 3. Alloy Wheels
        auto drawWheelV = [&](float wx, float wy) {
            sf::RectangleShape tire(sf::Vector2f(3.5f, 11.0f));
            tire.setFillColor(sf::Color(20, 20, 22));
            tire.setPosition(sf::Vector2f(wx, wy));
            window.draw(tire);

            sf::RectangleShape rim(sf::Vector2f(2.0f, 6.0f));
            rim.setFillColor(sf::Color(185, 190, 200));
            rim.setPosition(sf::Vector2f(wx + 0.75f, wy + 2.5f));
            window.draw(rim);
        };

        float frontWheelY = isReverse ? (position.y + 8.0f) : (position.y + drawH - 19.0f);
        float rearWheelY  = isReverse ? (position.y + drawH - 19.0f) : (position.y + 8.0f);
        drawWheelV(position.x - 2.0f, frontWheelY);
        drawWheelV(position.x + drawW - 1.5f, frontWheelY);
        drawWheelV(position.x - 2.0f, rearWheelY);
        drawWheelV(position.x + drawW - 1.5f, rearWheelY);

        // 4. Chassis Body
        sf::RectangleShape body(sf::Vector2f(drawW, drawH));
        body.setPosition(position);
        body.setFillColor(bodyColor);
        body.setOutlineThickness(1.5f);
        body.setOutlineColor(sf::Color(25, 28, 32));
        window.draw(body);

        // Gloss Highlight
        sf::RectangleShape glossHighlight(sf::Vector2f(2.0f, drawH - 16.0f));
        glossHighlight.setPosition(sf::Vector2f(position.x + 2.5f, position.y + 8.0f));
        glossHighlight.setFillColor(sf::Color(255, 255, 255, 65));
        window.draw(glossHighlight);

        // Side Mirrors
        float mirrorY = isReverse ? (position.y + 14.0f) : (position.y + drawH - 16.0f);
        sf::RectangleShape mirrorLeft(sf::Vector2f(2.5f, 3.0f));
        mirrorLeft.setPosition(sf::Vector2f(position.x - 2.5f, mirrorY));
        mirrorLeft.setFillColor(bodyColor);
        window.draw(mirrorLeft);

        sf::RectangleShape mirrorRight(sf::Vector2f(2.5f, 3.0f));
        mirrorRight.setPosition(sf::Vector2f(position.x + drawW, mirrorY));
        mirrorRight.setFillColor(bodyColor);
        window.draw(mirrorRight);

        // Liveries for Police / Ambulance
        if (vehicleType == VehicleType::POLICE) {
            sf::RectangleShape whiteCenter(sf::Vector2f(drawW, drawH * 0.48f));
            whiteCenter.setPosition(sf::Vector2f(position.x, position.y + drawH * 0.26f));
            whiteCenter.setFillColor(sf::Color(245, 245, 250));
            window.draw(whiteCenter);

            float pushBarY = isReverse ? (position.y - 2.5f) : (position.y + drawH);
            sf::RectangleShape pushBar(sf::Vector2f(drawW - 6.0f, 2.5f));
            pushBar.setPosition(sf::Vector2f(position.x + 3.0f, pushBarY));
            pushBar.setFillColor(sf::Color(20, 20, 25));
            window.draw(pushBar);

            sf::CircleShape policeStar(3.5f, 5);
            policeStar.setPosition(sf::Vector2f(position.x + (drawW - 7.0f) / 2.0f, position.y + (drawH - 7.0f) / 2.0f));
            policeStar.setFillColor(sf::Color(225, 195, 45));
            window.draw(policeStar);
        } else if (vehicleType == VehicleType::AMBULANCE) {
            sf::RectangleShape redStripe(sf::Vector2f(6.0f, drawH));
            redStripe.setPosition(sf::Vector2f(position.x + (drawW - 6.0f) / 2.0f, position.y));
            redStripe.setFillColor(sf::Color(220, 25, 25));
            window.draw(redStripe);
        }

        // Windshields
        float windY = isReverse ? (position.y + 10.0f) : (position.y + drawH - 17.0f);
        sf::RectangleShape windshield(sf::Vector2f(drawW - 6.0f, 7.0f));
        windshield.setPosition(sf::Vector2f(position.x + 3.0f, windY));
        windshield.setFillColor(sf::Color(140, 195, 230, 225));
        windshield.setOutlineThickness(1.0f);
        windshield.setOutlineColor(sf::Color(25, 30, 35));
        window.draw(windshield);

        float rearY = isReverse ? (position.y + drawH - 11.0f) : (position.y + 5.0f);
        sf::RectangleShape rearWindow(sf::Vector2f(drawW - 6.0f, 5.0f));
        rearWindow.setPosition(sf::Vector2f(position.x + 3.0f, rearY));
        rearWindow.setFillColor(sf::Color(130, 180, 215, 190));
        rearWindow.setOutlineThickness(1.0f);
        rearWindow.setOutlineColor(sf::Color(25, 30, 35));
        window.draw(rearWindow);

        // Headlights & Taillights
        float frontY = isReverse ? position.y : (position.y + drawH - 3.0f);
        float backY  = isReverse ? (position.y + drawH - 3.0f) : position.y;

        sf::RectangleShape headL(sf::Vector2f(4.0f, 3.0f));
        headL.setPosition(sf::Vector2f(position.x + 2.5f, frontY));
        headL.setFillColor(sf::Color(255, 250, 190));
        window.draw(headL);

        sf::RectangleShape headR(sf::Vector2f(4.0f, 3.0f));
        headR.setPosition(sf::Vector2f(position.x + drawW - 6.5f, frontY));
        headR.setFillColor(sf::Color(255, 250, 190));
        window.draw(headR);

        if (isBraking) {
            sf::CircleShape brakeHalo(8.0f);
            brakeHalo.setPosition(sf::Vector2f(position.x - 2.0f, backY - 4.0f));
            brakeHalo.setFillColor(sf::Color(255, 20, 20, 75));
            window.draw(brakeHalo);
            brakeHalo.setPosition(sf::Vector2f(position.x + drawW - 14.0f, backY - 4.0f));
            window.draw(brakeHalo);
        }

        sf::Color brakeColor = isBraking ? sf::Color(255, 30, 30) : sf::Color(160, 20, 20);
        sf::RectangleShape brakeL(sf::Vector2f(4.5f, 3.0f));
        brakeL.setPosition(sf::Vector2f(position.x + 2.5f, backY));
        brakeL.setFillColor(brakeColor);
        window.draw(brakeL);

        sf::RectangleShape brakeR(sf::Vector2f(4.5f, 3.0f));
        brakeR.setPosition(sf::Vector2f(position.x + drawW - 7.0f, backY));
        brakeR.setFillColor(brakeColor);
        window.draw(brakeR);

        // Emergency Strobes
        if (isEmergency()) {
            float strobeY = position.y + (drawH - 10.0f) / 2.0f;
            sf::Color colT = strobePhase ? sf::Color(255, 30, 30) : sf::Color(30, 90, 255);
            sf::Color colB = strobePhase ? sf::Color(30, 90, 255) : sf::Color(255, 30, 30);

            sf::CircleShape halo(11.0f);
            halo.setPosition(sf::Vector2f(position.x + (drawW - 22.0f) / 2.0f, strobeY - 6.0f));
            halo.setFillColor(sf::Color(colT.r, colT.g, colT.b, 60));
            window.draw(halo);

            sf::RectangleShape lightBar(sf::Vector2f(drawW - 8.0f, 10.0f));
            lightBar.setPosition(sf::Vector2f(position.x + 4.0f, strobeY));
            lightBar.setFillColor(sf::Color(25, 25, 30));
            window.draw(lightBar);

            sf::CircleShape lampT(3.5f);
            lampT.setPosition(sf::Vector2f(position.x + 4.5f, strobeY + 1.5f));
            lampT.setFillColor(colT);
            window.draw(lampT);

            sf::CircleShape lampB(3.5f);
            lampB.setPosition(sf::Vector2f(position.x + drawW - 11.5f, strobeY + 1.5f));
            lampB.setFillColor(colB);
            window.draw(lampB);
        }

        // On-Roof Label
        if (font != nullptr) {
            std::string onCarText = (vehicleType == VehicleType::POLICE) ? "POLICE" : uniqueId;
            sf::Text textMesh(*font, onCarText, 8);
            textMesh.setStyle(sf::Text::Bold);
            textMesh.setFillColor((vehicleType == VehicleType::POLICE) ? sf::Color(20, 20, 25) : sf::Color::White);
            textMesh.setPosition(sf::Vector2f(position.x + 3.0f, position.y + (drawH - 12.0f) / 2.0f));
            window.draw(textMesh);
        }
    }
}

sf::FloatRect Vehicle::getBounds() const {
    float carLength = (vehicleType == VehicleType::AMBULANCE || vehicleType == VehicleType::FIRE_TRUCK) ? 66.0f : 58.0f;
    float carWidth  = (vehicleType == VehicleType::AMBULANCE || vehicleType == VehicleType::FIRE_TRUCK) ? 30.0f : 26.0f;

    bool isHorizontal = (direction == TravelDirection::EASTBOUND || direction == TravelDirection::WESTBOUND);
    float w = isHorizontal ? carLength : carWidth;
    float h = isHorizontal ? carWidth  : carLength;

    return sf::FloatRect(position, sf::Vector2f(w, h));
}

bool Vehicle::isOffScreen() const {
    if (direction == TravelDirection::EASTBOUND && position.x > 1360.0f) return true;
    if (direction == TravelDirection::WESTBOUND && position.x < -90.0f) return true;
    if (direction == TravelDirection::SOUTHBOUND && position.y > 760.0f) return true;
    if (direction == TravelDirection::NORTHBOUND && position.y < -90.0f) return true;
    return false;
}

std::string Vehicle::getId() const {
    return uniqueId;
}

VehicleType Vehicle::getType() const {
    return vehicleType;
}

int Vehicle::getPriority() const {
    return priorityLevel;
}

float Vehicle::getSpeed() const {
    return currentSpeed;
}

sf::Vector2f Vehicle::getPosition() const {
    return position;
}

TravelDirection Vehicle::getDirection() const {
    return direction;
}

bool Vehicle::isEmergency() const {
    return (vehicleType == VehicleType::AMBULANCE || 
            vehicleType == VehicleType::FIRE_TRUCK || 
            vehicleType == VehicleType::POLICE);
}

bool Vehicle::getIsChangingLane() const {
    return isChangingLane;
}

std::string Vehicle::getTypeString() const {
    switch (vehicleType) {
        case VehicleType::AMBULANCE:  return "AMBULANCE";
        case VehicleType::FIRE_TRUCK: return "FIRE TRUCK";
        case VehicleType::VIP:        return "VIP CONVOY";
        case VehicleType::POLICE:     return "POLICE";
        case VehicleType::REGULAR_CAR:
        default:                      return "CIVILIAN";
    }
}
