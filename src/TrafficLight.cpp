/**
 * @file TrafficLight.cpp
 * @brief Implementation of the TrafficLight class with fair duty-cycles and EVP cooldowns.
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Intelligent Traffic Control & Emergency Preemption
 * Contributor: Student 3 (Traffic Control & State Machine)
 * Reviewers: Student 1, Student 2, Student 4
 */

#include "TrafficLight.h"

TrafficLight::TrafficLight(sf::Vector2f pos, std::string name, int initialState, int intId, bool isAve)
    : state(initialState), timer(0.0f), position(pos), intersectionName(name),
      preemptionActive(false), preemptionVehicleId(""), preemptionTimer(0.0f),
      preemptionCooldown(0.0f), intersectionId(intId), isAvenue(isAve) {
}

void TrafficLight::update(float deltaTime) {
    if (preemptionCooldown > 0.0f) {
        preemptionCooldown -= deltaTime;
    }

    if (preemptionActive) {
        preemptionTimer -= deltaTime;
        state = 1; // Green phase locked during preemption
        if (preemptionTimer <= 0.0f) {
            clearPreemption();
        }
        return;
    }

    timer += deltaTime;

    // Cyclic FSM: Red (5s) -> Green (5s) -> Yellow (2s) -> Red (5s)
    if (state == 0 && timer >= RED_DURATION) {
        state = 1; // Green
        timer = 0.0f;
    } else if (state == 1 && timer >= GREEN_DURATION) {
        state = 2; // Yellow
        timer = 0.0f;
    } else if (state == 2 && timer >= YELLOW_DURATION) {
        state = 0; // Red
        timer = 0.0f;
    }
}

bool TrafficLight::triggerEmergencyPreemption(const std::string& vehicleId, float duration) {
    // If cooldown is active, deny preemption to ensure the cross-street receives its fair green phase
    if (preemptionCooldown > 0.0f) {
        return false;
    }

    preemptionActive = true;
    preemptionVehicleId = vehicleId;
    preemptionTimer = duration;
    state = 1; // Immediate Green wave
    return true;
}

void TrafficLight::clearPreemption() {
    preemptionActive = false;
    preemptionVehicleId = "";
    preemptionCooldown = 8.0f; // 8-second cooldown guarantees cross-street green time
    timer = 0.0f;
}

void TrafficLight::draw(sf::RenderWindow& window, const sf::Font* font) const {
    // 1. Heavy-duty Galvanized Steel Mounting Mast / Pole
    sf::RectangleShape pole(sf::Vector2f(8.0f, 26.0f));
    pole.setPosition(sf::Vector2f(position.x + 14.0f, position.y + 82.0f));
    pole.setFillColor(sf::Color(45, 48, 52));
    window.draw(pole);

    sf::RectangleShape mastBracket(sf::Vector2f(14.0f, 6.0f));
    mastBracket.setPosition(sf::Vector2f(position.x + 11.0f, position.y + 82.0f));
    mastBracket.setFillColor(sf::Color(65, 70, 75));
    window.draw(mastBracket);

    // 2. Drop Shadow under Signal Housing
    sf::RectangleShape housingShadow(sf::Vector2f(38.0f, 84.0f));
    housingShadow.setPosition(sf::Vector2f(position.x + 3.0f, position.y + 3.0f));
    housingShadow.setFillColor(sf::Color(10, 12, 15, 120));
    window.draw(housingShadow);

    // 3. Compact Signal Housing (36px wide x 82px tall) with Beveled Border
    sf::RectangleShape housing(sf::Vector2f(36.0f, 82.0f));
    housing.setPosition(position);
    housing.setFillColor(sf::Color(28, 30, 34));
    housing.setOutlineThickness(2.0f);
    housing.setOutlineColor(preemptionActive ? sf::Color(255, 150, 0) : sf::Color(55, 60, 68));
    window.draw(housing);

    // Sun Hood Visor Helper (Shields lamps from glare)
    auto drawVisor = [&](float yOffset) {
        sf::ConvexShape visor;
        visor.setPointCount(4);
        visor.setPoint(0, sf::Vector2f(position.x + 6.0f, position.y + yOffset));
        visor.setPoint(1, sf::Vector2f(position.x + 9.0f, position.y + yOffset - 4.0f));
        visor.setPoint(2, sf::Vector2f(position.x + 27.0f, position.y + yOffset - 4.0f));
        visor.setPoint(3, sf::Vector2f(position.x + 30.0f, position.y + yOffset));
        visor.setFillColor(sf::Color(18, 20, 22));
        window.draw(visor);
    };

    // 4. Red Lamp with Sun Visor and Radiant Halo
    drawVisor(7.0f);
    if (state == 0) {
        // Glowing Red Halo
        sf::CircleShape redHalo(16.0f);
        redHalo.setPosition(sf::Vector2f(position.x + 2.0f, position.y + 1.0f));
        redHalo.setFillColor(sf::Color(255, 40, 40, 50));
        window.draw(redHalo);
    }
    sf::CircleShape redRim(10.5f);
    redRim.setPosition(sf::Vector2f(position.x + 7.5f, position.y + 6.5f));
    redRim.setFillColor(sf::Color(20, 20, 22));
    window.draw(redRim);

    sf::CircleShape redLamp(9.0f);
    redLamp.setPosition(sf::Vector2f(position.x + 9.0f, position.y + 8.0f));
    redLamp.setFillColor(state == 0 ? sf::Color(255, 35, 35) : sf::Color(75, 15, 15));
    window.draw(redLamp);
    if (state == 0) {
        sf::CircleShape redBulb(4.0f);
        redBulb.setPosition(sf::Vector2f(position.x + 14.0f, position.y + 13.0f));
        redBulb.setFillColor(sf::Color(255, 180, 180));
        window.draw(redBulb);
    }

    // 5. Yellow Lamp with Sun Visor and Radiant Halo
    drawVisor(31.0f);
    if (state == 2) {
        sf::CircleShape yelHalo(16.0f);
        yelHalo.setPosition(sf::Vector2f(position.x + 2.0f, position.y + 25.0f));
        yelHalo.setFillColor(sf::Color(255, 220, 30, 50));
        window.draw(yelHalo);
    }
    sf::CircleShape yelRim(10.5f);
    yelRim.setPosition(sf::Vector2f(position.x + 7.5f, position.y + 30.5f));
    yelRim.setFillColor(sf::Color(20, 20, 22));
    window.draw(yelRim);

    sf::CircleShape yellowLamp(9.0f);
    yellowLamp.setPosition(sf::Vector2f(position.x + 9.0f, position.y + 32.0f));
    yellowLamp.setFillColor(state == 2 ? sf::Color(255, 215, 20) : sf::Color(75, 65, 12));
    window.draw(yellowLamp);
    if (state == 2) {
        sf::CircleShape yelBulb(4.0f);
        yelBulb.setPosition(sf::Vector2f(position.x + 14.0f, position.y + 37.0f));
        yelBulb.setFillColor(sf::Color(255, 255, 200));
        window.draw(yelBulb);
    }

    // 6. Green Lamp with Sun Visor and Radiant Halo
    drawVisor(55.0f);
    if (state == 1) {
        sf::CircleShape grnHalo(16.0f);
        grnHalo.setPosition(sf::Vector2f(position.x + 2.0f, position.y + 49.0f));
        grnHalo.setFillColor(sf::Color(30, 240, 50, 50));
        window.draw(grnHalo);
    }
    sf::CircleShape grnRim(10.5f);
    grnRim.setPosition(sf::Vector2f(position.x + 7.5f, position.y + 54.5f));
    grnRim.setFillColor(sf::Color(20, 20, 22));
    window.draw(grnRim);

    sf::CircleShape greenLamp(9.0f);
    greenLamp.setPosition(sf::Vector2f(position.x + 9.0f, position.y + 56.0f));
    greenLamp.setFillColor(state == 1 ? sf::Color(35, 235, 45) : sf::Color(12, 68, 18));
    window.draw(greenLamp);
    if (state == 1) {
        sf::CircleShape grnBulb(4.0f);
        grnBulb.setPosition(sf::Vector2f(position.x + 14.0f, position.y + 61.0f));
        grnBulb.setFillColor(sf::Color(190, 255, 195));
        window.draw(grnBulb);
    }

    // 7. Street Name Plate and Preemption Badge
    if (font != nullptr) {
        sf::RectangleShape plate(sf::Vector2f(110.0f, 15.0f));
        plate.setPosition(sf::Vector2f(position.x - 37.0f, position.y - 18.0f));
        plate.setFillColor(sf::Color(24, 28, 34, 230));
        plate.setOutlineThickness(1.0f);
        plate.setOutlineColor(sf::Color(60, 68, 78));
        window.draw(plate);

        sf::Text nameText(*font, intersectionName, 9);
        nameText.setStyle(sf::Text::Bold);
        nameText.setFillColor(sf::Color(215, 220, 230));
        nameText.setPosition(sf::Vector2f(position.x - 33.0f, position.y - 17.0f));
        window.draw(nameText);

        if (preemptionActive) {
            sf::RectangleShape evpBadge(sf::Vector2f(105.0f, 18.0f));
            evpBadge.setPosition(sf::Vector2f(position.x + 42.0f, position.y + 10.0f));
            evpBadge.setFillColor(sf::Color(210, 35, 20, 240));
            evpBadge.setOutlineThickness(1.5f);
            evpBadge.setOutlineColor(sf::Color(255, 225, 50));
            window.draw(evpBadge);

            sf::Text evpText(*font, "EMERGENCY: " + preemptionVehicleId, 9);
            evpText.setStyle(sf::Text::Bold);
            evpText.setFillColor(sf::Color::White);
            evpText.setPosition(sf::Vector2f(position.x + 46.0f, position.y + 13.0f));
            window.draw(evpText);
        }
    }
}

bool TrafficLight::isRed() const {
    return state == 0;
}

bool TrafficLight::isGreen() const {
    return state == 1;
}

bool TrafficLight::isYellow() const {
    return state == 2;
}

bool TrafficLight::isPreempted() const {
    return preemptionActive;
}

bool TrafficLight::canPreempt() const {
    return !preemptionActive && preemptionCooldown <= 0.0f;
}

void TrafficLight::setState(int newState) {
    if (!preemptionActive && state != newState) {
        state = newState;
        timer = 0.0f;
    }
}

int TrafficLight::getState() const {
    return state;
}

float TrafficLight::getTimer() const {
    return timer;
}

void TrafficLight::resetTimer() {
    timer = 0.0f;
}

sf::Vector2f TrafficLight::getPosition() const {
    return position;
}

std::string TrafficLight::getIntersectionName() const {
    return intersectionName;
}

std::string TrafficLight::getPreemptionVehicleId() const {
    return preemptionVehicleId;
}

int TrafficLight::getIntersectionId() const {
    return intersectionId;
}

bool TrafficLight::getIsAvenue() const {
    return isAvenue;
}
