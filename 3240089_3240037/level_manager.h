#pragma once
#include "entity.h"
#include <vector>
#include <memory>

// Forward declaration για το EntityGraph
class EntityGraph;

class LevelManager {
private:
    int current_level;
    int max_levels;

public:
    LevelManager();

    // Level management
    void initLevel(int level, std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph);
    bool hasNextLevel() const;
    int getCurrentLevel() const;
    int getMaxLevels() const;

private:
    // Helper functions for different levels
    void initLevel1(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph);
    void initLevel2(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph);
};