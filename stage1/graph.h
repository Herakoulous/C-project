#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <memory>

struct PathData {
    std::vector<std::pair<float, float>> waypoints;
};

class EntityGraph {
private:
    std::map<Entity*, std::vector<Entity*>> connections;
    std::map<std::pair<Entity*, Entity*>, PathData> paths;

public:
    void addEdge(Entity* from, Entity* to);
    void calculatePaths(const std::vector<std::unique_ptr<Entity>>& entities);
    const PathData* getPath(Entity* from, Entity* to) const;
    void clear();
    void draw() const;

    const std::vector<Entity*>& getConnections(Entity* entity) const {
        static const std::vector<Entity*> empty;
        auto it = connections.find(entity);
        if (it != connections.end()) {
            return it->second;
        }
        return empty;
    }
};


