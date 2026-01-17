#include "ai_system.h"
#include "global_state.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cfloat>
#include <random>
#include <map>
#include "sgg/graphics.h"

// Random number generator
std::random_device rd;
std::mt19937 gen(rd());

AISystem::AISystem(GlobalState* gs, EntityGraph* graph)
    : global_state(gs), entity_graph(graph),
    decision_timer(0.0f), decision_interval(2.0f),
    strategy(AIStrategy::ADAPTIVE), aggression_level(0.5f) {
}

void AISystem::init() {
    decision_timer = 0.0f;
    aggression_level = 0.5f;
    strategy = AIStrategy::ADAPTIVE;
}

void AISystem::update(float dt) {
    float dt_seconds = dt / 1000.0f;
    decision_timer += dt_seconds;

    if (decision_timer >= decision_interval) {
        analyzeGameState();
        makeStrategicDecisions();
        decision_timer = 0.0f;
        decision_interval = 1.5f + ((rand() % 1000) / 1000.0f); // 1.5-2.5 seconds
    }

    // Update enemy spells (wizard AI)
    castEnemySpells();
}

void AISystem::analyzeGameState() {
    auto player_entities = getPlayerEntities();
    auto enemy_entities = getEnemyEntities();
    auto neutral_entities = getNeutralEntities();

    // Μετατροπή size_t σε int για αποφυγή warning
    int player_count = static_cast<int>(player_entities.size());
    int enemy_count = static_cast<int>(enemy_entities.size());
    int neutral_count = static_cast<int>(neutral_entities.size());

    // Συνάρτηση για να βρούμε πόσο δυνατός είναι κάθε παίκτης
    int player_power = calculateTotalPower(player_entities);
    int enemy_power = calculateTotalPower(enemy_entities);

    // Απλή λογική επιλογής στρατηγικής
    if (player_count > enemy_count + 2 || player_power > enemy_power * 2) {
        // Ο παίκτης είναι πολύ δυνατός - αμυνόμαστε
        strategy = AIStrategy::DEFENSIVE;
        aggression_level = 0.3f;
    }
    else if (neutral_count > player_count && neutral_count > enemy_count) {
        // Υπάρχουν πολλά ουδέτερα - επεκτεινόμαστε
        strategy = AIStrategy::EXPANSIVE;
        aggression_level = 0.4f;
    }
    else if (enemy_count > player_count && enemy_power > player_power) {
        // Είμαστε δυνατοί - επιτιθόμαστε
        strategy = AIStrategy::AGGRESSIVE;
        aggression_level = 0.7f;
    }
    else {
        // Ισορροπία - μικτή στρατηγική
        strategy = AIStrategy::MIXED;
        aggression_level = 0.5f;
    }
}

int AISystem::calculateTotalPower(const std::vector<Entity*>& entities) {
    int total_power = 0;
    for (auto entity : entities) {
        // Βασική δύναμη = ζωή
        int power = entity->getHealth();

        // Πολλαπλασιάζουμε με βάση τον τύπο
        std::string type = entity->getType();
        if (type == "Wizard") power *= 3;
        else if (type == "Tower") power *= 2;
        else if (type == "Warrior") power *= 2;

        // Προσθέτουμε επιπλέον βάρος για level
        power = static_cast<int>(power * (1.0f + entity->getLevel() * 0.2f));

        total_power += power;
    }
    return total_power;
}

void AISystem::makeStrategicDecisions() {
    auto enemy_entities = getEnemyEntities();

    // Για κάθε entity του AI, παίρνουμε απόφαση
    for (auto entity : enemy_entities) {
        std::string type = entity->getType();
        int health = entity->getHealth();

        // 1. Πάντα δοκιμάζουμε να αναβαθμίσουμε πρώτα (αν έχουμε αρκετή ζωή)
        if (health > entity->getMaxHealth() * 0.7f && entity->canUpgrade()) {
            if (rand() % 100 < 50) { // 50% πιθανότητα αναβάθμισης (ακόμα πιο συχνά)
                entity->performUpgrade();
                continue; // Συνέχεια με επόμενο entity
            }
        }

        // 2. Ειδική λογική για BABY: Προτεραιότητα στην αναβάθμιση
        if (type == "Baby" && entity->canUpgrade()) {
            if (rand() % 100 < 70) { // 70% πιθανότητα το Baby να αναβαθμιστεί
                entity->performUpgrade();
                continue;
            }
        }

        // 3. Λαμβάνουμε απόφαση ανάλογα με τη στρατηγική
        int decision_roll = rand() % 100;

        switch (strategy) {
        case AIStrategy::AGGRESSIVE:
            if (type == "Warrior") {
                if (decision_roll < 80) attackWeakestPlayerTarget(entity);
                else if (decision_roll < 95) attackStrategicTarget(entity);
            }
            else if (type == "Baby") {
                if (decision_roll < 60) convertNearestNeutral(entity);
                else if (decision_roll < 90 && entity->canUpgrade()) {
                    entity->performUpgrade(); // Αναβάθμιση Baby
                }
            }
            break;

        case AIStrategy::DEFENSIVE:
            if (type == "Warrior") {
                if (decision_roll < 50) attackWeakestPlayerTarget(entity);
                else if (decision_roll < 70) convertNearestNeutral(entity);
            }
            else if (type == "Baby") {
                if (decision_roll < 40) convertNearestNeutral(entity);
                else if (decision_roll < 80 && entity->canUpgrade()) {
                    entity->performUpgrade(); // Αναβάθμιση Baby
                }
            }
            break;

        case AIStrategy::EXPANSIVE:
            if (type == "Warrior" || type == "Baby") {
                if (decision_roll < 80) convertNearestNeutral(entity);
                else if (decision_roll < 95) attackWeakestPlayerTarget(entity);
            }
            break;

        case AIStrategy::MIXED:
            if (type == "Warrior") {
                if (decision_roll < 60) attackWeakestPlayerTarget(entity);
                else if (decision_roll < 85) convertNearestNeutral(entity);
                else attackStrategicTarget(entity);
            }
            else if (type == "Baby") {
                if (decision_roll < 50) convertNearestNeutral(entity);
                else if (decision_roll < 85 && entity->canUpgrade()) {
                    entity->performUpgrade(); // Αναβάθμιση Baby
                }
            }
            break;

        case AIStrategy::ADAPTIVE:
        default:
            if (type == "Warrior") {
                if (decision_roll < 70) attackWeakestPlayerTarget(entity);
                else if (decision_roll < 90) convertNearestNeutral(entity);
                else attackStrategicTarget(entity);
            }
            else if (type == "Baby") {
                if (decision_roll < 60) convertNearestNeutral(entity);
                else if (decision_roll < 90 && entity->canUpgrade()) {
                    entity->performUpgrade(); // Αναβάθμιση Baby
                }
            }
            break;
        }
    }
}

// Στην castEnemySpells - να ρίχνουν spells πιο συχνά:
void AISystem::castEnemySpells() {
    auto enemy_entities = getEnemyEntities();

    for (auto entity : enemy_entities) {
        if (entity->getType() == "Wizard") {
            Wizard* wizard = static_cast<Wizard*>(entity);

            // 5% πιθανότητα ανά frame να ρίξει spell (από 2%)
            if (rand() % 100 < 5) {
                castSmartSpell(wizard);
            }
        }
    }
}

// Καλύτερη λογική για τα spells:
void AISystem::castSmartSpell(Wizard* wizard) {
    if (!wizard) return;

    auto player_entities = getPlayerEntities();
    if (player_entities.empty()) return;

    // Επιλογή spell με βάση τη στρατηγική
    int spell_to_cast = -1;

    switch (strategy) {
    case AIStrategy::AGGRESSIVE:
        spell_to_cast = 1; // Sickness - για επιθετικότητα
        break;
    case AIStrategy::DEFENSIVE:
        spell_to_cast = 0; // Freeze - για άμυνα
        break;
    case AIStrategy::EXPANSIVE:
        spell_to_cast = 2; // Convert - για επέκταση
        break;
    default:
        // Τυχαία επιλογή
        spell_to_cast = rand() % 3;
        break;
    }

    if (!wizard->canCastSpell(spell_to_cast)) {
        // Αν δεν μπορεί να ρίξει το επιλεγμένο, δοκιμάζουμε τα υπόλοιπα
        for (int i = 0; i < 3; i++) {
            if (wizard->canCastSpell(i)) {
                spell_to_cast = i;
                break;
            }
        }
    }

    if (spell_to_cast >= 0 && wizard->canCastSpell(spell_to_cast)) {
        // Βρίσκουμε τον καλύτερο στόχο για το spell
        Entity* best_target = nullptr;
        float best_value = -1.0f;

        for (auto target : player_entities) {
            float value = evaluateSpellValue(wizard, spell_to_cast, target);
            if (value > best_value) {
                best_value = value;
                best_target = target;
            }
        }

        if (best_target && best_value > 30.0f) { // Μικρότερο threshold
            int cost = wizard->getSpellCost(spell_to_cast);
            if (wizard->getHealth() >= cost) {
                wizard->setHealth(wizard->getHealth() - cost);
                wizard->castSpell(spell_to_cast, best_target);
            }
        }
    }
}

float AISystem::evaluateSpellValue(Wizard* wizard, int spell_id, Entity* target) {
    if (!wizard || !target) return 0.0f;

    float value = static_cast<float>(target->getHealth());

    // Προσαρμογή βάσει τύπου στόχου
    std::string type = target->getType();

    switch (spell_id) {
    case 0: // Freeze
        if (type == "Wizard") value *= 3.0f;
        else if (type == "Tower") value *= 2.0f;
        else if (type == "Warrior") value *= 1.5f;
        break;

    case 1: // Sickness
        if (type == "Wizard") value *= 4.0f;
        else if (type == "Warrior" && target->getLevel() > 2) value *= 3.0f;
        else if (type == "Tower") value *= 2.0f;
        break;

    case 2: // Convert
        if (type == "Wizard") value *= 5.0f;
        else if (type == "Tower") value *= 4.0f;
        else if (type == "Warrior") value *= 2.0f;
        break;
    }

    // Προσαρμογή βάσει στρατηγικής
    if (strategy == AIStrategy::AGGRESSIVE && spell_id == 1) value *= 1.5f;
    if (strategy == AIStrategy::DEFENSIVE && spell_id == 0) value *= 1.5f;
    if (strategy == AIStrategy::EXPANSIVE && spell_id == 2) value *= 2.0f;

    return value;
}

// Απλοποιημένες εκδόσεις των στρατηγικών
void AISystem::executeAggressiveStrategy() {
    auto enemy_entities = getEnemyEntities();
    for (auto entity : enemy_entities) {
        if (entity->getType() == "Warrior" && rand() % 100 < 70) {
            attackWeakestPlayerTarget(entity);
        }
    }
}

void AISystem::executeDefensiveStrategy() {
    auto enemy_entities = getEnemyEntities();
    for (auto entity : enemy_entities) {
        if (entity->getType() == "Warrior" && rand() % 100 < 30) {
            convertNearestNeutral(entity);
        }
    }
}

void AISystem::executeExpansiveStrategy() {
    auto enemy_entities = getEnemyEntities();
    for (auto entity : enemy_entities) {
        if ((entity->getType() == "Warrior" || entity->getType() == "Baby") && rand() % 100 < 60) {
            convertNearestNeutral(entity);
        }
    }
}

void AISystem::executeMixedStrategy() {
    auto enemy_entities = getEnemyEntities();
    for (auto entity : enemy_entities) {
        if (entity->getType() == "Warrior") {
            int roll = rand() % 100;
            if (roll < 40) attackWeakestPlayerTarget(entity);
            else if (roll < 70) convertNearestNeutral(entity);
        }
    }
}

void AISystem::executeAdaptiveStrategy() {
    // Χρησιμοποιούμε την makeStrategicDecisions που έχει adaptive λογική
    makeStrategicDecisions();
}

void AISystem::attackWeakestPlayerTarget(Entity* attacker) {
    if (!attacker) return;

    Entity* target = findWeakestPlayerEntity();
    if (!target) return;

    int amount = calculateOptimalTroopSize(attacker);
    if (amount > 0) {
        sendTroops(attacker, target, amount);
    }
}

void AISystem::attackStrategicTarget(Entity* attacker) {
    if (!attacker) return;

    Entity* target = findStrategicPlayerTarget();
    if (!target) return;

    int amount = calculateOptimalTroopSize(attacker);
    if (amount > 0) {
        sendTroops(attacker, target, amount);
    }
}

Entity* AISystem::findStrategicPlayerTarget() {
    auto player_entities = getPlayerEntities();
    if (player_entities.empty()) return nullptr;

    Entity* best_target = nullptr;
    float best_value = -1.0f;

    for (auto entity : player_entities) {
        float value = 0.0f;

        // Βασική αξία βάσει τύπου
        std::string type = entity->getType();
        if (type == "Wizard") value = 100.0f;
        else if (type == "Tower") value = 80.0f;
        else if (type == "Warrior") value = 60.0f;
        else value = 40.0f; // Baby

        // Προτεραιότητα σε αδύνατους στόχους
        value += (100.0f - static_cast<float>(entity->getHealth()));

        // Προτεραιότητα σε υψηλό level
        value += static_cast<float>(entity->getLevel() * 10.0f);

        if (value > best_value) {
            best_value = value;
            best_target = entity;
        }
    }

    return best_target;
}

Entity* AISystem::findMostThreateningPlayerEntity() {
    auto player_entities = getPlayerEntities();
    if (player_entities.empty()) return nullptr;

    Entity* most_threatening = nullptr;
    int max_threat = -1;

    for (auto entity : player_entities) {
        int threat = entity->getHealth() * entity->getLevel();
        if (entity->getType() == "Wizard") threat *= 2;

        if (threat > max_threat) {
            max_threat = threat;
            most_threatening = entity;
        }
    }

    return most_threatening;
}

void AISystem::convertNearestNeutral(Entity* converter) {
    if (!converter) return;

    Entity* target = findNearestNeutralEntity(converter);
    if (!target) return;

    int amount = calculateOptimalTroopSize(converter);
    if (amount > 0) {
        sendTroops(converter, target, amount);
    }
}

int AISystem::calculateOptimalTroopSize(Entity* attacker) {
    if (!attacker) return 0;

    // Στέλνουμε ποσοστό της ζωής του attacker
    int max_troops = attacker->getHealth() * 0.6f; // 60% της ζωής
    if (max_troops < 5) return 0; // Ελάχιστο 5

    // Εξασφαλίζουμε ότι min <= max
    int min_troops = 5; // Ελάχιστο 5
    int max_troops_clamped = std::min(100, max_troops); // Μέγιστο 30

    if (min_troops > max_troops_clamped) {
        return min_troops; // Στέλνουμε τουλάχιστον το ελάχιστο
    }

    std::uniform_int_distribution<> dis(min_troops, max_troops_clamped);
    return dis(gen);
}

void AISystem::sendTroops(Entity* from, Entity* to, int amount) {
    if (!from || !to || amount <= 0) return;

    // Αφήνουμε τουλάχιστον 10 ζωή για να μην πεθάνει αμέσως
    if (from->getHealth() <= amount + 10) {
        amount = from->getHealth() - 10; // Στέλνουμε όσο περισσότερο μπορούμε
        if (amount < 5) return; // Πολύ λίγο - δεν στέλνουμε
    }

    const PathData* path = entity_graph->getPath(from, to);
    if (path && !path->waypoints.empty()) {
        auto& troops = global_state->getTroops();
        troops.push_back(std::make_unique<Troop>(
            from, to, amount, path->waypoints
        ));
        from->setHealth(from->getHealth() - amount);
    }
}


// Βοηθητικές συναρτήσεις - απλοποιημένες
std::vector<Entity*> AISystem::getPlayerEntities() {
    std::vector<Entity*> result;
    auto& entities = global_state->getEntities();

    for (auto& entity : entities) {
        if (entity->getSide() == Side::PLAYER) {
            result.push_back(entity.get());
        }
    }
    return result;
}

std::vector<Entity*> AISystem::getNeutralEntities() {
    std::vector<Entity*> result;
    auto& entities = global_state->getEntities();

    for (auto& entity : entities) {
        if (entity->getSide() == Side::NEUTRAL) {
            result.push_back(entity.get());
        }
    }
    return result;
}

std::vector<Entity*> AISystem::getEnemyEntities() {
    std::vector<Entity*> result;
    auto& entities = global_state->getEntities();

    for (auto& entity : entities) {
        if (entity->getSide() == Side::ENEMY) {
            result.push_back(entity.get());
        }
    }
    return result;
}

Entity* AISystem::findWeakestPlayerEntity() {
    auto player_entities = getPlayerEntities();
    if (player_entities.empty()) return nullptr;

    Entity* weakest = player_entities[0];
    for (auto entity : player_entities) {
        if (entity->getHealth() < weakest->getHealth()) {
            weakest = entity;
        }
    }
    return weakest;
}

Entity* AISystem::findNearestNeutralEntity(Entity* from) {
    if (!from) return nullptr;

    auto neutral_entities = getNeutralEntities();
    if (neutral_entities.empty()) return nullptr;

    Entity* nearest = nullptr;
    float min_dist = FLT_MAX;

    for (auto entity : neutral_entities) {
        float dx = entity->getX() - from->getX();
        float dy = entity->getY() - from->getY();
        float dist = dx * dx + dy * dy;

        if (dist < min_dist) {
            min_dist = dist;
            nearest = entity;
        }
    }
    return nearest;
}

bool AISystem::canReachTarget(Entity* from, Entity* to) {
    if (!from || !to) return false;

    const PathData* path = entity_graph->getPath(from, to);
    return path && !path->waypoints.empty();
}

bool AISystem::isThreatening(Entity* entity) {
    if (!entity) return false;

    std::string type = entity->getType();
    return (type == "Wizard" ||
        type == "Tower" ||
        (type == "Warrior" && entity->getLevel() > 1));
}