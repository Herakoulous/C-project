#pragma once
#include "node.h"
#include "entity.h"
#include <vector>

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
};