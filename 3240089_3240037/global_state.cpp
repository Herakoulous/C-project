#include "global_state.h"
#include "ai_system.h"
#include "sgg/graphics.h"
#include <algorithm>
#include <cmath>

// Αρχικοποίηση static μεταβλητής για Singleton pattern
GlobalState * GlobalState::instance = nullptr;

// Κατασκευαστής - αρχικοποίηση όλων των μελών
GlobalState::GlobalState() : selected_entity(nullptr),
target_selection_wizard(nullptr), target_selection_spell_id(-1),
ai_system(nullptr) {
    // Ο constructor του pause_menu και level_manager καλούνται αυτόματα
}

// Στατική μέθοδος για λήψη του μοναδικού instance (Singleton pattern)
GlobalState* GlobalState::getInstance() {
    if (!instance) {
        instance = new GlobalState();  // Δημιουργία νέου instance αν δεν υπάρχει
    }
    return instance;
}

// Αρχικοποίηση παιχνιδιού - ξεκινάει από level 1
void GlobalState::init() {
    initLevel(1); // Εκκίνηση από το πρώτο level
}

// Αρχικοποίηση συγκεκριμένου level
void GlobalState::initLevel(int level) {
    // ΚΑΘΑΡΙΣΜΟΣ ΟΛΩΝ ΤΩΝ ΔΕΔΟΜΕΝΩΝ
    entities.clear();           // Καθαρισμός όλων των οντοτήτων
    troops.clear();             // Καθαρισμός όλων των troops
    attack_effects.clear();     // Καθαρισμός οπτικών effects
    entity_graph.clear();       // Καθαρισμός γράφου συνδέσεων
    selected_entity = nullptr;  // Μηδενισμός επιλεγμένης οντότητας
    hover_menu.hide();          // Απόκρυψη hover menu
    ready_spells.clear();       // Καθαρισμός ready spells
    target_selection_wizard = nullptr;   // Μηδενισμός επιλεγμένου wizard
    target_selection_spell_id = -1;      // Επαναφορά spell ID
    pause_menu.reset();         // Επαναφορά pause menu

    // Καθαρισμός AI system αν υπάρχει
    if (ai_system) {
        delete ai_system;
        ai_system = nullptr;
    }

    // ΑΡΧΙΚΟΠΟΙΗΣΗ LEVEL
    level_manager.initLevel(level, entities, entity_graph);  // Δημιουργία οντοτήτων και γράφου

    // ΔΗΜΙΟΥΡΓΙΑ AI SYSTEM
    ai_system = new AISystem(this, &entity_graph);
    if (ai_system) {
        ai_system->init();  // Αρχικοποίηση AI
    }

    // Ενημέρωση pause menu για τη διαθεσιμότητα επόμενου level
    pause_menu.setNextLevelAvailable(level_manager.hasNextLevel());

    //Ορισμός αν το exit button είναι διαθέσιμο (μόνο level 3)
    pause_menu.setExitAvailable(level == 3);
}

// Ενημέρωση κατάστασης παιχνιδιού κάθε frame
void GlobalState::update(float dt) {
    // Έλεγχος pause menu πρώτα
    graphics::MouseState mouse;
    graphics::getMouseState(mouse);
    float canvas_x = graphics::windowToCanvasX((float)mouse.cur_pos_x);   // Μετατροπή σε συντεταγμένες καμβά
    float canvas_y = graphics::windowToCanvasY((float)mouse.cur_pos_y);

    pause_menu.update(canvas_x, canvas_y, mouse.button_left_pressed);  // Ενημέρωση pause menu

    // Αν το παιχνίδι είναι paused, διαχείριση μόνο του μενού
    if (pause_menu.isGamePaused()) {
        if (pause_menu.isResumeClicked()) {
            pause_menu.resumeGame();  // Συνέχιση παιχνιδιού
        }
        else if (pause_menu.isRestartClicked()) {
            initLevel(level_manager.getCurrentLevel());  // Επανεκκίνηση τρέχοντος level
        }
        else if (pause_menu.isNextLevelClicked()) {
            // Μετάβαση στο επόμενο level
            int next_level = level_manager.getCurrentLevel() + 1;
            if (next_level <= level_manager.getMaxLevels()) {
                initLevel(next_level);
            }
        }
        //Έλεγχος για exit button (μόνο στο level 3)
        else if (pause_menu.isExitClicked()) {
            // Κλείσιμο του παιχνιδιού
            graphics::stopMessageLoop();
            return;
        }
        return;  // Διακοπή ενημέρωσης αν το παιχνίδι είναι paused

    }

    // ΚΑΝΟΝΙΚΗ ΕΝΗΜΕΡΩΣΗ ΠΑΙΧΝΙΔΙΟΥ

    // Ενημέρωση όλων των οντοτήτων
    for (auto& entity : entities) {
        entity->update(dt);
    }

    // Ενημέρωση όλων των troops
    for (auto& troop : troops) {
        troop->update(dt);
    }

    // Επεξεργασία διαφόρων συστημάτων
    handleTowerAttacks();      // Επιθέσεις πύργων
    handleTroopArrivals();     // Άφιξη troops στους στόχους
    updateAttackEffects(dt);   // Ενημέρωση οπτικών effects
    updateReadySpells(dt);     // Ενημέρωση spells

    // Ενημέρωση AI system αν υπάρχει
    if (ai_system) {
        ai_system->update(dt);
    }

    // Επεξεργασία UI
    handleHoverMenu(canvas_x, canvas_y);                // Hover menu
    handleMouseInput(canvas_x, canvas_y, mouse.button_left_pressed);  // Κλικ ποντικιού
}

// Σχεδίαση όλων των στοιχείων του παιχνιδιού
void GlobalState::draw() {
    // Σχεδίαση φόντου
    graphics::Brush bg;
    bg.fill_color[0] = 0.3f;  // RGB: Γαλάζιο φόντο
    bg.fill_color[1] = 0.6f;
    bg.fill_color[2] = 1.0f;
    graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, bg);

    // Σχεδίαση γράφου συνδέσεων
    entity_graph.draw();

    // Σχεδίαση όλων των οντοτήτων
    for (auto& entity : entities) {
        entity->draw();
    }

    // Σχεδίαση όλων των troops
    for (auto& troop : troops) {
        troop->draw();
    }

    // Σχεδίαση οπτικών effects
    drawAttackEffects();

    // Σχεδίαση highlight για επιλεγμένη οντότητα
    if (selected_entity) {
        graphics::Brush hl;
        hl.fill_color[0] = hl.fill_color[1] = 1.0f; hl.fill_color[2] = 0.0f;  // Κίτρινο περίγραμμα
        hl.fill_opacity = 0.0f;    // Διαφανές εσωτερικό
        hl.outline_opacity = 1.0f; // Ορατό περίγραμμα
        hl.outline_width = 3.0f;   // Πάχος περιγράμματος
        graphics::drawDisk(selected_entity->getX(), selected_entity->getY(),
            selected_entity->getSize() + 0.1f, hl);
    }

    // Σχεδίαση hover menu
    hover_menu.draw();

    // Σχεδίαση spells και health bars
    drawReadySpells();
    drawHealthBars();

    // Σχεδίαση δείκτη level
    graphics::Brush level_br;
    level_br.fill_color[0] = 1.0f;
    level_br.fill_color[1] = 1.0f;
    level_br.fill_color[2] = 1.0f;
    graphics::drawText(0.5f, 0.5f, 0.2f,
        "LEVEL " + std::to_string(level_manager.getCurrentLevel()) +
        "/" + std::to_string(level_manager.getMaxLevels()), level_br);

    // Σχεδίαση pause menu (πάντα ορατό το pause button)
    pause_menu.draw();
}

// Καταστροφέας - καθαρισμός μνήμης
GlobalState::~GlobalState() {
    if (ai_system) {
        delete ai_system;
        ai_system = nullptr;
    }
}

// Ενημέρωση προετοιμασίας spells
void GlobalState::updateReadySpells(float dt) {
    float dt_seconds = dt / 1000.0f;

    // Ενημέρωση χρόνου προετοιμασίας για spells που προετοιμάζονται
    for (auto& spell : ready_spells) {
        if (!spell.is_ready) {
            spell.prep_time += dt_seconds;

            // Έλεγχος ολοκλήρωσης προετοιμασίας (2 δευτερόλεπτα)
            if (spell.prep_time >= 2.0f) {
                spell.is_ready = true;
                spell.prep_time = 2.0f;  // Περικοπή στο μέγιστο
            }
        }
    }

    // Αφαίρεση μη έγκυρων ready spells (wizard πέθανε ή μετατράπηκε)
    ready_spells.erase(
        std::remove_if(ready_spells.begin(), ready_spells.end(),
            [](const ReadySpell& s) {
                return !s.wizard || s.wizard->getSide() != Side::PLAYER;
            }),
        ready_spells.end()
    );
}

// Επεξεργασία troops που έφτασαν στους στόχους τους
void GlobalState::handleTroopArrivals() {
    for (auto it = troops.begin(); it != troops.end(); ) {
        if ((*it)->hasArrived()) {
            Entity* target = (*it)->getTarget();
            Entity* source = (*it)->getSource();
            int amount = (*it)->getHealthAmount();

            if (target->getSide() == source->getSide()) {
                // ΙΔΙΑ ΠΛΕΥΡΑ: Προσθήκη HP
                target->setHealth(target->getHealth() + amount);
            }
            else {
                // ΔΙΑΦΟΡΕΤΙΚΗ ΠΛΕΥΡΑ: Επίθεση
                int damage = std::max(1, amount - (int)target->getDefense());  // Υπολογισμός ζημιάς με άμυνα
                target->setHealth(target->getHealth() - damage);
                if (target->getHealth() < 0) {
                    // Μετατροπή οντότητας αν η ζωή γίνει αρνητική
                    target->setSide(source->getSide());
                    target->setHealth(std::abs(target->getHealth()));
                }
            }

            it = troops.erase(it);  // Αφαίρεση του troop
        }
        else if ((*it)->isDead()) {
            // Αφαίρεση νεκρών troops
            it = troops.erase(it);
        }
        else {
            ++it;
        }
    }
}

// Επεξεργασία επιθέσεων πύργων σε κοντινά troops
void GlobalState::handleTowerAttacks() {
    // Μετατροπή vector smart pointers σε raw pointers για τη συνάρτηση επίθεσης
    std::vector<Troop*> troop_ptrs;
    for (auto& troop : troops) {
        troop_ptrs.push_back(troop.get());
    }

    // Για κάθε πύργο, επίθεση σε κοντινά troops
    for (auto& entity : entities) {
        if (entity->getType() == "Tower") {
            Tower* tower = static_cast<Tower*>(entity.get());
            tower->attackNearbyTroops(troop_ptrs, attack_effects);
        }
    }
}

// Ενημέρωση οπτικών effects επιθέσεων
void GlobalState::updateAttackEffects(float dt) {
    float dt_seconds = dt / 1000.0f;

    for (auto it = attack_effects.begin(); it != attack_effects.end(); ) {
        it->elapsed += dt_seconds;
        if (it->elapsed >= it->duration) {
            it = attack_effects.erase(it);  // Αφαίρεση ολοκληρωμένων effects
        }
        else {
            ++it;
        }
    }
}

// Σχεδίαση οπτικών effects επιθέσεων
void GlobalState::drawAttackEffects() {
    for (const auto& effect : attack_effects) {
        float progress = effect.elapsed / effect.duration;
        float alpha = 1.0f - progress;  // Απόσβεση με το χρόνο

        graphics::Brush br;
        br.fill_color[0] = effect.r;
        br.fill_color[1] = effect.g;
        br.fill_color[2] = effect.b;
        br.fill_opacity = alpha;
        br.outline_opacity = alpha;
        br.outline_width = 3.0f;

        // Υπολογισμός τρέχουσας θέσης (γραμμική παρεμβολή)
        float current_x = effect.start_x + (effect.end_x - effect.start_x) * progress;
        float current_y = effect.start_y + (effect.end_y - effect.start_y) * progress;

        // Σχεδίαση προβολής (δίσκος)
        graphics::drawDisk(current_x, current_y, 0.15f, br);
    }
}

// Διαχείριση hover menu (εμφάνιση όταν γίνεται hover σε οντότητα)
void GlobalState::handleHoverMenu(float canvas_x, float canvas_y) {
    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            if (entity->getSide() == Side::PLAYER) {
                if (entity->getType() == "Baby") {
                    hover_menu.show(entity->getX(), entity->getY() + 1.0f,
                        entity.get(), true, 0);  // Menu για Baby
                }
                else {
                    hover_menu.show(entity->getX(), entity->getY() + 1.3f,
                        entity.get(), false, entity->getUpgradeCost());  // Menu για άλλες οντότητες
                }
            }
            break;
        }
    }

    // Απόκρυψη menu αν ο χρήστης κλικάρει έξω από αυτό
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

// Επεξεργασία κλικ σε menu (αναβάθμιση, μετατροπή, spells)
void GlobalState::handleMenuClick(float canvas_x, float canvas_y, int clicked_option) {
    Entity* target = hover_menu.getTarget();
    bool is_baby = hover_menu.getIsBaby();

    // Wizard spell buttons (IDs 10, 11, 12)
    if (clicked_option >= 10 && clicked_option <= 12) {
        if (target->getType() == "Wizard") {
            Wizard* wizard = static_cast<Wizard*>(target);
            int spell_id = clicked_option - 10; // 10->0, 11->1, 12->2

            if (wizard->canCastSpell(spell_id)) {
                // Έλεγχος αν αυτό το spell είναι ήδη ready
                bool already_ready = false;
                for (auto& spell : ready_spells) {
                    if (spell.wizard == wizard && spell.spell_id == spell_id) {
                        already_ready = true;
                        break;
                    }
                }

                if (!already_ready) {
                    // Έναρξη προετοιμασίας spell - ΑΛΛΑΓΗ: ΔΕΝ ΠΛΗΡΩΝΟΥΜΕ HP ΕΔΩ
                    ReadySpell new_spell;
                    new_spell.wizard = wizard;
                    new_spell.spell_id = spell_id;
                    new_spell.is_ready = false;
                    new_spell.prep_time = 0.0f; // Αρχή από 0
                    ready_spells.push_back(new_spell);

                    // ΑΦΑΙΡΕΙΣΗ: ΔΕΝ ΑΦΑΙΡΟΥΜΕ HP ΕΔΩ
                    // wizard->setHealth(wizard->getHealth() - wizard->getSpellCost(spell_id));

                    // Έναρξη cooldown αμέσως
                    wizard->castSpell(spell_id, nullptr); // nullptr για μόνο cooldown
                }

                hover_menu.hide();
                return;
            }
        }
        hover_menu.hide();
        return;
    }

    // ΕΠΕΞΕΡΓΑΣΙΑ ΜΕΤΑΤΡΟΠΗΣ BABY
    if (is_baby) {
        if (clicked_option == 0 && target->getHealth() >= 5) {
            // ΜΕΤΑΤΡΟΠΗ ΣΕ WARRIOR (5 HP)
            int hp = target->getHealth() - 5;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    // Αποθήκευση παλιών συνδέσεων
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    // Αντικατάσταση Baby με Warrior
                    entities[i] = std::make_unique<Warrior>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    // Επανασύνδεση με παλιούς γείτονες
                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    // Σύνδεση με όλες τις άλλες οντότητες
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
            // ΜΕΤΑΤΡΟΠΗ ΣΕ TOWER (10 HP)
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
            // ΜΕΤΑΤΡΟΠΗ ΣΕ WIZARD (10 HP)
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
        // ΑΝΑΒΑΘΜΙΣΗ ΟΝΤΟΤΗΤΩΝ (Warrior, Tower, Wizard)
        if (target->canUpgrade()) {
            target->performUpgrade();  // Εκτέλεση αναβάθμισης
            hover_menu.hide();
        }
        else {
            hover_menu.hide();
        }
    }

    // Επανυπολογισμός μονοπατιών μετά από αλλαγές
    entity_graph.calculatePaths(entities);
}

// Επεξεργασία ρίψης spell (όταν επιλέγεται στόχος)
void GlobalState::handleWizardSpellCast(float canvas_x, float canvas_y) {
    if (!target_selection_wizard || target_selection_spell_id < 0) return;

    // Εύρεση οντότητας που κλικάρθηκε
    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            // Έλεγχος αν ο στόχος είναι ο ίδιος με τον wizard
            if (entity.get() == target_selection_wizard) {
                // Δεν επιτρέπεται να ρίξεις spell στον εαυτό σου
                // Το spell επιστρέφει στη ready λίστα
                ReadySpell spell;
                spell.wizard = target_selection_wizard;
                spell.spell_id = target_selection_spell_id;
                spell.is_ready = true;
                spell.prep_time = 2.0f;  // Ολοκληρωμένη προετοιμασία
                ready_spells.push_back(spell);

                target_selection_wizard = nullptr;
                target_selection_spell_id = -1;
                return;
            }

            // ΠΛΗΡΩΜΑ HP ΟΤΑΝ ΧΡΗΣΙΜΟΠΟΙΕΙΣ ΤΟ SPELL (όχι όταν ξεκινά)
            // Έλεγχος αν ο wizard έχει αρκετά HP για το spell
            int spell_cost = target_selection_wizard->getSpellCost(target_selection_spell_id);
            if (target_selection_wizard->getHealth() < spell_cost) {
                // Δεν έχει αρκετά HP, επιστροφή spell
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

            // Ρίψη spell σε αυτή την οντότητα
            Entity* target = entity.get();

            // Εφαρμογή effect spell ανάλογα με το spell_id
            switch (target_selection_spell_id) {
            case 0: // Freeze - μόνιμα NEUTRAL
                target->setSide(Side::NEUTRAL);
                break;
            case 1: // Sickness - max_health = 0
                if (!target->isSick()) {
                    target->setSick(true);
                    target->setMaxHealth(0);
                }
                break;
            case 2: // Convert - μόνιμα PLAYER
                target->setSide(Side::PLAYER);
                break;
            }

            // Επαναφορά επιλογής - το spell χρησιμοποιήθηκε
            target_selection_wizard = nullptr;
            target_selection_spell_id = -1;
            return;
        }
    }

    // Κλικ σε άδειο χώρο - ακύρωση επιλογής αλλά το spell παραμένει ready
    // Επιστροφή στη ready_spells
    ReadySpell spell;
    spell.wizard = target_selection_wizard;
    spell.spell_id = target_selection_spell_id;
    spell.is_ready = true;
    spell.prep_time = 2.0f;  // Ολοκληρωμένη προετοιμασία
    ready_spells.push_back(spell);

    target_selection_wizard = nullptr;
    target_selection_spell_id = -1;
}

// Σχεδίαση spells που είναι έτοιμα για χρήση
void GlobalState::drawReadySpells() {
    static float total_time = 0.0f;
    total_time += 0.016f;  // Προσεγγιστικός χρόνος frame για animation

    for (const auto& spell : ready_spells) {
        if (!spell.wizard) continue;

        float wizard_x = spell.wizard->getX();
        float wizard_y = spell.wizard->getY();
        float wizard_size = spell.wizard->getSize();

        // Θέση δείκτη ΚΑΤΩ από τον wizard
        float indicator_y = wizard_y + wizard_size + 0.5f;
        float indicator_x = wizard_x;

        // Προσδιορισμός χρώματος και ονόματος spell
        graphics::Brush br_spell;
        std::string spell_name;

        if (spell.spell_id == 0) { // Freeze - Μπλε
            br_spell.fill_color[0] = 0.3f;
            br_spell.fill_color[1] = 0.3f;
            br_spell.fill_color[2] = 1.0f;
            spell_name = "FREEZE";
        }
        else if (spell.spell_id == 1) { // Sickness - Πράσινο
            br_spell.fill_color[0] = 0.3f;
            br_spell.fill_color[1] = 1.0f;
            br_spell.fill_color[2] = 0.3f;
            spell_name = "SICKNESS";
        }
        else if (spell.spell_id == 2) { // Convert - Κόκκινο
            br_spell.fill_color[0] = 1.0f;
            br_spell.fill_color[1] = 0.3f;
            br_spell.fill_color[2] = 0.3f;
            spell_name = "CONVERT";
        }

        if (!spell.is_ready) {
            // SPELL ΠΡΟΕΤΟΙΜΑΖΕΤΑΙ - Εμφάνιση progress bar ΚΑΤΩ ΑΠΟ ΤΟΝ WIZARD

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
            float progress = spell.prep_time / 2.0f;  // 2 δευτερόλεπτα συνολικά
            float progress_width = bar_width * progress;
            float progress_x = indicator_x - bar_width / 2 + progress_width / 2;

            br_spell.fill_opacity = 0.8f;
            graphics::drawRect(progress_x, indicator_y, progress_width, bar_height, br_spell);

            // Κείμενο
            graphics::Brush br_text;
            br_text.fill_color[0] = 1.0f;
            br_text.fill_color[1] = 1.0f;
            br_text.fill_color[2] = 1.0f;

            std::string prep_text = "Preparing " + spell_name + " " +
                std::to_string((int)(progress * 100)) + "%";
            graphics::drawText(indicator_x - 0.45f, indicator_y + 0.05f, 0.1f, prep_text, br_text);
        }
        else {
            // SPELL ΕΤΟΙΜΟ - Εμφάνιση έτοιμου δείκτη ΚΑΤΩ ΑΠΟ ΤΟΝ WIZARD

            // Σχεδίαση έτοιμου κύκλου
            br_spell.fill_opacity = 0.8f;
            float circle_radius = 0.2f;
            graphics::drawDisk(indicator_x, indicator_y, circle_radius, br_spell);

            // Παλλόμενο περίγραμμα όταν είναι ready
            graphics::Brush br_outline;
            br_outline.fill_opacity = 0.0f;
            br_outline.outline_color[0] = br_spell.fill_color[0];
            br_outline.outline_color[1] = br_spell.fill_color[1];
            br_outline.outline_color[2] = br_spell.fill_color[2];

            float pulse = 0.3f * sinf(total_time * 5.0f) + 0.7f;  // Εφέ παλμού
            br_outline.outline_opacity = pulse;
            br_outline.outline_width = 3.0f;

            graphics::drawDisk(indicator_x, indicator_y, circle_radius + 0.05f, br_outline);

            // Κείμενο "READY" ΚΑΤΩ ΑΠΟ ΤΟΝ ΚΥΚΛΟ
            graphics::Brush br_text;
            br_text.fill_color[0] = 1.0f;
            br_text.fill_color[1] = 1.0f;
            br_text.fill_color[2] = 0.0f;  // Κίτρινο για READY

            graphics::drawText(indicator_x - 0.2f, indicator_y + 0.4f, 0.1f, "READY", br_text);

            // Προαιρετικό: Περίγραμμα του wizard με το χρώμα του spell
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

    // Σχεδίαση δείκτη επιλογής στόχου αν είναι ενεργός
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

        // Κείμενο "SELECT TARGET" κάτω από τον wizard
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

// Επεξεργασία κλικ σε οντότητα (για αποστολή troops)
void GlobalState::handleEntityClick(float canvas_x, float canvas_y) {
    bool clicked_entity = false;

    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            clicked_entity = true;

            if (selected_entity && selected_entity != entity.get()) {
                // Αποστολή troops από επιλεγμένη οντότητα σε αυτή που κλικάρθηκε
                if (selected_entity->getSide() == Side::PLAYER) {
                    int amount = selected_entity->getHealth() / 2;  // 50% της ζωής
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
                // Επιλογή ή αποεπιλογή οντότητας
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
        selected_entity = nullptr;  // Αποεπιλογή αν κλικάρεται άδειος χώρος
    }
}

// Υπολογισμός συνολικής ζωής παίκτη και εχθρού
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

// Σχεδίαση health bars (συνολική ζωή παίκτη vs εχθρού)
void GlobalState::drawHealthBars() {
    std::pair<int, int> health_totals = calculateTotalHealth();
    int player_health = health_totals.first;
    int enemy_health = health_totals.second;

    int total_health = player_health + enemy_health;

    if (total_health == 0) return;  // Αποφυγή διαίρεσης με το μηδέν

    // ΣΤΑΘΕΡΟ ΜΕΓΕΘΟΣ για το συνολικό health bar
    float total_bar_width = 12.0f;
    float total_bar_height = 0.4f;
    float bar_y_position = 7.5f;  // Στο κάτω μέρος της οθόνης
    float bar_x_center = 8.0f;    // Κέντρο της οθόνης

    // Υπολογισμός ποσοστού
    float player_percentage = static_cast<float>(player_health) / total_health;
    float enemy_percentage = static_cast<float>(enemy_health) / total_health;

    // Υπολογισμός πραγματικών μεγεθών
    float player_bar_width = total_bar_width * player_percentage;
    float enemy_bar_width = total_bar_width * enemy_percentage;

    // 1. Background (μαύρο πλαίσιο)
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

    // 2. Player health bar (μπλε) - ΑΡΙΣΤΕΡΑ
    if (player_bar_width > 0) {
        graphics::Brush br_player;
        br_player.fill_color[0] = 0.2f;
        br_player.fill_color[1] = 0.2f;
        br_player.fill_color[2] = 1.0f;  // Μπλε
        br_player.fill_opacity = 0.9f;
        br_player.outline_opacity = 0.0f;

        // Υπολογισμός κέντρου για το player bar
        float player_bar_x = bar_x_center - (total_bar_width / 2) + (player_bar_width / 2);
        graphics::drawRect(player_bar_x, bar_y_position, player_bar_width, total_bar_height, br_player);
    }

    // 3. Enemy health bar (κόκκινο) - ΔΕΞΙΑ
    if (enemy_bar_width > 0) {
        graphics::Brush br_enemy;
        br_enemy.fill_color[0] = 1.0f;  // Κόκκινο
        br_enemy.fill_color[1] = 0.2f;
        br_enemy.fill_color[2] = 0.2f;
        br_enemy.fill_opacity = 0.9f;
        br_enemy.outline_opacity = 0.0f;

        // Υπολογισμός κέντρου για το enemy bar
        float enemy_bar_x = bar_x_center + (total_bar_width / 2) - (enemy_bar_width / 2);
        graphics::drawRect(enemy_bar_x, bar_y_position, enemy_bar_width, total_bar_height, br_enemy);
    }

    // 4. Κείμενο με τα συνολικά HP
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

// Επεξεργασία εισόδου από ποντίκι
void GlobalState::handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed) {
    if (mouse_pressed) {
        // Έλεγχος αν είμαστε σε λειτουργία επιλογής στόχου για spell
        if (target_selection_wizard && target_selection_spell_id >= 0) {
            handleWizardSpellCast(canvas_x, canvas_y);
            return;
        }

        // Έλεγχος αν κλικάρεται wizard με έτοιμο spell
        for (auto& spell : ready_spells) {
            if (spell.is_ready && spell.wizard) {
                if (spell.wizard->contains(canvas_x, canvas_y)) {
                    // Είσοδος σε λειτουργία επιλογής στόχου για αυτό το ready spell
                    target_selection_wizard = spell.wizard;
                    target_selection_spell_id = spell.spell_id;

                    // Αφαίρεση αυτού του spell από τη ready λίστα
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

        // Έλεγχος κλικ σε hover menu
        int clicked_option = hover_menu.checkClick(canvas_x, canvas_y);
        if (clicked_option != -1) {
            handleMenuClick(canvas_x, canvas_y, clicked_option);
        }
        else {
            handleEntityClick(canvas_x, canvas_y);  // Κλικ σε οντότητα
        }
    }
}