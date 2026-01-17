#pragma once
#include "node.h"
#include "entity.h" 
#include <vector>

// Forward declarations - DON'T include entity.h here!
enum class Side;
class Entity;

class Troop : public Node {
private:
    Entity* source;
    Entity* target;
    int health_amount;
    std::vector<std::pair<float, float>> waypoints;
    float progress;
    float speed;
    bool arrived;

    std::pair<float, float> interpolatePosition(float t);

public:
    Troop(Entity* src, Entity* tgt, int amount, const std::vector<std::pair<float, float>>& path);

    void update(float dt) override;
    void draw() override;
    bool contains(float mx, float my) const override;

    bool hasArrived() const { return arrived; }
    Entity* getTarget() const { return target; }
    Entity* getSource() const { return source; }
    int getHealthAmount() const { return health_amount; }

    // NEW: Reduce troop health when attacked by towers
    void reduceHealth(int amount);
    bool isDead() const { return health_amount <= 0; }
};