#include "entity.h"
#include "troop.h"
#include "sgg/graphics.h"
#include <string>
#include <cmath>

// Static constants initialization
const float Baby::BABY_SIZE = 0.4f;
const int Baby::BABY_MAX_HEALTH = 10;
const float Baby::BABY_HEALING_SPEED = 0.3f;
const int Baby::BABY_ATTACKING_SPEED = 50;

const float Warrior::WARRIOR_SIZE = 0.5f;
const int Warrior::WARRIOR_MAX_HEALTH = 100;
const float Warrior::WARRIOR_HEALING_SPEED = 10.0f;
const int Warrior::WARRIOR_ATTACKING_SPEED = 50;

const float Tower::TOWER_SIZE = 1.0f;
const int Tower::TOWER_MAX_HEALTH = 20;
const float Tower::TOWER_HEALING_SPEED = 0.0f;
const float Tower::TOWER_DEFENSE = 2.0f;
const int Tower::TOWER_ATTACKING_SPEED = 30;
const float Tower::TOWER_ATTACK_RADIUS = 2.0f;
const int Tower::TOWER_DAMAGE = 2;
const float Tower::TOWER_ATTACK_INTERVAL_BASE = 0.5f;

const float Wizard::WIZARD_SIZE = 1.2f;
const int Wizard::WIZARD_MAX_HEALTH = 50;
const float Wizard::WIZARD_HEALING_SPEED = 0.0f;
const float Wizard::WIZARD_DEFENSE = 1.0f;
const int Wizard::WIZARD_ATTACKING_SPEED = 75;
const float Wizard::SPELL_COOLDOWN = 10.0f;
const int Wizard::FREEZE_COST = 25;
const int Wizard::SICKNESS_COST = 30;
const int Wizard::CONVERT_COST = 50;

// Entity implementation
Entity::Entity(float x, float y, int health, Side side)
    : Node(x, y), health(health), side(side), timer(1.0f),
    size(0.0f), selected(false), max_health(0), level(0),
    healing_speed(0.0f), defense(0.0f), attacking_speed(0),
    is_sick(false), original_max_health(0) {
}

bool Entity::contains(float mx, float my) const {
    float dx = mx - x;
    float dy = my - y;
    return (dx * dx + dy * dy) <= (size * size);
}

void Entity::update(float dt) {
    float dt_seconds = dt / 1000.0f;

    // Sickness effect - lose HP over time
    if (is_sick && side != Side::NEUTRAL) {
        timer += dt_seconds;
        if (timer >= 1.0f) {
            health--;
            timer -= 1.0f;
            if (health <= 0) {
                health = 0;
            }
        }
    }
    else if (side != Side::NEUTRAL) {
        if (health < max_health && healing_speed > 0) {
            timer += dt_seconds;
            float hp_should_add = healing_speed * timer;
            int hp_to_add = (int)hp_should_add;

            if (hp_to_add > 0) {
                health += hp_to_add;
                float time_used = hp_to_add / healing_speed;
                timer -= time_used;

                if (health > max_health) {
                    health = max_health;
                    timer = 0.0f;
                }
            }
        }
    }

    if (health > max_health && !is_sick) {
        timer += dt_seconds;
        if (timer >= 1.0f) {
            health--;
            timer -= 1.0f;
        }
    }
}

void Entity::setSide(Side s) {
    side = s;
    // Cure sickness when side changes
    if (is_sick && side == Side::NEUTRAL) {
        is_sick = false;
        max_health = original_max_health;
    }
}

void Entity::draw() {}

// Baby
Baby::Baby(float x, float y, int health, Side side)
    : Entity(x, y, health, side) {
    size = BABY_SIZE;
    max_health = BABY_MAX_HEALTH;
    original_max_health = BABY_MAX_HEALTH;
    healing_speed = BABY_HEALING_SPEED;
    attacking_speed = BABY_ATTACKING_SPEED;
    level = 0;
    defense = 1.0f;
}

void Baby::draw() {
    graphics::Brush br;
    if (side == Side::ENEMY) {
        br.fill_color[0] = 1.0f; br.fill_color[1] = 0.1f; br.fill_color[2] = 0.1f;
    }
    else if (side == Side::PLAYER) {
        br.fill_color[0] = 0.1f; br.fill_color[1] = 0.1f; br.fill_color[2] = 1.0f;
    }
    else {
        br.fill_color[0] = 0.8f; br.fill_color[1] = 0.8f; br.fill_color[2] = 0.8f;
    }
    graphics::drawDisk(x, y, size, br);

    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
    graphics::drawText(x - size / 2, y + size / 2, size / 2, std::to_string(health), br_text);
}

Warrior::Warrior(float x, float y, int health, Side side)
    : Entity(x, y, health, side) {
    size = WARRIOR_SIZE;
    max_health = WARRIOR_MAX_HEALTH;
    original_max_health = WARRIOR_MAX_HEALTH;
    healing_speed = WARRIOR_HEALING_SPEED;
    attacking_speed = WARRIOR_ATTACKING_SPEED;
    level = 1;
    defense = 1;
}

void Warrior::draw() {
    graphics::Brush br;
    if (side == Side::ENEMY) {
        br.fill_color[0] = 1.0f; br.fill_color[1] = 0.1f; br.fill_color[2] = 0.1f;
    }
    else if (side == Side::PLAYER) {
        br.fill_color[0] = 0.1f; br.fill_color[1] = 0.1f; br.fill_color[2] = 1.0f;
    }
    else {
        br.fill_color[0] = 0.8f; br.fill_color[1] = 0.8f; br.fill_color[2] = 0.8f;
    }
    graphics::drawDisk(x, y, size, br);

    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
    graphics::drawText(x - size / 2, y + size / 2, size / 2, std::to_string(health), br_text);
}

bool Warrior::canUpgrade() const {
    if (level >= 5) return false;

    switch (level) {
    case 1: return health >= 5;
    case 2: return health >= 10;
    case 3: return health >= 20;
    case 4: return health >= 30;
    default: return false;
    }
}

int Warrior::getUpgradeCost() const {
    if (level >= 5) return 0;

    switch (level) {
    case 1: return 5;
    case 2: return 10;
    case 3: return 20;
    case 4: return 30;
    default: return 0;
    }
}

void Warrior::performUpgrade() {
    if (!canUpgrade()) return;

    switch (level) {
    case 1:
        if (health >= 5) {
            health -= 5;
            max_health += 10;
            healing_speed += 0.4;
            level = 2;
        }
        break;
    case 2:
        if (health >= 10) {
            health -= 10;
            max_health += 10;
            healing_speed += 0.4;
            level = 3;
        }
        break;
    case 3:
        if (health >= 20) {
            health -= 20;
            max_health += 10;
            healing_speed += 0.4;
            level = 4;
        }
        break;
    case 4:
        if (health >= 30) {
            health -= 30;
            max_health += 10;
            healing_speed += 0.4;
            level = 5;
        }
        break;
    default:
        break;
    }
}

// Tower
Tower::Tower(float x, float y, int health, Side side)
    : Entity(x, y, health, side), attack_timer(0.0f), attack_interval(TOWER_ATTACK_INTERVAL_BASE) {
    size = TOWER_SIZE;
    max_health = TOWER_MAX_HEALTH;
    original_max_health = TOWER_MAX_HEALTH;
    level = 1;
    healing_speed = TOWER_HEALING_SPEED;
    attacking_speed = TOWER_ATTACKING_SPEED;
    defense = TOWER_DEFENSE;
}

void Tower::update(float dt) {
    Entity::update(dt);

    float dt_seconds = dt / 1000.0f;
    attack_timer += dt_seconds;
}

void Tower::draw() {
    graphics::Brush br;
    if (side == Side::ENEMY) {
        br.fill_color[0] = 1.0f; br.fill_color[1] = 0.1f; br.fill_color[2] = 0.1f;
    }
    else if (side == Side::PLAYER) {
        br.fill_color[0] = 0.1f; br.fill_color[1] = 0.1f; br.fill_color[2] = 1.0f;
    }
    else {
        br.fill_color[0] = 0.8f; br.fill_color[1] = 0.8f; br.fill_color[2] = 0.8f;
    }


    // Draw transparent circle with white outline (simple circle on the circumference of the tower)
    graphics::Brush circle_br;
    circle_br.fill_color[0] = 1.0f;
    circle_br.fill_color[1] = 1.0f;
    circle_br.fill_color[2] = 1.0f;
    circle_br.fill_opacity = 0.0f; // Transparent fill
    circle_br.outline_color[0] = 1.0f; // White outline
    circle_br.outline_color[1] = 1.0f;
    circle_br.outline_color[2] = 1.0f;
    circle_br.outline_opacity = 1.0f; // Fully visible outline
    circle_br.outline_width = 2.0f; // Outline thickness

    // Draw the circle
    graphics::drawDisk(x, y, TOWER_ATTACK_RADIUS, circle_br);

    // Draw tower square
    graphics::drawRect(x, y, size, size, br);

    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
    graphics::drawText(x - size / 4, y + size / 4, size / 4, std::to_string(health), br_text);
}

void Tower::attackNearbyTroops(std::vector<Troop*>& troops, std::vector<AttackEffect>& effects) {
    if (attack_timer < attack_interval) return;

    attack_timer = 0.0f;

    // Find closest enemy troop within range
    Troop* closest_troop = nullptr;
    float closest_dist = TOWER_ATTACK_RADIUS;

    for (auto* troop : troops) {
        if (troop->hasArrived() || troop->isDead()) continue;

        // Check if troop is enemy
        if (troop->getSource()->getSide() == this->side) continue;

        float dx = troop->getX() - x;
        float dy = troop->getY() - y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= TOWER_ATTACK_RADIUS && dist < closest_dist) {
            closest_dist = dist;
            closest_troop = troop;
        }
    }

    // Attack the closest troop
    if (closest_troop) {
        closest_troop->reduceHealth(TOWER_DAMAGE);

        // Create visual effect
        AttackEffect effect;
        effect.start_x = x;
        effect.start_y = y;
        effect.end_x = closest_troop->getX();
        effect.end_y = closest_troop->getY();
        effect.duration = 0.3f;
        effect.elapsed = 0.0f;

        // Same color as tower
        if (side == Side::PLAYER) {
            effect.r = 0.1f; effect.g = 0.1f; effect.b = 1.0f;
        }
        else if (side == Side::ENEMY) {
            effect.r = 1.0f; effect.g = 0.1f; effect.b = 0.1f;
        }
        else {
            effect.r = 0.8f; effect.g = 0.8f; effect.b = 0.8f;
        }

        effects.push_back(effect);
    }
}

int Tower::getUpgradeCost() const {
    if (level >= 5) return 0;

    switch (level) {
    case 1: return 10;
    case 2: return 15;
    case 3: return 20;
    case 4: return 25;
    default: return 0;
    }
}

bool Tower::canUpgrade() const {
    if (level >= 5) return false;

    switch (level) {
    case 1: return health >= 10;
    case 2: return health >= 15;
    case 3: return health >= 20;
    case 4: return health >= 25;
    default: return false;
    }
}

void Tower::performUpgrade() {
    if (!canUpgrade()) return;

    switch (level) {
    case 1:
        if (health >= 10) {
            health -= 10;
            max_health += 20;
            original_max_health = max_health;
            defense += 0.5f;
            attack_interval -= 0.08f; // Faster attack speed
            level = 2;
        }
        break;
    case 2:
        if (health >= 15) {
            health -= 15;
            max_health += 20;
            original_max_health = max_health;
            defense += 0.5f;
            attack_interval -= 0.08f; // Faster attack speed
            level = 3;
        }
        break;
    case 3:
        if (health >= 20) {
            health -= 20;
            max_health += 20;
            original_max_health = max_health;
            defense += 0.5f;
            attack_interval -= 0.08f; // Faster attack speed
            level = 4;
        }
        break;
    case 4:
        if (health >= 25) {
            health -= 25;
            max_health += 20;
            original_max_health = max_health;
            defense += 0.5f;
            attack_interval -= 0.08f; // Faster attack speed
            level = 5;
        }
        break;
    default:
        break;
    }
}

// Wizard
Wizard::Wizard(float x, float y, int health, Side side) : Entity(x, y, health, side) {
    size = WIZARD_SIZE;
    max_health = WIZARD_MAX_HEALTH;
    original_max_health = WIZARD_MAX_HEALTH;
    level = 1;
    healing_speed = WIZARD_HEALING_SPEED;
    defense = WIZARD_DEFENSE;
    attacking_speed = WIZARD_ATTACKING_SPEED;

    // Initialize spell cooldowns
    spell_cooldowns[0] = 0.0f; // Freeze
    spell_cooldowns[1] = 0.0f; // Sickness
    spell_cooldowns[2] = 0.0f; // Convert
}

void Wizard::update(float dt) {
    Entity::update(dt);

    float dt_seconds = dt / 1000.0f;

    // Update spell cooldowns
    for (int i = 0; i < 3; i++) {
        if (spell_cooldowns[i] > 0.0f) {
            spell_cooldowns[i] -= dt_seconds;
            if (spell_cooldowns[i] < 0.0f) {
                spell_cooldowns[i] = 0.0f;
            }
        }
    }
}

void Wizard::draw() {
    graphics::Brush br;
    if (side == Side::ENEMY) {
        br.fill_color[0] = 1.0f; br.fill_color[1] = 0.1f; br.fill_color[2] = 0.1f;
    }
    else if (side == Side::PLAYER) {
        br.fill_color[0] = 0.1f; br.fill_color[1] = 0.1f; br.fill_color[2] = 1.0f;
    }
    else {
        br.fill_color[0] = 0.8f; br.fill_color[1] = 0.8f; br.fill_color[2] = 0.8f;
    }
    // Draw triangle shape for Wizard
    graphics::drawSector(x, y - 0.15f, 0, size, 240, 300, br);
    // Cover bottom to make triangle
    graphics::Brush br_cover;
    br_cover.fill_color[0] = 0.3f; 
    br_cover.fill_color[1] = 0.6f; 
    br_cover.fill_color[2] = 1.0f;
    br_cover.outline_color[0] = 0.3f;
    br_cover.outline_color[1] = 0.6f;
    br_cover.outline_color[2] = 1.0f;
    graphics::drawRect(x, y + 1.0f, size, 0.2f, br_cover);
    // Draw health text
    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
    graphics::drawText(x - size / 2 + 0.5f, y + size / 2 + 0.1f, 0.2f, std::to_string(health), br_text);
}

bool Wizard::canCastSpell(int spell_id) const {
    if (spell_id < 0 || spell_id > 2) return false;
    if (spell_cooldowns[spell_id] > 0.0f) return false;
    if (health < getSpellCost(spell_id)) return false;

    // Level requirements
    if (spell_id == 0 && level < 1) return false; // Freeze at level 1
    if (spell_id == 1 && level < 2) return false; // Sickness at level 2
    if (spell_id == 2 && level < 3) return false; // Convert at level 3

    return true;
}

int Wizard::getSpellCost(int spell_id) const {
    switch (spell_id) {
    case 0: return FREEZE_COST;
    case 1: return SICKNESS_COST;
    case 2: return CONVERT_COST;
    default: return 0;
    }
}

void Wizard::castSpell(int spell_id, Entity* target) {
    if (!canCastSpell(spell_id) || !target) return;
    if (target->getSide() == side) return; // Can't target own side

    // Pay HP cost
    health -= getSpellCost(spell_id);

    // Start cooldown
    spell_cooldowns[spell_id] = SPELL_COOLDOWN;

    // Apply spell effect
    switch (spell_id) {
    case 0: // Freeze - permanently neutral
        target->setSide(Side::NEUTRAL);
        break;

    case 1: // Sickness - max_health = 0
        if (!target->isSick()) {
            target->setSick(true);
            target->setMaxHealth(0);
        }
        break;

    case 2: // Convert - permanently player
        target->setSide(Side::PLAYER);
        break;
    }
}

int Wizard::getUpgradeCost() const {
    if (level >= 3) return 0;

    switch (level) {
    case 1: return 15;
    case 2: return 20;
    default: return 0;
    }
}

bool Wizard::canUpgrade() const {
    if (level >= 3) return false;

    switch (level) {
    case 1: return health >= 15;
    case 2: return health >= 20;
    default: return false;
    }
}

void Wizard::performUpgrade() {
    if (!canUpgrade()) return;

    switch (level) {
    case 1:
        if (health >= 15) {
            health -= 15;
            max_health += 50;
            original_max_health = max_health;
            attacking_speed += 35;
            level = 2;
        }
        break;
    case 2:
        if (health >= 20) {
            health -= 20;
            max_health += 50;
            original_max_health = max_health;
            attacking_speed += 40;
            level = 3;
        }
        break;
    default:
        break;
    }
}