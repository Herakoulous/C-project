#pragma once
#include "node.h"
#include <string>

enum class Side { PLAYER, ENEMY, NEUTRAL };

class Entity : public Node {
protected:
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

public:
    Entity(float x, float y, int health, Side side);

    void update(float dt) override;
    void draw() override;
    bool contains(float mx, float my) const override;

    // VIRTUAL FUNCTIONS for polimorphism 
    virtual void onClick() {}
    virtual bool canUpgrade() const { return false; }
    virtual void performUpgrade() {}
    virtual int getUpgradeCost() const { return 0; }
    virtual std::string getType() const = 0;  // Pure virtual

    // Getters
    int getHealth() const { return health; }
    Side getSide() const { return side; }
    float getSize() const { return size; }
    int getLevel() const { return level; }
    int getMaxHealth() const { return max_health; }
    float getHealingSpeed() const { return healing_speed; }
    int getAttackingSpeed() const { return attacking_speed; }
    float getDefense() const { return defense; }
    bool isSelected() const { return selected; }

    // Setters
    void setHealth(int h) { health = h; }
    void setSide(Side s) { side = s; }
    void setSelected(bool s) { selected = s; }

    virtual ~Entity() = default;
};

class Baby : public Entity {
private:
    static const float BABY_SIZE;
    static const int BABY_MAX_HEALTH;
    static const float BABY_HEALING_SPEED;
    static const int BABY_ATTACKING_SPEED;

public:
    Baby(float x, float y, int health, Side side);
    void draw() override;
    std::string getType() const override { return "Baby"; }
};

class Warrior : public Entity {
private:
    static const float WARRIOR_SIZE;
    static const int WARRIOR_MAX_HEALTH;
    static const float WARRIOR_HEALING_SPEED;
    static const int WARRIOR_ATTACKING_SPEED;

public:
    Warrior(float x, float y, int health, Side side);
    void draw() override;
    std::string getType() const override { return "Warrior"; }

    // Override virtual functions
    bool canUpgrade() const override;
    void performUpgrade() override;
    int getUpgradeCost() const override;
};

class Tower : public Entity {
private:
    static const float TOWER_SIZE;
    static const int TOWER_MAX_HEALTH;
    static const float TOWER_HEALING_SPEED;
    static const float TOWER_DEFENSE;
    static const int TOWER_ATTACKING_SPEED;

public:
    Tower(float x, float y, int health, Side side);
    void draw() override;
    std::string getType() const override { return "Tower"; }

    // Override virtual functions
    bool canUpgrade() const override;
    void performUpgrade() override;
    int getUpgradeCost() const override;
};

class Wizard : public Entity {
private:
    static const float WIZARD_SIZE;
    static const int WIZARD_MAX_HEALTH;
    static const float WIZARD_HEALING_SPEED;
    static const float WIZARD_DEFENSE;
    static const int WIZARD_ATTACKING_SPEED;

public:
    Wizard(float x, float y, int health, Side side);
    void draw() override;
    std::string getType() const override { return "Wizard"; }

    // Override virtual functions
    bool canUpgrade() const override;
    void performUpgrade() override;
    int getUpgradeCost() const override;
};