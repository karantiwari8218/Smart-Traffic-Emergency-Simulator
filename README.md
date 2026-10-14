# Smart Traffic & Emergency Simulator

### A C++17-based 2D simulation of urban traffic flow, traffic-signal control, and emergency vehicle priority.

The Smart Traffic & Emergency Simulator models how civilian and emergency vehicles move through a multi-lane road network with two intersections. It combines vehicle behaviour, lane management, traffic-light state transitions, and emergency-priority logic in a graphical desktop application.

The project was developed as a college Project-Based Learning (PBL) project to apply object-oriented programming, data structures, simulation logic, and graphical rendering to a practical traffic-management problem.

---

## 1. Problem Statement

Traffic congestion around intersections can make it difficult for emergency vehicles to move through ordinary traffic. Ambulances, fire engines, and police vehicles may encounter occupied lanes, slower vehicles, or traffic signals that do not favour their direction of travel.

An emergency vehicle needs more than movement speed alone to make progress through traffic. Its movement may depend on surrounding vehicles, available lanes, and the state of the next intersection.

The problem addressed by this project is how these interacting conditions can be represented and coordinated in a controlled simulation. The system models vehicle movement, lane-changing and yielding behaviour, signal phases, and emergency signal preemption to demonstrate the relationship between traffic flow and emergency priority.

The project does not claim to solve real-world congestion or provide operational emergency routing. Instead, it provides a visual environment in which traffic-control behaviour and emergency handling can be studied.

## 2. Motivation and Objectives

The motivation behind the project is to explore traffic management through software rather than treating vehicles and traffic lights as independent elements.

### The main objectives are to:

- Represent an urban road corridor with multiple lanes and intersections.
- Simulate the movement of civilian and emergency vehicles.
- Model traffic-light phases and transitions.
- Demonstrate priority handling for emergency vehicle categories.
- Implement signal preemption behaviour for emergency requests.
- Manage vehicle spawning using FIFO queues.
- Separate vehicle, lane, traffic-light, and environment responsibilities into C++ modules.
- Visualise the simulation using SFML.

These objectives connect core programming concepts with a practical scenario involving movement, coordination, and competing traffic demands.

## 3. Project Scope

The system represents a simplified urban traffic environment containing a divided highway, multiple lanes, two intersections, connecting roads, civilian vehicles, emergency vehicles, traffic signals, and surrounding environmental elements.

### The simulation focuses on the following areas:

Area| Scope
Road environment| Graphical representation of roads, lanes, and surroundings
Vehicle modelling| Vehicle properties, movement, and behaviour
Traffic flow| Movement through lanes and intersections
Lane behaviour| Lane changing, yielding, and overtaking logic
Signal management| Signal states, phase transitions, and clearance intervals
Emergency response| Vehicle-category priority and signal preemption behaviour
Vehicle spawning| FIFO queues containing vehicle blueprints
Rendering| 2D visualisation through SFML

The application is a local desktop simulation. It does not connect to real traffic lights, GPS devices, live traffic feeds, roadside sensors, or emergency-service dispatch systems.

## 4. System Overview

The application coordinates the environment, vehicles, lanes, and traffic-light controllers. Vehicle requests are prepared for spawning, active vehicles move through the road network, and traffic signals control movement at intersections.

Emergency handling introduces additional behaviour when an emergency vehicle needs priority. Depending on the implemented conditions, this can involve vehicle movement decisions as well as a temporary change in signal control.

The current simulation state is rendered in a 2D window through SFML.

### High-level workflow

flowchart TD
    A[Application Initialisation] --> B[Create Road Environment]
    B --> C[Initialise Lanes and Signals]
    C --> D[Prepare Vehicle Queues]
    D --> E[Simulation Update Loop]
    E --> F[Process Vehicle Spawning]
    F --> G[Update Vehicle Movement]
    G --> H[Evaluate Lane Behaviour]
    H --> I[Update Traffic Signals]
    I --> J{Emergency Request}
    J -- Yes --> K[Evaluate Emergency Priority]
    J -- No --> L[Continue Normal Signal Control]
    K --> M[Render Current Scene]
    L --> M
    M --> N{Window Still Open}
    N -- Yes --> E
    N -- No --> O[Terminate Application]

This diagram illustrates the overall system workflow. The exact order of individual operations should be understood from the corresponding implementation in "main.cpp" and the module source files.

## 5. System Architecture

The project uses a modular C++ structure. The main application coordinates execution, while the vehicle, lane, traffic-light, and environment modules separate the principal responsibilities.

Component| Responsibility
"main.cpp"| Application entry point and simulation coordination
"Vehicle"| Vehicle representation and movement-related behaviour
"Lane"| Lane representation and lane-related operations
"TrafficLight"| Signal states, phase transitions, and emergency signal control
"Environment"| Visual representation of the surrounding environment
"CMakeLists.txt"| Build configuration and dependency integration

### Architectural responsibilities

- Application layer: Initialises and coordinates the simulation.
- Vehicle layer: Represents vehicles and implements vehicle-related behaviour.
- Lane layer: Provides the lane structure used by traffic moving through the environment.
- Traffic-control layer: Manages signal state and emergency-related signal changes.
- Environment layer: Provides the surrounding graphical scene.
- Build layer: Defines how source files and external dependencies are assembled into the application.

### Component relationship overview

flowchart TB
    A[main.cpp] --> B[Vehicle Module]
    A --> C[Lane Module]
    A --> D[TrafficLight Module]
    A --> E[Environment Module]
    A --> F[Vehicle Spawning Queues]
    B --> G[Simulation State]
    C --> G
    D --> G
    E --> H[SFML Rendering]
    G --> H

The diagram represents the conceptual division of responsibilities, not a complete call graph of every class and function.

## 6. Functional Modules

### 6.1 Vehicle Module

Files: "src/Vehicle.cpp", "include/Vehicle.h"

The vehicle module represents vehicles participating in the simulation and contains vehicle-related properties and behaviour.

Depending on the vehicle category and implemented conditions, the movement logic supports ordinary traffic behaviour as well as emergency-related actions such as yielding and lane changing.

The module provides the vehicle-level behaviour used by the broader simulation.

### 6.2 Lane Module

Files: "src/Lane.cpp", "include/Lane.h"

The lane module represents the lanes through which vehicles travel. It provides the lane-related structure required to organise traffic within the road environment.

Lane representation is important to movement and overtaking behaviour because a vehicle's available movement depends on its current lane and the surrounding traffic conditions.

### 6.3 Traffic-Light Module

Files: "src/TrafficLight.cpp", "include/TrafficLight.h"

The traffic-light module manages signal states and the transitions between them. Signal control determines when traffic is permitted to proceed and when a phase change or clearance period is required.

The module also supports emergency-related signal priority. Signal preemption and ordinary signal cycling are distinct behaviours: normal cycling manages regular traffic flow, while preemption temporarily changes signal control in response to an emergency request.

### 6.4 Environment Module

File: "src/Environment.cpp"

The environment module implements graphical elements that form the surroundings of the simulated road network.

It contributes to the visual representation of the scene, allowing the traffic simulation to be presented as a 2D urban environment rather than as vehicle data alone.

### 6.5 Main Application and Simulation Coordination

File: "main.cpp"

The main application brings the individual modules together. It handles application initialisation and coordinates the simulation's execution, including the creation and management of relevant objects and the update and rendering process.

It is the central integration point between vehicle behaviour, lane representation, traffic-light control, and the environment.

### 6.6 Build Configuration

File: "CMakeLists.txt"

The build configuration defines the project structure, the C++ standard, the source files used by the executable, and the SFML dependency.

It keeps build-related configuration separate from the application logic and supports reproducible project configuration across compatible development environments.

## 7. Emergency Response and Traffic-Signal Priority

Emergency response is one of the principal behaviours explored by the simulation. It involves two related but different responsibilities: vehicle-level priority behaviour and intersection-level signal control.

### Vehicle-level priority

Emergency vehicle categories can receive different priority treatment from ordinary traffic. Vehicle movement logic may use this distinction when deciding how a vehicle behaves around slower traffic or when attempting to move through an occupied lane.

Lane changing and yielding behaviour are important because an emergency vehicle cannot make progress simply by being assigned a higher priority if surrounding traffic and available space are not considered.

### Signal preemption

Signal preemption refers to changing the normal signal-control sequence in response to an emergency request. The controller manages signal states and the transition to a priority state according to its implemented conditions.

#### The intended sequence is:

1. The simulation identifies an emergency-priority request.
2. The signal controller evaluates the request and the current signal state.
3. The relevant direction receives temporary priority when the preemption conditions are satisfied.
4. Signal transitions and clearance periods are managed by the controller.
5. Normal signal operation resumes according to the implemented preemption and cooldown logic.

The exact behaviour depends on the conditions and timing values defined in the source code.

Priority is not the same as routing. This project models local traffic and signal behaviour; it should not be described as a shortest-path navigation system unless such routing is implemented separately.

## 8. Data Structures and Algorithms

The project uses C++ language features and standard-library containers to represent simulation state and manage vehicle-related operations.

#### Data structures

Structure| Purpose
"std::vector"| Stores collections of objects or elements that need iteration and indexed access
"std::queue"| Maintains FIFO order for queued vehicle-spawning requests
"VehicleBlueprint"| Represents vehicle information prepared for spawning
Enumerations| Represent defined categories or states where used
Classes and objects| Encapsulate the state and behaviour of the system's main components

#### FIFO vehicle spawning

The spawning mechanism uses the First In, First Out principle.

A vehicle blueprint is placed into a queue, and the spawning process removes the earliest queued request when the applicable spawning conditions allow it.

This keeps vehicle preparation separate from the creation of active vehicles and demonstrates a standard queue-based workflow.

##### Vehicle movement and lane behaviour

The movement logic updates vehicle positions and handles lane-related behaviour according to the conditions implemented in the vehicle and lane modules.

Yielding and overtaking behaviour allow the simulation to represent interactions between emergency vehicles and surrounding traffic. The effectiveness of these behaviours depends on available space and the movement rules implemented by the application.

#### Signal state transitions

Traffic-light control is state-based. The controller moves between signal phases according to its configured timing and transition conditions.

Clearance intervals are important to represent the transition between conflicting traffic movements. Emergency preemption adds another control path that can temporarily modify the normal sequence.

#### Complexity considerations

The project uses standard data structures and simulation-update logic. Exact time complexity depends on the number of active vehicles, the collection operations performed, and the loops and checks used by each module.

No graph-based shortest-path algorithm, machine-learning model, or advanced traffic optimisation method is claimed as part of the current implementation unless it is explicitly added to the source code.

## 9. Technology Stack

Technology| Role
C++17| Core simulation logic and object-oriented design
SFML 3| 2D graphics and application window
CMake| Build configuration and dependency management
C++ Standard Library| Containers and supporting programming utilities

C++ supports the separation of application responsibilities into modules and classes. SFML provides the graphical layer required to display the road environment and moving vehicles. CMake manages the relationship between source files, build targets, and the graphics dependency.

## 10. Team Contributions

The project is developed as a four-member college PBL project. Responsibilities are divided across the application, vehicle, lane, traffic-light, environment, and build-configuration components.

Team Member| Responsibility| Assigned Files
Kriti — Member 1| Environment implementation and build configuration| "src/Environment.cpp", "CMakeLists.txt" , "readme.md"
Karan — Member 2| Main application and vehicle module| "main.cpp", "src/Vehicle.cpp", "include/Vehicle.h"
Neha — Member 3| Traffic-light control and signal management| "src/TrafficLight.cpp", "include/TrafficLight.h"
Prathmesh — Member 4| Lane representation and lane-related operations| "src/Lane.cpp", "include/Lane.h"

The modules are integrated to form one simulation. The main application coordinates execution, the vehicle and lane modules support traffic movement, the traffic-light module manages signal behaviour, and the environment and build configuration support the visual scene and application integration.

The table records the team's stated file responsibilities. It does not imply that each file was developed exclusively by one person or that integration and testing were performed independently of the rest of the team.

## 11. Limitations

The simulation represents traffic behaviour using simplified software models. Its results should be interpreted within that scope.

Current limitations include:

- Vehicle movement is a simplified representation of real driving behaviour.
- Traffic-signal behaviour is governed by implemented control rules and timing conditions.
- Emergency priority is simulated locally and is not connected to actual emergency-service systems.
- The road environment is graphical and is not a geographically accurate map.
- The application does not consume live traffic data or external road-sensor information.
- The simulation does not establish real-world safety, congestion reduction, or emergency-response performance.
- Its behaviour depends on the implemented rules and scenarios rather than on a validated real-world traffic model.

These limitations distinguish the project from a deployed intelligent transportation system.

## 12. Future Improvements

The existing design could be extended through the following improvements:

- Configurable traffic density and emergency vehicle frequency.
- On-screen indicators for signal states and vehicle counts.
- Pause, resume, and reset controls.
- Collection of waiting-time, queue-length, and vehicle-throughput statistics.
- More detailed coordination between intersections.
- Automated tests for vehicle movement, lane behaviour, and signal transitions.
- Repeatable traffic scenarios for comparing different control strategies.
- More detailed visualisation of emergency requests and signal-preemption events.

These items are proposed extensions and should not be interpreted as features already present in the current version.

## 13. Conclusion

The Smart Traffic & Emergency Simulator applies object-oriented programming, standard data structures, traffic-control logic, and 2D graphics to a simplified urban traffic scenario.

By bringing together vehicle movement, lane representation, signal-state management, FIFO-based spawning, and emergency-priority behaviour, the project provides a practical way to study the interactions between traffic and intersection control.

Its modular design establishes a foundation for further work in simulation metrics, testing, and more advanced traffic-management behaviour.

---

Developed as a college Project-Based Learning project for educational demonstration and experimentation.
