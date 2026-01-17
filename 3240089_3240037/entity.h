#pragma once
#include "node.h"
#include <string>
#include <vector>

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

    // For sickness spell
    bool is_sick;
    int original_max_health;

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
    bool isSick() const { return is_sick; }
    int getOriginalMaxHealth() const { return original_max_health; }

    // Setters
    void setHealth(int h) { health = h; }
    void setSide(Side s);
    void setSelected(bool s) { selected = s; }
    void setSick(bool sick) { is_sick = sick; }
    void setMaxHealth(int mh) { max_health = mh; }

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

// Forward declaration for Troop
class Troop;

class Tower : public Entity {
private:
    static const float TOWER_SIZE;
    static const int TOWER_MAX_HEALTH;
    static const float TOWER_HEALING_SPEED;
    static const float TOWER_DEFENSE;
    static const int TOWER_ATTACKING_SPEED;
    static const float TOWER_ATTACK_RADIUS;
    static const int TOWER_DAMAGE;
    static const float TOWER_ATTACK_INTERVAL_BASE;

    float attack_timer;
    float attack_interval; // Changes with upgrades

public:
    Tower(float x, float y, int health, Side side);
    void draw() override;
    void update(float dt) override;
    std::string getType() const override { return "Tower"; }

    // Override virtual functions
    bool canUpgrade() const override;
    void performUpgrade() override;
    int getUpgradeCost() const override;

    // NEW: Tower attacks nearby troops
    struct AttackEffect {
        float start_x, start_y;
        float end_x, end_y;
        float duration;
        float elapsed;
        float r, g, b;
    };

    // Forward declaration only - implementation in .cpp
    void attackNearbyTroops(std::vector<Troop*>& troops, std::vector<AttackEffect>& effects);

    // Getter for attack radius (for drawing range circle)
    float getAttackRadius() const { return TOWER_ATTACK_RADIUS; }
};

class Wizard : public Entity {
private:
    static const float WIZARD_SIZE;
    static const int WIZARD_MAX_HEALTH;
    static const float WIZARD_HEALING_SPEED;
    static const float WIZARD_DEFENSE;
    static const int WIZARD_ATTACKING_SPEED;
    static const float SPELL_COOLDOWN;
    static const int FREEZE_COST;
    static const int SICKNESS_COST;
    static const int CONVERT_COST;

    float spell_cooldowns[3]; // 0=Freeze, 1=Sickness, 2=Convert

public:
    Wizard(float x, float y, int health, Side side);
    void draw() override;
    void update(float dt) override;
    std::string getType() const override { return "Wizard"; }

    // Override virtual functions
    bool canUpgrade() const override;
    void performUpgrade() override;
    int getUpgradeCost() const override;

    // Spell system
    bool canCastSpell(int spell_id) const;
    void castSpell(int spell_id, Entity* target);
    float getSpellCooldown(int spell_id) const { return spell_cooldowns[spell_id]; }
    int getSpellCost(int spell_id) const;
};