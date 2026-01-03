#include "entity.h"
#include <string>

// Entity
Entity::Entity(float x, float y, int health, Side side)
    : x(x), y(y), size(0.0f), health(health), side(side),
    timer(1.0f), selected(false), max_health(0), level(0), healing_speed(0.0f), defense(0), attacking_speed(0) {
}

bool Entity::contains(float mx, float my) {
    float dx = mx - x;
    float dy = my - y;
    return (dx * dx + dy * dy) <= (size * size);
}

void Entity::onClick() {}

void Entity::draw() {}

void Entity::update(float dt) {
    float dt_seconds = dt / 1000.0f;

    if (side != Side::NEUTRAL) {
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
    if (health > max_health) {
        timer += dt_seconds;
        if (timer >= 1.0f) {
            health--;
            timer -= 1.0f;
        }
    }
}

// Baby
Baby::Baby(float x, float y, int health, Side side)
    : Entity(x, y, health, side) {
    size = BABY_SIZE;
    max_health = BABY_MAX_HEALTH;
    healing_speed = BABY_HEALING_SPEED;
    attacking_speed = BABY_ATTACKING_SPEED;
    level = 0;
    defense = 1;
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

// Warrior
Warrior::Warrior(float x, float y, int health, Side side)
    : Entity(x, y, health, side) {
    size = WARRIOR_SIZE;
    max_health = WARRIOR_MAX_HEALTH;
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

int Warrior::getUpgradeCost() {
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
    : Entity(x, y, health, side) {
    size = TOWER_SIZE;
    max_health = TOWER_MAX_HEALTH;
    level = 1;
    healing_speed = TOWER_HEALING_SPEED;
    attacking_speed = TOWER_ATTACKING_SPEED;
    defense = TOWER_DEFENSE;
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
    graphics::drawRect(x, y, size, size, br);

    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
    graphics::drawText(x - size / 4, y + size / 4, size / 4, std::to_string(health), br_text);
}

int Tower::getUpgradeCost() {
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
            defense += 0.5;
            level = 2;
        }
        break;
    case 2:
        if (health >= 15) {
            health -= 15;
            max_health += 20;
            defense += 0.5;
            level = 3;
        }
        break;
    case 3:
        if (health >= 20) {
            health -= 20;
            max_health += 20;
            defense += 0.5;
            level = 4;
        }
        break;
    case 4:
        if (health >= 25) {
            health -= 25;
            max_health += 20;
            defense += 0.5;
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
    level = 1;
    healing_speed = WIZARD_HEALING_SPEED;
    defense = WIZARD_DEFENSE;
    attacking_speed = WIZARD_ATTACKING_SPEED;
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
    br_cover.fill_color[0] = 0.0f; br_cover.fill_color[1] = 1.0f; br_cover.fill_color[2] = 0.0f;
    br_cover.outline_color[0] = 0.0f; br_cover.outline_color[1] = 1.0f; br_cover.outline_color[2] = 0.0f;
    graphics::drawRect(x, y + 1.0f, size, 0.2f, br_cover);
    // Draw health text
    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
    graphics::drawText(x - size / 2 + 0.5f, y + size / 2 + 0.1f, 0.2f, std::to_string(health), br_text);
}

int Wizard::getUpgradeCost() {
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
            healing_speed += 35;
            level = 2;
        }
        break;
    case 2:
        if (health >= 20) {
            health -= 20;
            max_health += 50;
            defense += 40;
            level = 3;
        }
        break;
    default:
        break;
    }
}