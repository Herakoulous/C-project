#pragma once
#include "node.h"
#include "entity.h"
#include "hover_menu.h"
#include "troop.h"
#include "graph.h"
#include <vector>
#include <memory>

class GlobalState {
private:
    static GlobalState* instance;

    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Troop>> troops;
    std::vector<Tower::AttackEffect> attack_effects;

    EntityGraph entity_graph;
    Entity* selected_entity;
    HoverMenu hover_menu;

    // Wizard spell casting
    Wizard* casting_wizard;
    int pending_spell_id;
    float casting_timer;
    float casting_duration;
    bool is_casting;

    void handleTroopArrivals();
    void handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed);
    void handleHoverMenu(float canvas_x, float canvas_y);
    void handleEntityClick(float canvas_x, float canvas_y);
    void handleMenuClick(float canvas_x, float canvas_y, int clicked_option);
    void handleTowerAttacks();
    void updateAttackEffects(float dt);
    void drawAttackEffects();
    void handleWizardSpellCast(float canvas_x, float canvas_y);
    void updateCasting(float dt);
    void drawCastingBar();

public:
    GlobalState();

    static GlobalState* getInstance();

    GlobalState(const GlobalState&) = delete;
    GlobalState& operator=(const GlobalState&) = delete;

    void init();
    void update(float dt);
    void draw();

    std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }
    std::vector<std::unique_ptr<Troop>>& getTroops() { return troops; }
    EntityGraph& getGraph() { return entity_graph; }
    Entity* getSelectedEntity() const { return selected_entity; }
    HoverMenu& getHoverMenu() { return hover_menu; }

    void setSelectedEntity(Entity* entity) { selected_entity = entity; }

    ~GlobalState();
};