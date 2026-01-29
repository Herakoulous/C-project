#include "level_manager.h"
#include "graph.h"
#include <cmath>  // Προςθήκη για sqrt
#include <memory> // Προςθήκη για std::make_unique
#include "global_state.h" // Για πρόσβαση σε troops

LevelManager::LevelManager() : current_level(1), max_levels(3) {}

void LevelManager::initLevel(int level, std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph) {
    current_level = level;

    // Clear existing data
    entities.clear();
    graph.clear();

	// Κλήση της αντίστοιχης συνάρτησης αρχικοποίησης
    switch (level) {
    case 1:
        initLevel1(entities, graph);
        break;
    case 2:
        initLevel2(entities, graph);
        break;
    case 3:
        initLevel3(entities, graph);
        break;
    default:
        initLevel1(entities, graph);
        break; // Fallback στο level 1
    }
}

bool LevelManager::hasNextLevel() const {
    return current_level < max_levels;
}

int LevelManager::getCurrentLevel() const {
    return current_level;
}

int LevelManager::getMaxLevels() const {
    return max_levels;
}

// Level 1
void LevelManager::initLevel1(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph) {
    // Προςθήκη οντοτήτων
    entities.push_back(std::make_unique<Warrior>(3.5f, 3.0f, 200, Side::PLAYER));
    entities.push_back(std::make_unique<Baby>(1.5f, 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(3.5f, 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(1.5f, 1.5f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Tower>(16 - 6.0f, 2.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(6.0f, 8 - 2.5f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Warrior>(16 - 3.5f, 8 - 3.0f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 3.5f, 8 - 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(16 - 1.5f, 8 - 1.5f, 10, Side::NEUTRAL));

    // Δημιουργία συνδέσεων
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    graph.calculatePaths(entities);
}

// Level 2
void LevelManager::initLevel2(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph) {
    // Clear for safety
    entities.clear();

    // Προςθήκη οντοτήτων για level 2
    entities.push_back(std::make_unique<Warrior>(1.5f, 6.0f, 150, Side::PLAYER));
    entities.push_back(std::make_unique<Baby>(1.0f, 4.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(7.0f, 6.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(4.2f, 5.1f, 15, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(3.5f, 2.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(10.0f, 6.5f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Warrior>(16 - 1.5f, 8 - 6.0f, 150, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 1.0f, 8 - 4.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 7.0f, 8 - 6.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 4.2f, 8 - 5.1f, 15, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(16 - 3.5f, 8 - 2.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(16 - 10.0f, 8 - 6.5f, 10, Side::NEUTRAL));


    // Δημιουργία συνδέσεων
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    graph.calculatePaths(entities);
}


void LevelManager::initLevel3(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph) {
    // Clear for safety
    entities.clear();

    // Προςθήκη οντοτήτων για level 3
    entities.push_back(std::make_unique<Warrior>(8.0f, 7.0f, 20, Side::PLAYER));
    entities.push_back(std::make_unique<Baby>(6.5f, 6.1f, 5, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(5.0f, 4.0f, 50, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(2.0f, 5.0f, 5, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(1.0f, 1.0f, 20, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(5.0f, 2.0f, 5, Side::NEUTRAL));

    entities.push_back(std::make_unique<Baby>(16 - 6.5f, 6.1f, 5, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 5.0f, 4.0f, 50, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 2.0f, 5.0f, 5, Side::NEUTRAL));
    entities.push_back(std::make_unique<Warrior>(16 - 1.0f, 1.0f, 20, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 5.0f, 2.0f, 5, Side::NEUTRAL));
    
    // Δημιουργία συνδέσεων
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    graph.calculatePaths(entities);
}