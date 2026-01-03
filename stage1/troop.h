#pragma once
#include "entity.h"
#include "sgg/graphics.h"
#include <vector>

class Troop {
private:
    Entity* source;
    Entity* target;
    int health_amount;
    std::vector<std::pair<float, float>> waypoints;
    float progress; // 0.0 to waypoints.size()-1
    float x, y;
    float speed;
    bool arrived;

    std::pair<float, float> interpolatePosition(float t);

public:
    Troop(Entity* src, Entity* tgt, int amount, const std::vector<std::pair<float, float>>& path);
    void update(float dt);
    void draw();
    bool hasArrived() const { return arrived; }
    Entity* getTarget() const { return target; }
    Entity* getSource() const { return source; }
    int getHealthAmount() const { return health_amount; }
};