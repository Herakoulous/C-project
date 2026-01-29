#pragma once
#include "entity.h"
#include "graph.h"
#include <vector>
#include <memory>

// Forward declaration για το GlobalState
class GlobalState;

// Κλάση για το "AI" του εχθρού
// Χειρίζεται τις αποφάσεις και ενέργειες των εχθρικών entities
class AISystem {
private:
    GlobalState* global_state; // Δείκτης σε κεντρική κατάσταση παιχνιδιού
    EntityGraph* entity_graph; // Δείκτης σε γράφο συνδέσεων

    // Κατάσταση AI
    float decision_timer; // Χρονόμετρο για λήψη αποφάσεων
    float decision_interval; // Χρονικό διάστημα μεταξύ αποφάσεων
    float aggression_level; // Επίπεδο επιθετικότητας (0.0 έως 1.0)

    // Στρατηγικές του AI
    enum class AIStrategy {
        AGGRESSIVE,     // Συνεχείς επιθέσεις
        DEFENSIVE,      // ’μυνα
        EXPANSIVE,      // Επέκταση με μετατροπή ουδετέρων
        MIXED,          // Ισορροπημένη προσέγγιση
        ADAPTIVE        // Προσαρμοστική (αλλάζει βάσει κατάστασης)
    };

    AIStrategy strategy; // Τρέχουσα στρατηγική

    // Λήψη αποφάσεων και ανάλυση
    void analyzeGameState();
    void makeStrategicDecisions();
    int calculateTotalPower(const std::vector<Entity*>& entities);

    // Υλοποιήσεις στρατηγικών
    void executeAggressiveStrategy();
    void executeDefensiveStrategy();
    void executeExpansiveStrategy();
    void executeMixedStrategy();
    void executeAdaptiveStrategy();

    // Τακτικές ενέργειες
    void attackWeakestPlayerTarget(Entity* attacker);
    void attackStrategicTarget(Entity* attacker);
    void convertNearestNeutral(Entity* converter);
    void sendTroops(Entity* from, Entity* to, int amount);
    int calculateOptimalTroopSize(Entity* attacker);

    // Ρίψη spells από Wizards
    void castEnemySpells();
    void castSmartSpell(Wizard* wizard);
    float evaluateSpellValue(Wizard* wizard, int spell_id, Entity* target);
    bool isThreatening(Entity* entity);

    // Βοηθητικές συναρτήσεις
    Entity* findWeakestPlayerEntity();
    Entity* findStrategicPlayerTarget();
    Entity* findMostThreateningPlayerEntity();
    Entity* findNearestNeutralEntity(Entity* from);
    std::vector<Entity*> getPlayerEntities();
    std::vector<Entity*> getNeutralEntities();
    std::vector<Entity*> getEnemyEntities();
    bool canReachTarget(Entity* from, Entity* to);

public:
    AISystem(GlobalState* gs, EntityGraph* graph); // Constructor
    void update(float dt); // Αρχικοποίηση AI
    void init(); // Ενημέρωση AI κάθε frame
};