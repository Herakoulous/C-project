#include "global_state.h"
#include "ai_system.h"  // Include AI system
#include "sgg/graphics.h"
#include <algorithm>
#include <cmath>


GlobalState* GlobalState::instance = nullptr;

GlobalState::GlobalState() : selected_entity(nullptr),
target_selection_wizard(nullptr), target_selection_spell_id(-1),
ai_system(nullptr) {  // Initialize to nullptr
}

GlobalState* GlobalState::getInstance() {
    if (!instance) {
        instance = new GlobalState();
    }
    return instance;
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
    updateReadySpells(dt);

    // Update AI system if it exists
    if (ai_system) {
        ai_system->update(dt);
    }

    graphics::MouseState mouse;
    graphics::getMouseState(mouse);
    float canvas_x = graphics::windowToCanvasX((float)mouse.cur_pos_x);
    float canvas_y = graphics::windowToCanvasY((float)mouse.cur_pos_y);

    handleHoverMenu(canvas_x, canvas_y);
    handleMouseInput(canvas_x, canvas_y, mouse.button_left_pressed);
}

void GlobalState::init() {
    entities.clear();
    troops.clear();
    attack_effects.clear();
    entity_graph.clear();
    selected_entity = nullptr;

    // Clean up AI system if it exists
    if (ai_system) {
        delete ai_system;
        ai_system = nullptr;
    }

    // Initialize player entities
    entities.push_back(std::make_unique<Warrior>(3.5f, 3.0f, 200, Side::PLAYER));
    entities.push_back(std::make_unique<Wizard>(1.5f, 3.5f, 10, Side::PLAYER));
    entities.push_back(std::make_unique<Baby>(3.5f, 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(1.5f, 1.5f, 10, Side::NEUTRAL));

    // Neutral towers
    entities.push_back(std::make_unique<Tower>(8.0f, 4.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 5.5f, 1.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(5.5f, 8 - 1.4f, 10, Side::NEUTRAL));

    // Enemy entities (AI will control these)
    entities.push_back(std::make_unique<Warrior>(16 - 3.5f, 8 - 3.0f, 15, Side::ENEMY));
    entities.push_back(std::make_unique<Wizard>(16 - 1.5f, 8 - 3.5f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 3.5f, 8 - 1.0f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 1.5f, 10, Side::NEUTRAL));

    // Create graph connections
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            entity_graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    entity_graph.calculatePaths(entities);

    // Create AI system
    ai_system = new AISystem(this, &entity_graph);
    if (ai_system) {
        ai_system->init();
    }
}

// Add destructor
GlobalState::~GlobalState() {
    if (ai_system) {
        delete ai_system;
        ai_system = nullptr;
    }
}

void GlobalState::updateReadySpells(float dt) {
    float dt_seconds = dt / 1000.0f;

    // Update preparation time for spells being prepared
    for (auto& spell : ready_spells) {
        if (!spell.is_ready) {
            spell.prep_time += dt_seconds;

            // Check if preparation is complete (2 seconds)
            if (spell.prep_time >= 2.0f) {
                spell.is_ready = true;
                spell.prep_time = 2.0f; // Clamp to max
            }
        }
    }

    // Remove invalid ready spells (wizard died or was converted)
    ready_spells.erase(
        std::remove_if(ready_spells.begin(), ready_spells.end(),
            [](const ReadySpell& s) {
                return !s.wizard || s.wizard->getSide() != Side::PLAYER;
            }),
        ready_spells.end()
    );
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
                // Check if this spell is already ready
                bool already_ready = false;
                for (auto& spell : ready_spells) {
                    if (spell.wizard == wizard && spell.spell_id == spell_id) {
                        already_ready = true;
                        break;
                    }
                }

                if (!already_ready) {
                    // Start preparing this spell - ΑΛΛΑΓΗ: ΔΕΝ ΠΛΗΡΩΝΟΥΜΕ HP ΕΔΩ
                    ReadySpell new_spell;
                    new_spell.wizard = wizard;
                    new_spell.spell_id = spell_id;
                    new_spell.is_ready = false;
                    new_spell.prep_time = 0.0f; // Start from 0
                    ready_spells.push_back(new_spell);

                    // ΑΦΑΙΡΕΙΣΗ: ΔΕΝ ΑΦΑΙΡΟΥΜΕ HP ΕΔΩ
                    // wizard->setHealth(wizard->getHealth() - wizard->getSpellCost(spell_id));

                    // Start cooldown immediately
                    wizard->castSpell(spell_id, nullptr); // Pass nullptr to just start cooldown
                }

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
    if (!target_selection_wizard || target_selection_spell_id < 0) return;

    // Find clicked entity
    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            // Ελέγχουμε αν ο στόχος είναι ο ίδιος με τον wizard
            if (entity.get() == target_selection_wizard) {
                // Δεν επιτρέπουμε να castάρεις spell στον εαυτό σου
                // Αλλά το spell επιστρέφει στην ready λίστα
                ReadySpell spell;
                spell.wizard = target_selection_wizard;
                spell.spell_id = target_selection_spell_id;
                spell.is_ready = true;
                spell.prep_time = 2.0f; // Mark as fully prepared
                ready_spells.push_back(spell);

                target_selection_wizard = nullptr;
                target_selection_spell_id = -1;
                return;
            }

            // ΠΛΗΡΩΜΑ HP ΟΤΑΝ ΧΡΗΣΙΜΟΠΟΙΕΙΣ ΤΟ SPELL (όχι όταν ξεκινά)
            // Ελέγχουμε αν ο wizard έχει αρκετά HP για να castάρει το spell
            int spell_cost = target_selection_wizard->getSpellCost(target_selection_spell_id);
            if (target_selection_wizard->getHealth() < spell_cost) {
                // Δεν έχει αρκετά HP, επιστρέφει το spell
                ReadySpell spell;
                spell.wizard = target_selection_wizard;
                spell.spell_id = target_selection_spell_id;
                spell.is_ready = true;
                spell.prep_time = 2.0f;
                ready_spells.push_back(spell);

                target_selection_wizard = nullptr;
                target_selection_spell_id = -1;
                return;
            }

            // ΠΛΗΡΩΝΕΙ ΤΟ HP ΤΩΡΑ
            target_selection_wizard->setHealth(target_selection_wizard->getHealth() - spell_cost);

            // Cast spell on this entity
            Entity* target = entity.get();

            // Apply spell effect based on spell_id
            switch (target_selection_spell_id) {
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

            // Reset target selection - spell has been used
            target_selection_wizard = nullptr;
            target_selection_spell_id = -1;
            return;
        }
    }

    // Clicked empty space - cancel target selection but spell remains ready
    // Put it back in ready_spells
    ReadySpell spell;
    spell.wizard = target_selection_wizard;
    spell.spell_id = target_selection_spell_id;
    spell.is_ready = true;
    spell.prep_time = 2.0f; // Mark as fully prepared
    ready_spells.push_back(spell);

    target_selection_wizard = nullptr;
    target_selection_spell_id = -1;
}

void GlobalState::drawReadySpells() {
    static float total_time = 0.0f;
    total_time += 0.016f; // Approximate frame time for animation

    for (const auto& spell : ready_spells) {
        if (!spell.wizard) continue;

        float wizard_x = spell.wizard->getX();
        float wizard_y = spell.wizard->getY();
        float wizard_size = spell.wizard->getSize();

        // Position the indicator BELOW the wizard
        float indicator_y = wizard_y + wizard_size + 0.5f; // ΑΥΤΟ ΕΙΝΑΙ ΚΑΤΩ ΑΠΟ ΤΟΝ WIZARD
        float indicator_x = wizard_x;

        // Determine spell color
        graphics::Brush br_spell;
        std::string spell_name;

        if (spell.spell_id == 0) { // Freeze - Blue
            br_spell.fill_color[0] = 0.3f;
            br_spell.fill_color[1] = 0.3f;
            br_spell.fill_color[2] = 1.0f;
            spell_name = "FREEZE";
        }
        else if (spell.spell_id == 1) { // Sickness - Green
            br_spell.fill_color[0] = 0.3f;
            br_spell.fill_color[1] = 1.0f;
            br_spell.fill_color[2] = 0.3f;
            spell_name = "SICKNESS";
        }
        else if (spell.spell_id == 2) { // Convert - Red
            br_spell.fill_color[0] = 1.0f;
            br_spell.fill_color[1] = 0.3f;
            br_spell.fill_color[2] = 0.3f;
            spell_name = "CONVERT";
        }

        if (!spell.is_ready) {
            // SPELL IS BEING PREPARED - Show progress bar ΚΑΤΩ ΑΠΟ ΤΟΝ WIZARD

            // Background bar
            graphics::Brush br_bg;
            br_bg.fill_color[0] = 0.2f;
            br_bg.fill_color[1] = 0.2f;
            br_bg.fill_color[2] = 0.2f;
            br_bg.fill_opacity = 0.8f;

            float bar_width = 1.0f;
            float bar_height = 0.12f;

            graphics::drawRect(indicator_x, indicator_y, bar_width, bar_height, br_bg);

            // Progress fill
            float progress = spell.prep_time / 2.0f; // 2 seconds total
            float progress_width = bar_width * progress;
            float progress_x = indicator_x - bar_width / 2 + progress_width / 2;

            br_spell.fill_opacity = 0.8f;
            graphics::drawRect(progress_x, indicator_y, progress_width, bar_height, br_spell);

            // Text
            graphics::Brush br_text;
            br_text.fill_color[0] = 1.0f;
            br_text.fill_color[1] = 1.0f;
            br_text.fill_color[2] = 1.0f;

            std::string prep_text = "Preparing " + spell_name + " " +
                std::to_string((int)(progress * 100)) + "%";
            graphics::drawText(indicator_x - 0.45f, indicator_y + 0.05f, 0.1f, prep_text, br_text);
        }
        else {
            // SPELL IS READY - Show ready indicator ΚΑΤΩ ΑΠΟ ΤΟΝ WIZARD

            // Draw ready circle
            br_spell.fill_opacity = 0.8f;
            float circle_radius = 0.2f;
            graphics::drawDisk(indicator_x, indicator_y, circle_radius, br_spell);

            // Pulsing outline when ready
            graphics::Brush br_outline;
            br_outline.fill_opacity = 0.0f;
            br_outline.outline_color[0] = br_spell.fill_color[0];
            br_outline.outline_color[1] = br_spell.fill_color[1];
            br_outline.outline_color[2] = br_spell.fill_color[2];

            float pulse = 0.3f * sinf(total_time * 5.0f) + 0.7f; // Pulsing effect
            br_outline.outline_opacity = pulse;
            br_outline.outline_width = 3.0f;

            graphics::drawDisk(indicator_x, indicator_y, circle_radius + 0.05f, br_outline);

            // "READY" text ΚΑΤΩ ΑΠΟ ΤΟΝ ΚΥΚΛΟ
            graphics::Brush br_text;
            br_text.fill_color[0] = 1.0f;
            br_text.fill_color[1] = 1.0f;
            br_text.fill_color[2] = 0.0f; // Yellow for READY

            graphics::drawText(indicator_x - 0.2f, indicator_y + 0.4f, 0.1f, "READY", br_text);

            // Optional: Also outline the wizard with spell color
            graphics::Brush br_wizard_outline;
            br_wizard_outline.fill_opacity = 0.0f;
            br_wizard_outline.outline_color[0] = br_spell.fill_color[0];
            br_wizard_outline.outline_color[1] = br_spell.fill_color[1];
            br_wizard_outline.outline_color[2] = br_spell.fill_color[2];
            br_wizard_outline.outline_opacity = 0.6f;
            br_wizard_outline.outline_width = 3.0f;

            graphics::drawDisk(wizard_x, wizard_y, wizard_size + 0.15f, br_wizard_outline);
        }
    }

    // Draw target selection indicator if active
    if (target_selection_wizard) {
        graphics::Brush br_selection;
        br_selection.fill_color[0] = 1.0f;
        br_selection.fill_color[1] = 1.0f;
        br_selection.fill_color[2] = 0.0f;
        br_selection.fill_opacity = 0.0f;
        br_selection.outline_opacity = 1.0f;
        br_selection.outline_width = 4.0f;

        graphics::drawDisk(target_selection_wizard->getX(), target_selection_wizard->getY(),
            target_selection_wizard->getSize() + 0.25f, br_selection);

        // Draw "SELECT TARGET" text below wizard
        graphics::Brush br_text;
        br_text.fill_color[0] = 1.0f;
        br_text.fill_color[1] = 1.0f;
        br_text.fill_color[2] = 0.0f;

        float wizard_x = target_selection_wizard->getX();
        float wizard_y = target_selection_wizard->getY();
        float wizard_size = target_selection_wizard->getSize();

        std::string spell_name;
        if (target_selection_spell_id == 0) spell_name = "FREEZE";
        else if (target_selection_spell_id == 1) spell_name = "SICKNESS";
        else if (target_selection_spell_id == 2) spell_name = "CONVERT";

        graphics::drawText(wizard_x - 0.4f, wizard_y + wizard_size + 0.6f,
            0.12f, "SELECT TARGET FOR " + spell_name, br_text);
    }
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

std::pair<int, int> GlobalState::calculateTotalHealth() const {
    int player_total_health = 0;
    int enemy_total_health = 0;

    for (const auto& entity : entities) {
        if (entity->getSide() == Side::PLAYER) {
            player_total_health += entity->getHealth();
        }
        else if (entity->getSide() == Side::ENEMY) {
            enemy_total_health += entity->getHealth();
        }
    }

    return std::make_pair(player_total_health, enemy_total_health);
}

void GlobalState::drawHealthBars() {
    std::pair<int, int> health_totals = calculateTotalHealth();
    int player_health = health_totals.first;
    int enemy_health = health_totals.second;

    int total_health = player_health + enemy_health;

    if (total_health == 0) return; // Avoid division by zero

    // Σταθερό μέγεθος για το συνολικό health bar
    float total_bar_width = 12.0f;
    float total_bar_height = 0.4f;
    float bar_y_position = 7.5f; // Στο κάτω μέρος της οθόνης (8.0f είναι το κάτω όριο)
    float bar_x_center = 8.0f; // Κέντρο της οθόνης

    // Υπολογισμός ποσοστού
    float player_percentage = static_cast<float>(player_health) / total_health;
    float enemy_percentage = static_cast<float>(enemy_health) / total_health;

    // Υπολογισμός πραγματικών μεγεθών
    float player_bar_width = total_bar_width * player_percentage;
    float enemy_bar_width = total_bar_width * enemy_percentage;

    // 1. Ζωγραφίζουμε το background (μαύρο πλαίσιο)
    graphics::Brush br_background;
    br_background.fill_color[0] = 0.1f;
    br_background.fill_color[1] = 0.1f;
    br_background.fill_color[2] = 0.1f;
    br_background.fill_opacity = 0.8f;
    br_background.outline_opacity = 1.0f;
    br_background.outline_color[0] = 1.0f;
    br_background.outline_color[1] = 1.0f;
    br_background.outline_color[2] = 1.0f;
    br_background.outline_width = 2.0f;

    graphics::drawRect(bar_x_center, bar_y_position, total_bar_width, total_bar_height, br_background);

    // 2. Ζωγραφίζουμε το player health bar (μπλε) - ΑΡΙΣΤΕΡΑ
    if (player_bar_width > 0) {
        graphics::Brush br_player;
        br_player.fill_color[0] = 0.2f;
        br_player.fill_color[1] = 0.2f;
        br_player.fill_color[2] = 1.0f; // Μπλε
        br_player.fill_opacity = 0.9f;
        br_player.outline_opacity = 0.0f;

        // Υπολογίζουμε το κέντρο για το player bar
        float player_bar_x = bar_x_center - (total_bar_width / 2) + (player_bar_width / 2);
        graphics::drawRect(player_bar_x, bar_y_position, player_bar_width, total_bar_height, br_player);
    }

    // 3. Ζωγραφίζουμε το enemy health bar (κόκκινο) - ΔΕΞΙΑ
    if (enemy_bar_width > 0) {
        graphics::Brush br_enemy;
        br_enemy.fill_color[0] = 1.0f; // Κόκκινο
        br_enemy.fill_color[1] = 0.2f;
        br_enemy.fill_color[2] = 0.2f;
        br_enemy.fill_opacity = 0.9f;
        br_enemy.outline_opacity = 0.0f;

        // Υπολογίζουμε το κέντρο για το enemy bar
        float enemy_bar_x = bar_x_center + (total_bar_width / 2) - (enemy_bar_width / 2);
        graphics::drawRect(enemy_bar_x, bar_y_position, enemy_bar_width, total_bar_height, br_enemy);
    }

    // 4. Ζωγραφίζουμε τα κείμενα με τα συνολικά HP
    graphics::Brush br_text;
    br_text.fill_color[0] = 1.0f;
    br_text.fill_color[1] = 1.0f;
    br_text.fill_color[2] = 1.0f;

    // Player HP text (αριστερά του bar)
    std::string player_text = "PLAYER: " + std::to_string(player_health);
    graphics::drawText(bar_x_center - total_bar_width / 2 - 2.0f, bar_y_position + 0.1f, 0.25f, player_text, br_text);

    // Enemy HP text (δεξιά του bar)
    std::string enemy_text = "ENEMY: " + std::to_string(enemy_health);
    graphics::drawText(bar_x_center + total_bar_width / 2 + 0.3f, bar_y_position + 0.1f, 0.25f, enemy_text, br_text);

}

void GlobalState::handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed) {
    if (mouse_pressed) {
        // First check if we're in target selection mode
        if (target_selection_wizard && target_selection_spell_id >= 0) {
            handleWizardSpellCast(canvas_x, canvas_y);
            return;
        }

        // Check if we clicked on a wizard with a ready spell
        for (auto& spell : ready_spells) {
            if (spell.is_ready && spell.wizard) {
                if (spell.wizard->contains(canvas_x, canvas_y)) {
                    // Enter target selection mode for this ready spell
                    target_selection_wizard = spell.wizard;
                    target_selection_spell_id = spell.spell_id;

                    // Remove this spell from ready spells
                    ready_spells.erase(
                        std::remove_if(ready_spells.begin(), ready_spells.end(),
                            [&](const ReadySpell& s) {
                                return s.wizard == spell.wizard && s.spell_id == spell.spell_id;
                            }),
                        ready_spells.end()
                    );

                    return;
                }
            }
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

void GlobalState::draw() {
    graphics::Brush bg;
    bg.fill_color[0] = 0.3f;
    bg.fill_color[1] = 0.6f;
    bg.fill_color[2] = 1.0f;
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

    drawReadySpells();
    drawHealthBars();
}
