#include "global_state.h"
#include "sgg/graphics.h"
#include <algorithm>
#include <cmath>

GlobalState* GlobalState::instance = nullptr;

GlobalState::GlobalState() : selected_entity(nullptr), casting_wizard(nullptr), pending_spell_id(-1),
casting_timer(0.0f), casting_duration(2.0f), is_casting(false) {
}

GlobalState* GlobalState::getInstance() {
    if (!instance) {
        instance = new GlobalState();
    }
    return instance;
}

void GlobalState::init() {
    entities.clear();
    troops.clear();
    attack_effects.clear();
    entity_graph.clear();
    selected_entity = nullptr;

    entities.push_back(std::make_unique<Warrior>(3.5f, 3.0f, 200, Side::PLAYER));
    entities.push_back(std::make_unique<Wizard>(1.5f, 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(3.5f, 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(1.5f, 1.5f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Tower>(8.0f, 4.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 5.5f, 1.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(5.5f, 8 - 1.4f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Wizard>(16 - 3.5f, 8 - 3.0f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 3.5f, 8 - 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 1.5f, 10, Side::NEUTRAL));

    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            entity_graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    entity_graph.calculatePaths(entities);
}

void GlobalState::handleTroopArrivals() {
    for (auto it = troops.begin(); it != troops.end(); ) {
        if ((*it)->hasArrived()) {
            Entity* target = (*it)->getTarget();
            Entity* source = (*it)->getSource();
            int amount = (*it)->getHealthAmount();

            if (target->getSide() == source->getSide()) {
                target->setHealth(target->getHealth() + amount);
            }
            else {
                int damage = std::max(1, amount - (int)target->getDefense());
                target->setHealth(target->getHealth() - damage);
                if (target->getHealth() < 0) {
                    target->setSide(source->getSide());
                    target->setHealth(std::abs(target->getHealth()));
                }
            }

            it = troops.erase(it);
        }
        else if ((*it)->isDead()) {
            // Remove dead troops
            it = troops.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GlobalState::handleTowerAttacks() {
    // Convert unique_ptr vector to raw pointer vector for the attack function
    std::vector<Troop*> troop_ptrs;
    for (auto& troop : troops) {
        troop_ptrs.push_back(troop.get());
    }

    for (auto& entity : entities) {
        if (entity->getType() == "Tower") {
            Tower* tower = static_cast<Tower*>(entity.get());
            tower->attackNearbyTroops(troop_ptrs, attack_effects);
        }
    }
}

void GlobalState::updateAttackEffects(float dt) {
    float dt_seconds = dt / 1000.0f;

    for (auto it = attack_effects.begin(); it != attack_effects.end(); ) {
        it->elapsed += dt_seconds;
        if (it->elapsed >= it->duration) {
            it = attack_effects.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GlobalState::drawAttackEffects() {
    for (const auto& effect : attack_effects) {
        float progress = effect.elapsed / effect.duration;
        float alpha = 1.0f - progress;

        graphics::Brush br;
        br.fill_color[0] = effect.r;
        br.fill_color[1] = effect.g;
        br.fill_color[2] = effect.b;
        br.fill_opacity = alpha;
        br.outline_opacity = alpha;
        br.outline_width = 3.0f;

        // Interpolate position
        float current_x = effect.start_x + (effect.end_x - effect.start_x) * progress;
        float current_y = effect.start_y + (effect.end_y - effect.start_y) * progress;

        // Draw projectile circle
        graphics::drawDisk(current_x, current_y, 0.15f, br);
    }
}

void GlobalState::handleHoverMenu(float canvas_x, float canvas_y) {
    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            if (entity->getSide() == Side::PLAYER) {
                if (entity->getType() == "Baby") {
                    hover_menu.show(entity->getX(), entity->getY() + 1.0f,
                        entity.get(), true, 0);
                }
                else {
                    hover_menu.show(entity->getX(), entity->getY() + 1.3f,
                        entity.get(), false, entity->getUpgradeCost());
                }
            }
            break;
        }
    }

    if (hover_menu.isVisible()) {
        int hovered = hover_menu.checkClick(canvas_x, canvas_y);
        if (hovered == -1) {
            bool over_entity = false;
            for (auto& entity : entities) {
                if (entity->contains(canvas_x, canvas_y)) {
                    over_entity = true;
                    break;
                }
            }
            if (!over_entity) hover_menu.hide();
        }
    }
}

void GlobalState::handleMenuClick(float canvas_x, float canvas_y, int clicked_option) {
    Entity* target = hover_menu.getTarget();
    bool is_baby = hover_menu.getIsBaby();

    // Wizard spell buttons (IDs 10, 11, 12)
    if (clicked_option >= 10 && clicked_option <= 12) {
        if (target->getType() == "Wizard") {
            Wizard* wizard = static_cast<Wizard*>(target);
            int spell_id = clicked_option - 10; // 10->0, 11->1, 12->2

            if (wizard->canCastSpell(spell_id)) {
                // Start casting preparation
                casting_wizard = wizard;
                pending_spell_id = spell_id;
                casting_timer = 0.0f;
                is_casting = true;
                hover_menu.hide();
                return;
            }
        }
        hover_menu.hide();
        return;
    }

    if (is_baby) {
        if (clicked_option == 0 && target->getHealth() >= 5) {
            int hp = target->getHealth() - 5;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    entities[i] = std::make_unique<Warrior>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    for (size_t j = 0; j < entities.size(); j++) {
                        if (j != i && entities[j].get() != new_entity) {
                            entity_graph.addEdge(new_entity, entities[j].get());
                        }
                    }

                    hover_menu.hide();
                    break;
                }
            }
        }
        else if (clicked_option == 1 && target->getHealth() >= 10) {
            int hp = target->getHealth() - 10;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    entities[i] = std::make_unique<Tower>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    for (size_t j = 0; j < entities.size(); j++) {
                        if (j != i && entities[j].get() != new_entity) {
                            entity_graph.addEdge(new_entity, entities[j].get());
                        }
                    }

                    hover_menu.hide();
                    break;
                }
            }
        }
        else if (clicked_option == 2 && target->getHealth() >= 10) {
            int hp = target->getHealth() - 10;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    entities[i] = std::make_unique<Wizard>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    for (size_t j = 0; j < entities.size(); j++) {
                        if (j != i && entities[j].get() != new_entity) {
                            entity_graph.addEdge(new_entity, entities[j].get());
                        }
                    }

                    hover_menu.hide();
                    break;
                }
            }
        }
    }
    else {
        if (target->canUpgrade()) {
            target->performUpgrade();
            hover_menu.hide();
        }
        else {
            hover_menu.hide();
        }
    }

    entity_graph.calculatePaths(entities);
}

void GlobalState::handleWizardSpellCast(float canvas_x, float canvas_y) {
    if (!casting_wizard || pending_spell_id < 0 || !is_casting) return;
    if (casting_timer < casting_duration) return; // Not finished casting yet

    // Find clicked entity
    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            // Cast spell on this entity
            casting_wizard->castSpell(pending_spell_id, entity.get());

            // Reset casting mode
            casting_wizard = nullptr;
            pending_spell_id = -1;
            is_casting = false;
            casting_timer = 0.0f;
            return;
        }
    }

    // Clicked empty space - cancel spell
    casting_wizard = nullptr;
    pending_spell_id = -1;
    is_casting = false;
    casting_timer = 0.0f;
}

void GlobalState::updateCasting(float dt) {
    if (!is_casting || !casting_wizard) return;

    float dt_seconds = dt / 1000.0f;
    casting_timer += dt_seconds;

    // If casting finished, keep waiting for target selection
    if (casting_timer >= casting_duration) {
        // Casting finished, waiting for target
        // Do nothing here, just keep the state active
    }
}

void GlobalState::drawCastingBar() {
    if (!is_casting || !casting_wizard) return;

    float wizard_x = casting_wizard->getX();
    float wizard_y = casting_wizard->getY();
    float wizard_size = casting_wizard->getSize();

    // Draw casting bar background
    graphics::Brush br_bg;
    br_bg.fill_color[0] = 0.1f;
    br_bg.fill_color[1] = 0.1f;
    br_bg.fill_color[2] = 0.1f;
    br_bg.fill_opacity = 0.8f;

    float bar_width = 1.0f;
    float bar_height = 0.15f;
    float bar_x = wizard_x;
    float bar_y = wizard_y - wizard_size - 0.3f;

    graphics::drawRect(bar_x, bar_y, bar_width, bar_height, br_bg);

    // Draw casting progress
    graphics::Brush br_progress;

    // Color based on spell
    if (pending_spell_id == 0) { // Freeze - Blue
        br_progress.fill_color[0] = 0.3f;
        br_progress.fill_color[1] = 0.3f;
        br_progress.fill_color[2] = 1.0f;
    }
    else if (pending_spell_id == 1) { // Sickness - Green
        br_progress.fill_color[0] = 0.3f;
        br_progress.fill_color[1] = 1.0f;
        br_progress.fill_color[2] = 0.3f;
    }
    else if (pending_spell_id == 2) { // Convert - Red
        br_progress.fill_color[0] = 1.0f;
        br_progress.fill_color[1] = 0.3f;
        br_progress.fill_color[2] = 0.3f;
    }

    br_progress.fill_opacity = 0.8f;

    float progress = casting_timer / casting_duration;
    if (progress > 1.0f) progress = 1.0f;

    float progress_width = bar_width * progress;
    float progress_x = wizard_x - bar_width / 2 + progress_width / 2;

    graphics::drawRect(progress_x, bar_y, progress_width, bar_height, br_progress);

    // Draw casting text
    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f;
    br_text.fill_color[1] = 1.0f;
    br_text.fill_color[2] = 1.0f;

    std::string spell_name;
    if (pending_spell_id == 0) spell_name = "FREEZE";
    else if (pending_spell_id == 1) spell_name = "SICKNESS";
    else if (pending_spell_id == 2) spell_name = "CONVERT";

    if (casting_timer < casting_duration) {
        // Still casting
        graphics::drawText(bar_x - 0.45f, bar_y + 0.05f, 0.1f, "CASTING " + spell_name + "...", br_text);
    }
    else {
        // Casting finished, waiting for target
        br_text.fill_color[0] = 1.0f;
        br_text.fill_color[1] = 1.0f;
        br_text.fill_color[2] = 0.0f; // Yellow text
        graphics::drawText(bar_x - 0.5f, bar_y + 0.05f, 0.1f, "SELECT TARGET FOR " + spell_name, br_text);
    }

    // Draw wizard outline to show he's casting
    graphics::Brush br_outline;
    br_outline.fill_color[0] = 1.0f;
    br_outline.fill_color[1] = 1.0f;
    br_outline.fill_color[2] = 0.0f;
    br_outline.fill_opacity = 0.0f;
    br_outline.outline_opacity = 1.0f;
    br_outline.outline_width = 4.0f;

    // Pulsing effect when casting
    float pulse = 0.1f * sinf(casting_timer * 10.0f) + 0.1f;
    br_outline.outline_opacity = 0.7f + pulse;

    graphics::drawDisk(wizard_x, wizard_y, wizard_size + 0.2f, br_outline);
}
 
void GlobalState::handleEntityClick(float canvas_x, float canvas_y) {
    bool clicked_entity = false;

    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            clicked_entity = true;

            if (selected_entity && selected_entity != entity.get()) {
                if (selected_entity->getSide() == Side::PLAYER) {
                    int amount = selected_entity->getHealth() / 2;
                    if (amount > 0) {
                        const PathData* path = entity_graph.getPath(selected_entity, entity.get());
                        if (path && !path->waypoints.empty()) {
                            troops.push_back(std::make_unique<Troop>(
                                selected_entity, entity.get(), amount, path->waypoints
                            ));
                            selected_entity->setHealth(selected_entity->getHealth() - amount);
                        }
                    }
                }
                selected_entity = nullptr;
            }
            else {
                if (entity->getSide() == Side::PLAYER) {
                    selected_entity = entity.get();
                }
                else {
                    selected_entity = nullptr;
                }
            }
            break;
        }
    }

    if (!clicked_entity) {
        selected_entity = nullptr;
    }
}

void GlobalState::handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed) {
    if (mouse_pressed) {
        // Check if we're in spell casting mode (casting finished)
        if (is_casting && casting_timer >= casting_duration) {
            handleWizardSpellCast(canvas_x, canvas_y);
            return;
        }

        // If still casting, clicking cancels it
        if (is_casting && casting_timer < casting_duration) {
            // Cancel casting
            casting_wizard = nullptr;
            pending_spell_id = -1;
            is_casting = false;
            casting_timer = 0.0f;
            return;
        }

        int clicked_option = hover_menu.checkClick(canvas_x, canvas_y);
        if (clicked_option != -1) {
            handleMenuClick(canvas_x, canvas_y, clicked_option);
        }
        else {
            handleEntityClick(canvas_x, canvas_y);
        }
    }
}

void GlobalState::update(float dt) {
    for (auto& entity : entities) {
        entity->update(dt);
    }

    for (auto& troop : troops) {
        troop->update(dt);
    }

    handleTowerAttacks();
    handleTroopArrivals();
    updateAttackEffects(dt);
    updateCasting(dt);

    graphics::MouseState mouse;
    graphics::getMouseState(mouse);
    float canvas_x = graphics::windowToCanvasX((float)mouse.cur_pos_x);
    float canvas_y = graphics::windowToCanvasY((float)mouse.cur_pos_y);

    handleHoverMenu(canvas_x, canvas_y);
    handleMouseInput(canvas_x, canvas_y, mouse.button_left_pressed);
}

void GlobalState::draw() {
    graphics::Brush bg;
    bg.fill_color[0] = 0.0f; bg.fill_color[1] = 1.0f; bg.fill_color[2] = 0.0f;
    graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, bg);

    entity_graph.draw();

    for (auto& entity : entities) {
        entity->draw();
    }

    for (auto& troop : troops) {
        troop->draw();
    }

    drawAttackEffects();

    if (selected_entity) {
        graphics::Brush hl;
        hl.fill_color[0] = hl.fill_color[1] = 1.0f; hl.fill_color[2] = 0.0f;
        hl.fill_opacity = 0.0f;
        hl.outline_opacity = 1.0f;
        hl.outline_width = 3.0f;
        graphics::drawDisk(selected_entity->getX(), selected_entity->getY(),
            selected_entity->getSize() + 0.1f, hl);
    }

    hover_menu.draw();

    drawCastingBar();
}