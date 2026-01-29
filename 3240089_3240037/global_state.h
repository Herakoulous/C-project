#pragma once
#include "node.h"
#include "entity.h"
#include "hover_menu.h"
#include "troop.h"
#include "graph.h"
#include "pause_menu.h"     
#include "level_manager.h"   
#include <vector>
#include <memory>

// Δήλωση μπροστά για την κλάση AISystem
class AISystem;

// Κλάση Singleton για την καθολική κατάσταση του παιχνιδιού
// Διατηρεί όλα τα δεδομένα και λογική του παιχνιδιού σε ένα κεντρικό σημείο
class GlobalState {
private:
    static GlobalState* instance;  // Δείκτης στο μοναδικό instance (Singleton pattern)

	// Δεδομένα παιχνιδιού
    std::vector<std::unique_ptr<Entity>> entities;  // Όλα τα entities
    std::vector<std::unique_ptr<Troop>> troops;     // Όλα τα troops
    std::vector<Tower::AttackEffect> attack_effects; // Οπτικά effects επιθέσεων πύργων


    EntityGraph entity_graph;     // Γράφος συνδέσεων μεταξύ οντοτήτων
    Entity* selected_entity;      // Τρέχουσα επιλεγμένη οντότητα (για αποστολή troops)
    HoverMenu hover_menu;         // Menu που εμφανίζεται όταν γίνεται hover σε οντότητα
    AISystem* ai_system;          // Σύστημα τεχνητής νοημοσύνης για τον εχθρό
    PauseMenu pause_menu;         // Μενού παύσης του παιχνιδιού
    LevelManager level_manager;   // Διαχειριστής επιπέδων

    // Spells
    struct ReadySpell {
        Wizard* wizard;           // Wizard που ετοιμάζει το spell
        int spell_id;             // ID του spell (0=Freeze, 1=Sickness, 2=Convert)
        bool is_ready;            // Αν το spell είναι έτοιμο για χρήση
        float prep_time;          // Χρόνος προετοιμασίας (0-2 δευτερόλεπτα)
    };

    std::vector<ReadySpell> ready_spells;  // Λίστα spells που προετοιμάζονται ή είναι έτοιμα
    Wizard* target_selection_wizard;       // Wizard που έχει επιλέξει spell και περιμένει στόχο
    int target_selection_spell_id;         // ID του spell που έχει επιλεγεί για ρίψη

    void handleTroopArrivals();                       // Επεξεργασία troops που έφτασαν στον στόχο
    void handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed);  // Επεξεργασία κλικ ποντικιού
    void handleHoverMenu(float canvas_x, float canvas_y);  // Διαχείριση hover menu
    void handleEntityClick(float canvas_x, float canvas_y); // Επεξεργασία κλικ σε οντότητα
    void handleMenuClick(float canvas_x, float canvas_y, int clicked_option);  // Επεξεργασία κλικ σε menu
    void handleTowerAttacks();                       // Επεξεργασία επιθέσεων πύργων
    void updateAttackEffects(float dt);               // Ενημέρωση οπτικών effects
    void drawAttackEffects();                         // Σχεδίαση οπτικών effects
    void handleWizardSpellCast(float canvas_x, float canvas_y);  // Επεξεργασία ρίψης spell
    void drawReadySpells();                           // Σχεδίαση spells που είναι έτοιμα
    void updateReadySpells(float dt);                 // Ενημέρωση προετοιμασίας spells

public:
    GlobalState();  // Constructor

    // Στατικές συναρτήσεις singleton
    static GlobalState* getInstance();  // Λήψη του μοναδικού instance

	// Απαγόρευση αντιγραφής/ανάθεσης
    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;

	// Βασικές συναρτήσεις
    void init();                         // Αρχικοποίηση παιχνιδιού
    void initLevel(int level = 1);       // Αρχικοποίηση συγκεκριμένου level
    void update(float dt);               // Ενημέρωση κατάστασης παιχνιδιού
    void draw();                         // Σχεδίαση όλων των στοιχείων

    // Getters
    std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }
    std::vector<std::unique_ptr<Troop>>& getTroops() { return troops; }
    EntityGraph& getGraph() { return entity_graph; }
    Entity* getSelectedEntity() const { return selected_entity; }
    HoverMenu& getHoverMenu() { return hover_menu; }
    PauseMenu& getPauseMenu() { return pause_menu; }
    LevelManager& getLevelManager() { return level_manager; }

    // Συναρτήσεις υπολογισμού και σχεδίασης
    std::pair<int, int> calculateTotalHealth() const;  // Υπολογισμός συνολικής ζωής παίκτη/εχθρού
    void drawHealthBars();                             // Σχεδίαση health bars

    // Setters
    void setSelectedEntity(Entity* entity) { selected_entity = entity; }

    // Destructor
    ~GlobalState();
};