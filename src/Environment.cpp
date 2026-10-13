/**
 * @file Environment.cpp
 * @brief Implementation of the urban environment (buildings, sidewalks, pedestrians, greenery).
 * 
 * Project: Traffic Simulation Using C++, OOP, DSA, and SFML
 * Group Project: Team of 4 Students (PBL Initiative)
 * Module: Multi-Lane Highway & Urban Environment Simulation
 * Contributor: Student 2 (Environment & Road Geometry)
 * Reviewers: Student 1, Student 3, Student 4
 */

#include "Environment.h"
#include <cmath>

Environment::Environment() : beaconTimer(0.0f) {
    initBuildings();
    initGreenery();
    initPedestrians();
}

void Environment::initBuildings() {
    // Block 1 (Top-Left)
    buildings.push_back({sf::FloatRect(sf::Vector2f(50.0f, 55.0f), sf::Vector2f(150.0f, 140.0f)),
                         sf::Color(240, 245, 250), sf::Color(220, 230, 240),
                         "METRO HOSPITAL", sf::Color(220, 20, 20), 0});

    buildings.push_back({sf::FloatRect(sf::Vector2f(215.0f, 65.0f), sf::Vector2f(95.0f, 130.0f)),
                         sf::Color(210, 235, 225), sf::Color(190, 220, 210),
                         "CITY CLINIC", sf::Color(20, 140, 90), 4});

    // Block 2 (Top-Center)
    buildings.push_back({sf::FloatRect(sf::Vector2f(490.0f, 50.0f), sf::Vector2f(145.0f, 150.0f)),
                         sf::Color(45, 75, 105), sf::Color(35, 60, 85),
                         "CYBER TOWER A", sf::Color(120, 210, 255), 3});

    buildings.push_back({sf::FloatRect(sf::Vector2f(650.0f, 55.0f), sf::Vector2f(160.0f, 145.0f)),
                         sf::Color(200, 200, 205), sf::Color(180, 180, 185),
                         "CENTRAL BANK", sf::Color(255, 215, 0), 4});

    // Block 3 (Top-Right)
    buildings.push_back({sf::FloatRect(sf::Vector2f(990.0f, 55.0f), sf::Vector2f(135.0f, 145.0f)),
                         sf::Color(180, 45, 45), sf::Color(150, 35, 35),
                         "FIRE DEPT #12", sf::Color(255, 230, 80), 1});

    buildings.push_back({sf::FloatRect(sf::Vector2f(1135.0f, 65.0f), sf::Vector2f(130.0f, 135.0f)),
                         sf::Color(215, 195, 175), sf::Color(195, 175, 155),
                         "CITY LOGISTICS", sf::Color(60, 60, 70), 5});

    // Block 4 (Bottom-Left)
    buildings.push_back({sf::FloatRect(sf::Vector2f(50.0f, 500.0f), sf::Vector2f(150.0f, 150.0f)),
                         sf::Color(55, 70, 90), sf::Color(40, 55, 75),
                         "POLICE PRECINCT", sf::Color(255, 220, 50), 2});

    buildings.push_back({sf::FloatRect(sf::Vector2f(215.0f, 510.0f), sf::Vector2f(95.0f, 140.0f)),
                         sf::Color(210, 210, 215), sf::Color(190, 190, 195),
                         "JUSTICE COURT", sf::Color(80, 80, 90), 4});

    // Block 5 (Bottom-Center)
    buildings.push_back({sf::FloatRect(sf::Vector2f(490.0f, 495.0f), sf::Vector2f(145.0f, 155.0f)),
                         sf::Color(40, 95, 85), sf::Color(30, 80, 70),
                         "COMMERCE TOWER B", sf::Color(150, 240, 200), 3});

    buildings.push_back({sf::FloatRect(sf::Vector2f(650.0f, 505.0f), sf::Vector2f(160.0f, 145.0f)),
                         sf::Color(225, 210, 190), sf::Color(205, 190, 170),
                         "GRAND HOTEL", sf::Color(240, 180, 40), 4});

    // Block 6 (Bottom-Right)
    buildings.push_back({sf::FloatRect(sf::Vector2f(990.0f, 505.0f), sf::Vector2f(135.0f, 145.0f)),
                         sf::Color(195, 85, 60), sf::Color(175, 70, 50),
                         "METRO ARCADE", sf::Color(255, 255, 255), 4});

    buildings.push_back({sf::FloatRect(sf::Vector2f(1135.0f, 510.0f), sf::Vector2f(130.0f, 140.0f)),
                         sf::Color(205, 175, 150), sf::Color(185, 155, 130),
                         "CITY RESIDENCES", sf::Color(60, 60, 70), 5});
}

void Environment::initGreenery() {
    // Trees along highway sidewalk borders & building lawns
    float topY = 230.0f;
    float botY = 475.0f;

    // Top block sidewalk trees
    for (float x = 70.0f; x < 300.0f; x += 55.0f) trees.push_back({sf::Vector2f(x, topY), 11.0f});
    for (float x = 515.0f; x < 790.0f; x += 60.0f) trees.push_back({sf::Vector2f(x, topY), 11.0f});
    for (float x = 1010.0f; x < 1260.0f; x += 60.0f) trees.push_back({sf::Vector2f(x, topY), 11.0f});

    // Bottom block sidewalk trees
    for (float x = 70.0f; x < 300.0f; x += 55.0f) trees.push_back({sf::Vector2f(x, botY), 11.0f});
    for (float x = 515.0f; x < 790.0f; x += 60.0f) trees.push_back({sf::Vector2f(x, botY), 11.0f});
    for (float x = 1010.0f; x < 1260.0f; x += 60.0f) trees.push_back({sf::Vector2f(x, botY), 11.0f});

    // Trees in the Highway Median (between vehicle crossings)
    for (float x = 50.0f; x < 290.0f; x += 65.0f) trees.push_back({sf::Vector2f(x, 360.0f), 8.0f});
    for (float x = 510.0f; x < 790.0f; x += 65.0f) trees.push_back({sf::Vector2f(x, 360.0f), 8.0f});
    for (float x = 1010.0f; x < 1270.0f; x += 65.0f) trees.push_back({sf::Vector2f(x, 360.0f), 8.0f});
}

void Environment::initPedestrians() {
    // Pedestrians walking along sidewalks
    // Highway North sidewalk (Y = 246)
    pedestrians.push_back({sf::Vector2f(80.0f, 246.0f), TravelDirection::EASTBOUND, 22.0f, sf::Color(220, 50, 50), 40.0f, 300.0f});
    pedestrians.push_back({sf::Vector2f(260.0f, 246.0f), TravelDirection::WESTBOUND, 20.0f, sf::Color(40, 100, 220), 40.0f, 300.0f});
    pedestrians.push_back({sf::Vector2f(520.0f, 246.0f), TravelDirection::EASTBOUND, 25.0f, sf::Color(230, 180, 20), 490.0f, 790.0f});
    pedestrians.push_back({sf::Vector2f(720.0f, 246.0f), TravelDirection::WESTBOUND, 24.0f, sf::Color(50, 180, 80), 490.0f, 790.0f});
    pedestrians.push_back({sf::Vector2f(1030.0f, 246.0f), TravelDirection::EASTBOUND, 23.0f, sf::Color(160, 60, 200), 990.0f, 1260.0f});

    // Highway South sidewalk (Y = 460)
    pedestrians.push_back({sf::Vector2f(100.0f, 460.0f), TravelDirection::EASTBOUND, 22.0f, sf::Color(40, 120, 200), 40.0f, 300.0f});
    pedestrians.push_back({sf::Vector2f(600.0f, 460.0f), TravelDirection::WESTBOUND, 25.0f, sf::Color(200, 80, 40), 490.0f, 790.0f});
    pedestrians.push_back({sf::Vector2f(1100.0f, 460.0f), TravelDirection::EASTBOUND, 21.0f, sf::Color(80, 180, 160), 990.0f, 1260.0f});

    // Avenue 1 sidewalk (West X = 320, East X = 478)
    pedestrians.push_back({sf::Vector2f(320.0f, 80.0f), TravelDirection::SOUTHBOUND, 20.0f, sf::Color(220, 90, 40), 55.0f, 230.0f});
    pedestrians.push_back({sf::Vector2f(478.0f, 520.0f), TravelDirection::NORTHBOUND, 22.0f, sf::Color(40, 140, 220), 475.0f, 660.0f});

    // Avenue 2 sidewalk (West X = 820, East X = 978)
    pedestrians.push_back({sf::Vector2f(820.0f, 100.0f), TravelDirection::SOUTHBOUND, 24.0f, sf::Color(180, 60, 180), 55.0f, 230.0f});
    pedestrians.push_back({sf::Vector2f(978.0f, 550.0f), TravelDirection::NORTHBOUND, 21.0f, sf::Color(60, 190, 90), 475.0f, 660.0f});
}

void Environment::update(float deltaTime) {
    beaconTimer += deltaTime;

    for (auto& ped : pedestrians) {
        if (ped.direction == TravelDirection::EASTBOUND) {
            ped.position.x += ped.speed * deltaTime;
            if (ped.position.x > ped.maxCoord) {
                ped.position.x = ped.maxCoord;
                ped.direction = TravelDirection::WESTBOUND;
            }
        } else if (ped.direction == TravelDirection::WESTBOUND) {
            ped.position.x -= ped.speed * deltaTime;
            if (ped.position.x < ped.minCoord) {
                ped.position.x = ped.minCoord;
                ped.direction = TravelDirection::EASTBOUND;
            }
        } else if (ped.direction == TravelDirection::SOUTHBOUND) {
            ped.position.y += ped.speed * deltaTime;
            if (ped.position.y > ped.maxCoord) {
                ped.position.y = ped.maxCoord;
                ped.direction = TravelDirection::NORTHBOUND;
            }
        } else if (ped.direction == TravelDirection::NORTHBOUND) {
            ped.position.y -= ped.speed * deltaTime;
            if (ped.position.y < ped.minCoord) {
                ped.position.y = ped.minCoord;
                ped.direction = TravelDirection::SOUTHBOUND;
            }
        }
    }
}

void Environment::drawSidewalksAndGreenery(sf::RenderWindow& window) const {
    // 1. Manicured Lawn Grass Base with Alternating Mowing Stripes
    for (float y = 0.0f; y < 740.0f; y += 24.0f) {
        sf::RectangleShape lawnStripe(sf::Vector2f(1320.0f, 24.0f));
        lawnStripe.setPosition(sf::Vector2f(0.0f, y));
        int row = static_cast<int>(y / 24.0f);
        lawnStripe.setFillColor(row % 2 == 0 ? sf::Color(44, 76, 52) : sf::Color(48, 82, 56));
        window.draw(lawnStripe);
    }

    // 2. Decorative Stone Pathways Connecting Buildings to Sidewalks
    auto drawPathway = [&](float px, float py, float pw, float ph) {
        sf::RectangleShape path(sf::Vector2f(pw, ph));
        path.setPosition(sf::Vector2f(px, py));
        path.setFillColor(sf::Color(190, 195, 202));
        window.draw(path);

        sf::RectangleShape pathBorder(sf::Vector2f(pw, ph));
        pathBorder.setPosition(sf::Vector2f(px, py));
        pathBorder.setFillColor(sf::Color::Transparent);
        pathBorder.setOutlineThickness(1.0f);
        pathBorder.setOutlineColor(sf::Color(140, 145, 152));
        window.draw(pathBorder);
    };

    // Pathways from top buildings to sidewalk
    drawPathway(115.0f, 195.0f, 24.0f, 47.0f); // Hospital ER path
    drawPathway(250.0f, 195.0f, 20.0f, 47.0f); // Clinic path
    drawPathway(550.0f, 200.0f, 26.0f, 42.0f); // Cyber Tower path
    drawPathway(725.0f, 200.0f, 24.0f, 42.0f); // Bank path
    drawPathway(1050.0f, 200.0f, 24.0f, 42.0f); // Fire Dept path
    drawPathway(1190.0f, 200.0f, 20.0f, 42.0f); // Logistics path

    // Pathways from bottom buildings to sidewalk
    drawPathway(115.0f, 461.0f, 24.0f, 39.0f); // Police precinct path
    drawPathway(250.0f, 461.0f, 20.0f, 49.0f); // Court path
    drawPathway(550.0f, 461.0f, 26.0f, 34.0f); // Commerce path
    drawPathway(725.0f, 461.0f, 24.0f, 44.0f); // Hotel path
    drawPathway(1050.0f, 461.0f, 24.0f, 44.0f); // Arcade path
    drawPathway(1190.0f, 461.0f, 20.0f, 49.0f); // Residences path

    // 3. Continuous Pedestrian Sidewalks alongside roads (Concrete pavers with granite curbs)
    // Highway North Sidewalk (Y: 242 -> 258)
    sf::RectangleShape hwyNorthWalk(sf::Vector2f(1320.0f, 16.0f));
    hwyNorthWalk.setPosition(sf::Vector2f(0.0f, 242.0f));
    hwyNorthWalk.setFillColor(sf::Color(190, 195, 200));
    window.draw(hwyNorthWalk);

    sf::RectangleShape hwyNorthCurb(sf::Vector2f(1320.0f, 3.0f));
    hwyNorthCurb.setPosition(sf::Vector2f(0.0f, 257.0f));
    hwyNorthCurb.setFillColor(sf::Color(115, 120, 125));
    window.draw(hwyNorthCurb);

    // Highway South Sidewalk (Y: 445 -> 461)
    sf::RectangleShape hwySouthWalk(sf::Vector2f(1320.0f, 16.0f));
    hwySouthWalk.setPosition(sf::Vector2f(0.0f, 445.0f));
    hwySouthWalk.setFillColor(sf::Color(190, 195, 200));
    window.draw(hwySouthWalk);

    sf::RectangleShape hwySouthCurb(sf::Vector2f(1320.0f, 3.0f));
    hwySouthCurb.setPosition(sf::Vector2f(0.0f, 445.0f));
    hwySouthCurb.setFillColor(sf::Color(115, 120, 125));
    window.draw(hwySouthCurb);

    // Avenue 1 West Sidewalk (X: 314 -> 330)
    sf::RectangleShape ave1WestWalk(sf::Vector2f(16.0f, 740.0f));
    ave1WestWalk.setPosition(sf::Vector2f(314.0f, 0.0f));
    ave1WestWalk.setFillColor(sf::Color(190, 195, 200));
    window.draw(ave1WestWalk);

    // Avenue 1 East Sidewalk (X: 470 -> 486)
    sf::RectangleShape ave1EastWalk(sf::Vector2f(16.0f, 740.0f));
    ave1EastWalk.setPosition(sf::Vector2f(470.0f, 0.0f));
    ave1EastWalk.setFillColor(sf::Color(190, 195, 200));
    window.draw(ave1EastWalk);

    // Avenue 2 West Sidewalk (X: 814 -> 830)
    sf::RectangleShape ave2WestWalk(sf::Vector2f(16.0f, 740.0f));
    ave2WestWalk.setPosition(sf::Vector2f(814.0f, 0.0f));
    ave2WestWalk.setFillColor(sf::Color(190, 195, 200));
    window.draw(ave2WestWalk);

    // Avenue 2 East Sidewalk (X: 970 -> 986)
    sf::RectangleShape ave2EastWalk(sf::Vector2f(16.0f, 740.0f));
    ave2EastWalk.setPosition(sf::Vector2f(970.0f, 0.0f));
    ave2EastWalk.setFillColor(sf::Color(190, 195, 200));
    window.draw(ave2EastWalk);

    // Sidewalk Pavement Expansion Grooves
    for (float x = 0.0f; x < 1320.0f; x += 22.0f) {
        sf::RectangleShape groove(sf::Vector2f(1.0f, 16.0f));
        groove.setFillColor(sf::Color(160, 165, 170));
        groove.setPosition(sf::Vector2f(x, 242.0f));
        window.draw(groove);
        groove.setPosition(sf::Vector2f(x, 445.0f));
        window.draw(groove);
    }

    // 4. Tactile Yellow Blister Warning Pads at Crosswalk Ramps (High realism ADA standard)
    auto drawTactilePad = [&](float x, float y, float w, float h) {
        sf::RectangleShape pad(sf::Vector2f(w, h));
        pad.setPosition(sf::Vector2f(x, y));
        pad.setFillColor(sf::Color(240, 205, 30));
        pad.setOutlineThickness(1.0f);
        pad.setOutlineColor(sf::Color(180, 150, 20));
        window.draw(pad);
    };

    // Highway crosswalk approaches
    drawTactilePad(310.0f, 242.0f, 4.0f, 16.0f);
    drawTactilePad(486.0f, 242.0f, 4.0f, 16.0f);
    drawTactilePad(310.0f, 445.0f, 4.0f, 16.0f);
    drawTactilePad(486.0f, 445.0f, 4.0f, 16.0f);

    drawTactilePad(810.0f, 242.0f, 4.0f, 16.0f);
    drawTactilePad(986.0f, 242.0f, 4.0f, 16.0f);
    drawTactilePad(810.0f, 445.0f, 4.0f, 16.0f);
    drawTactilePad(986.0f, 445.0f, 4.0f, 16.0f);

    // Avenue crosswalk approaches
    drawTactilePad(314.0f, 238.0f, 16.0f, 4.0f);
    drawTactilePad(470.0f, 238.0f, 16.0f, 4.0f);
    drawTactilePad(314.0f, 461.0f, 16.0f, 4.0f);
    drawTactilePad(470.0f, 461.0f, 16.0f, 4.0f);

    drawTactilePad(814.0f, 238.0f, 16.0f, 4.0f);
    drawTactilePad(970.0f, 238.0f, 16.0f, 4.0f);
    drawTactilePad(814.0f, 461.0f, 16.0f, 4.0f);
    drawTactilePad(970.0f, 461.0f, 16.0f, 4.0f);

    // 5. Ornamental Flowerbeds & Park Benches
    auto drawFlowerbed = [&](float fx, float fy, float fw, float fh) {
        sf::RectangleShape bed(sf::Vector2f(fw, fh));
        bed.setPosition(sf::Vector2f(fx, fy));
        bed.setFillColor(sf::Color(80, 50, 30)); // Dark mulch
        bed.setOutlineThickness(1.5f);
        bed.setOutlineColor(sf::Color(140, 145, 150));
        window.draw(bed);

        for (float bx = fx + 4.0f; bx < fx + fw - 4.0f; bx += 8.0f) {
            sf::CircleShape petal(2.0f);
            petal.setPosition(sf::Vector2f(bx, fy + fh / 2.0f - 2.0f));
            int pIdx = static_cast<int>(bx / 8.0f) % 3;
            if (pIdx == 0) petal.setFillColor(sf::Color(235, 40, 80));      // Red rose
            else if (pIdx == 1) petal.setFillColor(sf::Color(255, 215, 0)); // Yellow tulip
            else petal.setFillColor(sf::Color(190, 70, 230));               // Violet blossom
            window.draw(petal);
        }
    };

    drawFlowerbed(185.0f, 222.0f, 45.0f, 10.0f);
    drawFlowerbed(630.0f, 222.0f, 50.0f, 10.0f);
    drawFlowerbed(1100.0f, 222.0f, 45.0f, 10.0f);
    drawFlowerbed(185.0f, 471.0f, 45.0f, 10.0f);
    drawFlowerbed(630.0f, 471.0f, 50.0f, 10.0f);
    drawFlowerbed(1100.0f, 471.0f, 45.0f, 10.0f);

    // Modern Wood & Steel Park Benches
    auto drawBench = [&](float bx, float by) {
        sf::RectangleShape bench(sf::Vector2f(18.0f, 6.0f));
        bench.setPosition(sf::Vector2f(bx, by));
        bench.setFillColor(sf::Color(145, 95, 50));
        bench.setOutlineThickness(1.0f);
        bench.setOutlineColor(sf::Color(35, 40, 45));
        window.draw(bench);
    };
    drawBench(240.0f, 224.0f);
    drawBench(690.0f, 224.0f);
    drawBench(1155.0f, 224.0f);
    drawBench(240.0f, 473.0f);
    drawBench(690.0f, 473.0f);
    drawBench(1155.0f, 473.0f);
}

void Environment::drawBuildingsAndTrees(sf::RenderWindow& window, const sf::Font* font) const {
    bool beaconFlash = (std::fmod(beaconTimer, 1.0f) < 0.5f);

    // 1. Draw Buildings with 3D Depth, Rooftops, and Window Grids
    for (const auto& b : buildings) {
        // Multi-Layer Ambient Occlusion Drop Shadow
        sf::RectangleShape softShadow(sf::Vector2f(b.bounds.size.x + 10.0f, b.bounds.size.y + 10.0f));
        softShadow.setPosition(sf::Vector2f(b.bounds.position.x + 5.0f, b.bounds.position.y + 5.0f));
        softShadow.setFillColor(sf::Color(10, 15, 12, 90));
        window.draw(softShadow);

        sf::RectangleShape hardShadow(sf::Vector2f(b.bounds.size.x + 4.0f, b.bounds.size.y + 4.0f));
        hardShadow.setPosition(sf::Vector2f(b.bounds.position.x + 2.0f, b.bounds.position.y + 2.0f));
        hardShadow.setFillColor(sf::Color(15, 20, 18, 140));
        window.draw(hardShadow);

        // Building Facade Wall
        sf::RectangleShape wall(b.bounds.size);
        wall.setPosition(b.bounds.position);
        wall.setFillColor(b.facadeColor);
        wall.setOutlineThickness(2.0f);
        wall.setOutlineColor(sf::Color(32, 36, 42));
        window.draw(wall);

        // Parapet Coping Border (3D Roof Rim)
        sf::RectangleShape roof(sf::Vector2f(b.bounds.size.x - 12.0f, b.bounds.size.y - 12.0f));
        roof.setPosition(sf::Vector2f(b.bounds.position.x + 6.0f, b.bounds.position.y + 6.0f));
        roof.setFillColor(b.roofColor);
        roof.setOutlineThickness(1.5f);
        roof.setOutlineColor(sf::Color(65, 70, 78));
        window.draw(roof);

        // Rooftop HVAC Industrial Chillers & Ventilation Units
        auto drawHVAC = [&](float hx, float hy) {
            sf::RectangleShape hvac(sf::Vector2f(20.0f, 15.0f));
            hvac.setPosition(sf::Vector2f(hx, hy));
            hvac.setFillColor(sf::Color(115, 120, 128));
            hvac.setOutlineThickness(1.0f);
            hvac.setOutlineColor(sf::Color(45, 50, 55));
            window.draw(hvac);

            sf::CircleShape fan(4.5f);
            fan.setPosition(sf::Vector2f(hx + 5.5f, hy + 3.0f));
            fan.setFillColor(sf::Color(55, 60, 65));
            window.draw(fan);
        };
        drawHVAC(b.bounds.position.x + 14.0f, b.bounds.position.y + 14.0f);

        // High-Detail Specialized Building Architecture
        if (b.style == 0) { // METRO HOSPITAL
            // Rooftop Emergency Helicopter Landing Pad (Helipad)
            float cx = b.bounds.position.x + b.bounds.size.x / 2.0f;
            float cy = b.bounds.position.y + b.bounds.size.y / 2.0f;

            sf::CircleShape heliPad(28.0f);
            heliPad.setPosition(sf::Vector2f(cx - 28.0f, cy - 28.0f));
            heliPad.setFillColor(sf::Color(60, 65, 72));
            heliPad.setOutlineThickness(3.0f);
            heliPad.setOutlineColor(sf::Color(255, 215, 0)); // Bold yellow helipad ring
            window.draw(heliPad);

            // Bold Red 'H' in the center
            sf::RectangleShape hBar1(sf::Vector2f(6.0f, 26.0f));
            hBar1.setPosition(sf::Vector2f(cx - 11.0f, cy - 13.0f));
            hBar1.setFillColor(sf::Color(220, 25, 25));
            window.draw(hBar1);

            sf::RectangleShape hBar2(sf::Vector2f(6.0f, 26.0f));
            hBar2.setPosition(sf::Vector2f(cx + 5.0f, cy - 13.0f));
            hBar2.setFillColor(sf::Color(220, 25, 25));
            window.draw(hBar2);

            sf::RectangleShape hCross(sf::Vector2f(16.0f, 6.0f));
            hCross.setPosition(sf::Vector2f(cx - 8.0f, cy - 3.0f));
            hCross.setFillColor(sf::Color(220, 25, 25));
            window.draw(hCross);

            // ER Bay Entrance Apron with Red & White Hazard Warning Stripes
            for (float sx = b.bounds.position.x + 30.0f; sx < b.bounds.position.x + b.bounds.size.x - 30.0f; sx += 12.0f) {
                sf::RectangleShape stripe(sf::Vector2f(6.0f, 10.0f));
                stripe.setPosition(sf::Vector2f(sx, b.bounds.position.y + b.bounds.size.y - 12.0f));
                stripe.setFillColor(sf::Color(225, 30, 30));
                window.draw(stripe);
            }
        } else if (b.style == 1) { // FIRE DEPARTMENT #12
            // Fire Engine Roll-up Bay Garage Doors with Yellow Hazard Stripes
            auto drawBayDoor = [&](float bx) {
                sf::RectangleShape bay(sf::Vector2f(38.0f, 28.0f));
                bay.setPosition(sf::Vector2f(bx, b.bounds.position.y + b.bounds.size.y - 30.0f));
                bay.setFillColor(sf::Color(170, 35, 35));
                bay.setOutlineThickness(1.5f);
                bay.setOutlineColor(sf::Color(240, 205, 30));
                window.draw(bay);

                for (float by = b.bounds.position.y + b.bounds.size.y - 26.0f; by < b.bounds.position.y + b.bounds.size.y - 6.0f; by += 6.0f) {
                    sf::RectangleShape slit(sf::Vector2f(34.0f, 1.5f));
                    slit.setPosition(sf::Vector2f(bx + 2.0f, by));
                    slit.setFillColor(sf::Color(70, 15, 15));
                    window.draw(slit);
                }
            };
            drawBayDoor(b.bounds.position.x + 20.0f);
            drawBayDoor(b.bounds.position.x + 75.0f);
        } else if (b.style == 2) { // POLICE PRECINCT
            // Rooftop Radio Communications Antenna Mast with Pulsating Obstruction Beacon
            float ax = b.bounds.position.x + b.bounds.size.x - 25.0f;
            float ay = b.bounds.position.y + 20.0f;

            sf::RectangleShape mast(sf::Vector2f(3.0f, 22.0f));
            mast.setPosition(sf::Vector2f(ax, ay));
            mast.setFillColor(sf::Color(180, 185, 195));
            window.draw(mast);

            sf::CircleShape beacon(4.0f);
            beacon.setPosition(sf::Vector2f(ax - 2.5f, ay - 4.0f));
            beacon.setFillColor(beaconFlash ? sf::Color(255, 30, 30) : sf::Color(100, 10, 10));
            window.draw(beacon);

            if (beaconFlash) {
                sf::CircleShape glow(9.0f);
                glow.setPosition(sf::Vector2f(ax - 7.5f, ay - 9.0f));
                glow.setFillColor(sf::Color(255, 30, 30, 60));
                window.draw(glow);
            }
        } else if (b.style == 3) { // MODERN CYBER / COMMERCE GLASS TOWERS
            // Glass Curtain Wall Diagonal Sunlight Reflection
            sf::ConvexShape skyReflection;
            skyReflection.setPointCount(4);
            skyReflection.setPoint(0, sf::Vector2f(b.bounds.position.x + 35.0f, b.bounds.position.y + 10.0f));
            skyReflection.setPoint(1, sf::Vector2f(b.bounds.position.x + 75.0f, b.bounds.position.y + 10.0f));
            skyReflection.setPoint(2, sf::Vector2f(b.bounds.position.x + 40.0f, b.bounds.position.y + b.bounds.size.y - 15.0f));
            skyReflection.setPoint(3, sf::Vector2f(b.bounds.position.x, b.bounds.position.y + b.bounds.size.y - 15.0f));
            skyReflection.setFillColor(sf::Color(255, 255, 255, 35));
            window.draw(skyReflection);
        }

        // Window Grids on building roof/facade
        for (float wy = b.bounds.position.y + 40.0f; wy < b.bounds.position.y + b.bounds.size.y - 25.0f; wy += 22.0f) {
            for (float wx = b.bounds.position.x + 22.0f; wx < b.bounds.position.x + b.bounds.size.x - 22.0f; wx += 24.0f) {
                sf::RectangleShape win(sf::Vector2f(12.0f, 10.0f));
                win.setPosition(sf::Vector2f(wx, wy));
                if (b.style == 3) {
                    win.setFillColor(sf::Color(130, 210, 245, 220)); // Cyan glass
                } else if (b.style == 0) {
                    win.setFillColor(sf::Color(220, 240, 255, 220)); // Hospital clean glass
                } else {
                    win.setFillColor(sf::Color(255, 235, 150, 210)); // Warm office yellow
                }
                win.setOutlineThickness(1.0f);
                win.setOutlineColor(sf::Color(40, 45, 50));
                window.draw(win);
            }
        }

        // Building Signage Plaque
        if (font != nullptr) {
            sf::RectangleShape signPlate(sf::Vector2f(b.bounds.size.x - 20.0f, 16.0f));
            signPlate.setPosition(sf::Vector2f(b.bounds.position.x + 10.0f, b.bounds.position.y + b.bounds.size.y - 20.0f));
            signPlate.setFillColor(sf::Color(22, 26, 32, 235));
            signPlate.setOutlineThickness(1.0f);
            signPlate.setOutlineColor(sf::Color(55, 62, 72));
            window.draw(signPlate);

            sf::Text title(*font, b.name, 9);
            title.setStyle(sf::Text::Bold);
            title.setFillColor(b.nameColor);
            title.setPosition(sf::Vector2f(b.bounds.position.x + 14.0f, b.bounds.position.y + b.bounds.size.y - 19.0f));
            window.draw(title);
        }
    }

    // 2. Multi-Tier Organic Leafy Trees with Drop Shadows
    for (const auto& t : trees) {
        // Shadow
        sf::CircleShape shadow(t.radius * 1.1f);
        shadow.setPosition(sf::Vector2f(t.position.x - t.radius + 3.0f, t.position.y - t.radius + 3.0f));
        shadow.setFillColor(sf::Color(12, 22, 14, 130));
        window.draw(shadow);

        // Dark Outer Green Foliage
        sf::CircleShape outerCanopy(t.radius);
        outerCanopy.setPosition(sf::Vector2f(t.position.x - t.radius, t.position.y - t.radius));
        outerCanopy.setFillColor(sf::Color(28, 95, 42));
        window.draw(outerCanopy);

        // Mid-Tone Leaf Cluster
        sf::CircleShape midFoliage(t.radius * 0.82f);
        midFoliage.setPosition(sf::Vector2f(t.position.x - t.radius * 0.82f + 1.0f, t.position.y - t.radius * 0.82f - 1.0f));
        midFoliage.setFillColor(sf::Color(45, 140, 58));
        window.draw(midFoliage);

        // Bright Leafy Crown
        sf::CircleShape innerCrown(t.radius * 0.58f);
        innerCrown.setPosition(sf::Vector2f(t.position.x - t.radius * 0.58f - 1.0f, t.position.y - t.radius * 0.58f - 2.0f));
        innerCrown.setFillColor(sf::Color(70, 180, 80));
        window.draw(innerCrown);

        // Trunk Core
        sf::CircleShape trunk(t.radius * 0.22f);
        trunk.setPosition(sf::Vector2f(t.position.x - t.radius * 0.22f, t.position.y - t.radius * 0.22f));
        trunk.setFillColor(sf::Color(110, 70, 38));
        window.draw(trunk);
    }

    // 3. Modern LED Street Lampposts with Warm Circular Ambient Ground Lighting
    auto drawLamppost = [&](float lx, float ly) {
        // Warm circular ground lighting pool
        sf::CircleShape glow(22.0f);
        glow.setPosition(sf::Vector2f(lx - 22.0f, ly - 22.0f));
        glow.setFillColor(sf::Color(255, 240, 180, 28));
        window.draw(glow);

        // Steel base
        sf::CircleShape base(3.0f);
        base.setPosition(sf::Vector2f(lx - 3.0f, ly - 3.0f));
        base.setFillColor(sf::Color(40, 45, 50));
        window.draw(base);

        // Center luminaire bulb
        sf::CircleShape bulb(1.8f);
        bulb.setPosition(sf::Vector2f(lx - 1.8f, ly - 1.8f));
        bulb.setFillColor(sf::Color(255, 250, 210));
        window.draw(bulb);
    };

    // Lampposts along highway sidewalks
    for (float x = 40.0f; x < 1320.0f; x += 110.0f) {
        if ((x >= 350.0f && x <= 460.0f) || (x >= 850.0f && x <= 960.0f)) continue; // Skip intersection boxes
        drawLamppost(x, 240.0f);
        drawLamppost(x, 462.0f);
    }

    // 4. Red Fire Hydrants on Sidewalk Corners
    auto drawHydrant = [&](float hx, float hy) {
        sf::CircleShape body(3.0f);
        body.setPosition(sf::Vector2f(hx - 3.0f, hy - 3.0f));
        body.setFillColor(sf::Color(220, 25, 25));
        body.setOutlineThickness(1.0f);
        body.setOutlineColor(sf::Color(40, 10, 10));
        window.draw(body);

        sf::CircleShape cap(1.5f);
        cap.setPosition(sf::Vector2f(hx - 1.5f, hy - 1.5f));
        cap.setFillColor(sf::Color(235, 235, 240));
        window.draw(cap);
    };
    drawHydrant(355.0f, 244.0f);
    drawHydrant(458.0f, 244.0f);
    drawHydrant(855.0f, 244.0f);
    drawHydrant(958.0f, 244.0f);
}

void Environment::drawPedestrians(sf::RenderWindow& window) const {
    for (const auto& ped : pedestrians) {
        float walkCycle = (ped.direction == TravelDirection::EASTBOUND || ped.direction == TravelDirection::WESTBOUND)
                          ? std::sin(ped.position.x * 0.25f)
                          : std::sin(ped.position.y * 0.25f);

        // Drop Shadow
        sf::CircleShape shadow(3.0f);
        shadow.setPosition(sf::Vector2f(ped.position.x - 2.0f, ped.position.y + 4.0f));
        shadow.setFillColor(sf::Color(15, 20, 18, 90));
        window.draw(shadow);

        // Legs in Stride
        sf::RectangleShape legLeft(sf::Vector2f(1.8f, 3.5f));
        legLeft.setPosition(sf::Vector2f(ped.position.x - 2.0f, ped.position.y + 4.0f + walkCycle * 1.5f));
        legLeft.setFillColor(sf::Color(35, 45, 60)); // Denim blue
        window.draw(legLeft);

        sf::RectangleShape legRight(sf::Vector2f(1.8f, 3.5f));
        legRight.setPosition(sf::Vector2f(ped.position.x + 0.5f, ped.position.y + 4.0f - walkCycle * 1.5f));
        legRight.setFillColor(sf::Color(35, 45, 60));
        window.draw(legRight);

        // Torso / Shirt
        sf::RectangleShape torso(sf::Vector2f(5.5f, 5.0f));
        torso.setPosition(sf::Vector2f(ped.position.x - 2.75f, ped.position.y));
        torso.setFillColor(ped.shirtColor);
        torso.setOutlineThickness(0.5f);
        torso.setOutlineColor(sf::Color(25, 30, 35));
        window.draw(torso);

        // Head with Hair/Cap
        sf::CircleShape head(2.6f);
        head.setPosition(sf::Vector2f(ped.position.x - 2.6f, ped.position.y - 3.8f));
        head.setFillColor(sf::Color(245, 205, 165)); // Skin tone
        window.draw(head);

        sf::CircleShape hair(2.2f);
        hair.setPosition(sf::Vector2f(ped.position.x - 2.2f, ped.position.y - 4.2f));
        hair.setFillColor(sf::Color(45, 30, 20)); // Brown/dark hair
        window.draw(hair);
    }
}

