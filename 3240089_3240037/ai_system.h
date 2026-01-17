#pragma once
#include "entity.h"
#include "graph.h"
#include <vector>
#include <memory>

class GlobalState;

class AISystem {
private:
    GlobalState* global_state;
    EntityGraph* entity_graph;

    // AI state
    float decision_timer;
    float decision_interval;
    float aggression_level; // 0.0 to 1.0

    enum class AIStrategy {
        AGGRESSIVE,     // Attack constantly
        DEFENSIVE,      // Build up defenses
        EXPANSIVE,      // Convert neutrals
        MIXED,          // Balanced approach
        ADAPTIVE        // Changes based on situation
    };

    AIStrategy strategy;

    // AI decision making
    void analyzeGameState();
    void makeStrategicDecisions();
    int calculateTotalPower(const std::vector<Entity*>& entities);

    // Strategy implementations
    void executeAggressiveStrategy();
    void executeDefensiveStrategy();
    void executeExpansiveStrategy();
    void executeMixedStrategy();
    void executeAdaptiveStrategy();

    // Tactical actions
    void attackWeakestPlayerTarget(Entity* attacker);
    void attackStrategicTarget(Entity* attacker);
    void convertNearestNeutral(Entity* converter);
    void sendTroops(Entity* from, Entity* to, int amount);
    int calculateOptimalTroopSize(Entity* attacker); // ???????? ?? ?????

    // Spell casting
    void castEnemySpells();
    void castSmartSpell(Wizard* wizard);
    float evaluateSpellValue(Wizard* wizard, int spell_id, Entity* target);
    bool isThreatening(Entity* entity);

    // Helper functions
    Entity* findWeakestPlayerEntity();
    Entity* findStrategicPlayerTarget();
    Entity* findMostThreateningPlayerEntity();
    Entity* findNearestNeutralEntity(Entity* from);
    std::vector<Entity*> getPlayerEntities();
    std::vector<Entity*> getNeutralEntities();
    std::vector<Entity*> getEnemyEntities();
    bool canReachTarget(Entity* from, Entity* to);

public:
    AISystem(GlobalState* gs, EntityGraph* graph);
    void update(float dt);
    void init();
};