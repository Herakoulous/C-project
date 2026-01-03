#pragma once
#include "sgg/graphics.h"
#include <string>

enum class Side { PLAYER, ENEMY, NEUTRAL };

class Entity {
public:
    float x, y;
    int health;
    Side side;
    float timer;
    float size;
    bool selected;
    int max_health;
    float healing_speed;
    int attacking_speed;
    float defense;
    int level;

    Entity(float x, float y, int health, Side side);

    virtual bool contains(float mx, float my);
    virtual void onClick();
    virtual void draw();
    virtual void update(float dt);
    virtual ~Entity() = default;
    int getLevel() const { return level; }
    Side getSide() const { return side; }
};

class Baby : public Entity {
private:
    const float BABY_SIZE = 0.4f;
    const int BABY_MAX_HEALTH = 10;
    const float BABY_HEALING_SPEED = 0.3f;
    const int BABY_ATTACKING_SPEED = 50;

public:
    Baby(float x, float y, int health, Side side);
    void draw() override;
};

class Warrior : public Entity {
private:
    const float WARRIOR_SIZE = 0.5f;
    const int WARRIOR_MAX_HEALTH = 10;
    const float WARRIOR_HEALING_SPEED = 0.4f;
    const int WARRIOR_ATTACKING_SPEED = 50;

public:
    Warrior(float x, float y, int health, Side side);
    void draw() override;
    bool canUpgrade() const;
    void performUpgrade();
    int getUpgradeCost();
};

class Tower : public Entity {
private:
    const float TOWER_SIZE = 1.0f;
    const int TOWER_MAX_HEALTH = 20;
    const float TOWER_HEALING_SPEED = 0.0f;
    const float TOWER_DEFENSE = 2.0f;
    const int TOWER_ATTACKING_SPEED = 30;

public:
    Tower(float x, float y, int health, Side side);
    void draw() override;
    bool canUpgrade() const;
    void performUpgrade();
    int getUpgradeCost();
};

class Wizard : public Entity {
private:
    const float WIZARD_SIZE = 1.2f;
    const int WIZARD_MAX_HEALTH = 50;
    const float WIZARD_HEALING_SPEED = 0.0f;
    const float WIZARD_DEFENSE = 1.0f;
    const int WIZARD_ATTACKING_SPEED = 75;

public:
    Wizard(float x, float y, int health, Side side);
    void draw() override;
    bool canUpgrade() const;
    void performUpgrade();
    int getUpgradeCost();
};