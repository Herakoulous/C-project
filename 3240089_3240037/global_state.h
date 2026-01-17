#pragma once
#include "node.h"
#include "entity.h"
#include "hover_menu.h"
#include "troop.h"
#include "graph.h"
#include "pause_menu.h"  // Αυτό υπάρχει ήδη
#include "level_manager.h"  // ΠΡΟΣΘΗΚΗ
#include <vector>
#include <memory>

// Forward declaration
class AISystem;

class GlobalState {
private:
    static GlobalState* instance;

    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Troop>> troops;
    std::vector<Tower::AttackEffect> attack_effects;

    EntityGraph entity_graph;
    Entity* selected_entity;
    HoverMenu hover_menu;
    AISystem* ai_system;
    PauseMenu pause_menu;  // Αυτό υπάρχει ήδη
    LevelManager level_manager;  // ΠΡΟΣΘΗΚΗ

    struct ReadySpell {
        Wizard* wizard;
        int spell_id;
        bool is_ready;
        float prep_time;
    };

    std::vector<ReadySpell> ready_spells;
    Wizard* target_selection_wizard;
    int target_selection_spell_id;

    void handleTroopArrivals();
    void handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed);
    void handleHoverMenu(float canvas_x, float canvas_y);
    void handleEntityClick(float canvas_x, float canvas_y);
    void handleMenuClick(float canvas_x, float canvas_y, int clicked_option);
    void handleTowerAttacks();
    void updateAttackEffects(float dt);
    void drawAttackEffects();
    void handleWizardSpellCast(float canvas_x, float canvas_y);
    void drawReadySpells();
    void updateReadySpells(float dt);

public:
    GlobalState();

    static GlobalState* getInstance();

    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;

    void init();
    void initLevel(int level = 1);  // ΝΕΗ ΣΥΝΑΡΤΗΣΗ
    void update(float dt);
    void draw();

    std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }
    std::vector<std::unique_ptr<Troop>>& getTroops() { return troops; }
    EntityGraph& getGraph() { return entity_graph; }
    Entity* getSelectedEntity() const { return selected_entity; }
    HoverMenu& getHoverMenu() { return hover_menu; }
    PauseMenu& getPauseMenu() { return pause_menu; }  // Αυτό υπάρχει ήδη
    LevelManager& getLevelManager() { return level_manager; }  // ΠΡΟΣΘΗΚΗ
    std::pair<int, int> calculateTotalHealth() const;
    void drawHealthBars();

    void setSelectedEntity(Entity* entity) { selected_entity = entity; }

    ~GlobalState();
};