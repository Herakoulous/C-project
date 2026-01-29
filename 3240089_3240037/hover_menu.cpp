#include "hover_menu.h"
#include "sgg/graphics.h"

// Εμφάνιση hover menu σε συγκεκριμένη θέση για συγκεκριμένη οντότητα
void HoverMenu::show(float menu_x, float menu_y, Entity* entity, bool baby, int upgrade_cost) {
    x = menu_x;
    y = menu_y;
    visible = true;
    target = entity;
    is_baby = baby;
    cost = upgrade_cost;
    buttons.clear();  // Καθαρισμός παλιών buttons

    // Δημιουργία buttons ανάλογα με τον τύπο της οντότητας
    if (is_baby) {
        // Menu για Baby: 3 επιλογές (Warrior, Tower, Wizard) σε τριγωνική διάταξη
        buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 0 });    // Αριστερό - Warrior
        buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 1 });           // Κεντρικό - Tower  
        buttons.push_back({ x + 0.5f, y - 1.3f, 0.4f, 0.4f, 2 });    // Δεξιό - Wizard
    }
    else if (entity->getType() == "Wizard") {
        // Menu για Wizard: spells και αναβάθμιση, διαφορετικό ανάλογα με το level
        Wizard* wizard = static_cast<Wizard*>(entity);
        int level = wizard->getLevel();

        // Επίπεδο 1: Freeze (αριστερά) + Αναβάθμιση (κέντρο)
        if (level == 1) {
            buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 10 }); // ID 10 = Freeze spell
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 });         // ID 0 = Αναβάθμιση
        }
        // Επίπεδο 2: Freeze (αριστερά), Αναβάθμιση (κέντρο), Sickness (δεξιά)
        else if (level == 2) {
            buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 10 }); // Freeze
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 });         // Αναβάθμιση
            buttons.push_back({ x + 0.5f, y - 1.3f, 0.4f, 0.4f, 11 }); // ID 11 = Sickness spell
        }
        // Επίπεδο 3: Freeze (αριστερά), Convert (κέντρο), Sickness (δεξιά) - ΧΩΡΙΣ ΑΝΑΒΑΘΜΙΣΗ
        else if (level >= 3) {
            buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 10 }); // Freeze
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 12 });         // ID 12 = Convert spell
            buttons.push_back({ x + 0.5f, y - 1.3f, 0.4f, 0.4f, 11 }); // Sickness
        }
    }
    else if (entity->getType() == "Warrior") {
        // Menu για Warrior: μόνο αναβάθμιση (αν level < 5)
        Warrior* warrior = static_cast<Warrior*>(entity);
        int level = warrior->getLevel();

        // Εμφάνιση button μόνο αν level < 5 (μέγιστο επίπεδο)
        if (level < 5) {
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 }); // Κεντρικό - Αναβάθμιση
        }
        else {
            // Επίπεδο 5 - απόκρυψη menu (δεν υπάρχουν άλλες αναβαθμίσεις)
            visible = false;
        }
    }
    else if (entity->getType() == "Tower") {
        // Menu για Tower: μόνο αναβάθμιση (αν level < 5)
        Tower* tower = static_cast<Tower*>(entity);
        int level = tower->getLevel();

        // Εμφάνιση button μόνο αν level < 5 (μέγιστο επίπεδο)
        if (level < 5) {
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 }); // Κεντρικό - Αναβάθμιση
        }
        else {
            // Επίπεδο 5 - απόκρυψη menu (δεν υπάρχουν άλλες αναβαθμίσεις)
            visible = false;
        }
    }
}

// Έλεγχος αν ο χρήστης κλικάρει σε κάποιο button του menu
int HoverMenu::checkClick(float mx, float my) {
    if (!visible) return -1;  // Αν το menu δεν είναι ορατό, επιστροφή -1

    for (size_t i = 0; i < buttons.size(); i++) {
        const Button& btn = buttons[i];
        // Έλεγχος αν το σημείο κλικ βρίσκεται μέσα στα όρια του button
        if (mx >= btn.center_x - btn.width / 2 && mx <= btn.center_x + btn.width / 2 &&
            my >= btn.center_y - btn.height / 2 && my <= btn.center_y + btn.height / 2) {
            return btn.option_id;  // Επιστροφή ID του button που πατήθηκε
        }
    }
    return -1;  // Κανένα button δεν πατήθηκε
}

// Απόκρυψη του hover menu
void HoverMenu::hide() {
    visible = false;
    target = nullptr;  // Μηδενισμός δείκτη προς οντότητα
}

// Σχεδίαση του hover menu
void HoverMenu::draw() {
    if (!visible || !target) return;  // Μη σχεδίαση αν δεν είναι ορατό ή δεν έχει στόχο

    float x = target->getX();
    float y = target->getY();
    graphics::Brush br_back, br_char, br_text;

    // Προκαθορισμένα χρώματα για διάφορα στοιχεία
    br_back.fill_color[0] = 0.1f; br_back.fill_color[1] = 0.1f; br_back.fill_color[2] = 0.2f;  // Σκούρο μπλε background
    br_char.fill_color[0] = 0.1f; br_char.fill_color[1] = 0.1f; br_char.fill_color[2] = 1.0f;  // Φωτεινό μπλε για στοιχεία
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;  // ’σπρο για κείμενο

    if (is_baby) {
        // ΜΕΝΟΥ ΓΙΑ BABY: 3 επιλογές μετατροπής

        // ΚΥΚΛΟΣ (5 HP) - για μετατροπή σε Warrior
        graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);  // Background
        graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);        // Μπλε δίσκος
        graphics::drawText(x - 0.52f, y + 1.1f - 1.20f, 0.15f, "5", br_text); // Κείμενο "5" (κόστος)

        // ΤΕΤΡΑΓΩΝΟ (10 HP) - για μετατροπή σε Tower
        graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);         // Background
        graphics::drawRect(x, y + 1.1f - 1.5f, 0.25f, 0.25f, br_char);       // Μπλε τετράγωνο
        graphics::drawText(x - 0.06f, y + 1.1f - 1.4f, 0.15f, "10", br_text); // Κείμενο "10" (κόστος)

        // ΤΡΙΓΩΝΟ (10 HP) - για μετατροπή σε Wizard
        graphics::drawRect(x + 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);  // Background
        graphics::drawSector(x + 0.5f, y + 1.1f - 1.45f, 0, 0.3f, 240, 300, br_char); // Μπλε τρίγωνο (μέρος κύκλου)
        br_back.outline_color[0] = 0.0f; br_back.outline_color[1] = 0.0f; br_back.outline_color[2] = 0.0f;
        graphics::drawRect(x + 0.5f, y + 1.1f - 1.15f, 0.3f, 0.08f, br_back); // Κάλυψη για να φαίνεται σαν τρίγωνο
        graphics::drawLine(x + 0.35f, y + 1.1f - 1.2f, x + 0.65f, y + 1.1f - 1.2f, br_text); // Γραμμή για οπτική βελτίωση
        graphics::drawText(x + 0.44f, y + 1.1f - 1.20f, 0.15f, "10", br_text); // Κείμενο "10" (κόστος)
    }
    else if (target->getType() == "Wizard") {
        // ΜΕΝΟΥ ΓΙΑ WIZARD: spells και αναβάθμιση
        Wizard* wizard = static_cast<Wizard*>(target);
        int level = wizard->getLevel();

        // ΕΠΙΠΕΔΟ 1: Freeze + Αναβάθμιση
        if (level == 1) {
            // FREEZE BUTTON (ΑΡΙΣΤΕΡΑ) - Μπλε με progress bar για cooldown
            graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float freeze_cd = wizard->getSpellCooldown(0);
            if (freeze_cd > 0.0f) {
                // Spell σε cooldown - γκρι δίσκος
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                // Προοδευτική μπάρα (κυκλική) που δείχνει το υπόλοιπο cooldown
                float cooldown_progress = 1.0f - (freeze_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawSector(x - 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                // Χρονόμετρο cooldown
                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.55f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)freeze_cd + 1), br_text);
            }
            else {
                // Έτοιμο για χρήση - κανονικό μπλε δίσκο
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                // Κείμενο κόστους (25 HP)
                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(0)) {
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);  // ’σπρο αν μπορεί
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);  // Κόκκινο αν δεν μπορεί
                }
            }

            // UPGRADE BUTTON (ΚΕΝΤΡΟ) - Αναβάθμιση Wizard
            graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);
            graphics::drawSector(x, y + 1.1f - 1.65f, 0, 0.25f, 240, 300, br_char); // Μπλε τρίγωνο
            br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
            graphics::drawRect(x, y + 1.1f - 1.4f, 0.3f, 0.08f, br_back);  // Κάλυψη
            graphics::drawLine(x - 0.12f, y + 1.1f - 1.45f, x + 0.13f, y + 1.1f - 1.45f, br_text); // Γραμμή
            graphics::drawText(x - 0.17f, y + 1.1f - 1.32f, 0.12f, std::to_string(cost), br_text); // Κόστος αναβάθμισης
            graphics::drawRect(x, y + 1.1f - 1.41f, 0.08f, 0.08f, br_char);  // Μικρό τετράγωνο
        }
        // ΕΠΙΠΕΔΟ 2: Freeze + Αναβάθμιση + Sickness
        else if (level == 2) {
            // FREEZE BUTTON (ΑΡΙΣΤΕΡΑ) - Ίδια λογική με level 1
            graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float freeze_cd = wizard->getSpellCooldown(0);
            if (freeze_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (freeze_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawSector(x - 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.55f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)freeze_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(0)) {
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
            }

            // UPGRADE BUTTON (ΚΕΝΤΡΟ) - Ίδια λογική με level 1
            graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);
            graphics::drawSector(x, y + 1.1f - 1.65f, 0, 0.25f, 240, 300, br_char);
            br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
            graphics::drawRect(x, y + 1.1f - 1.4f, 0.3f, 0.08f, br_back);
            graphics::drawLine(x - 0.12f, y + 1.1f - 1.45f, x + 0.13f, y + 1.1f - 1.45f, br_text);
            graphics::drawText(x - 0.17f, y + 1.1f - 1.32f, 0.12f, std::to_string(cost), br_text);
            graphics::drawRect(x, y + 1.1f - 1.41f, 0.08f, 0.08f, br_char);

            // SICKNESS BUTTON (ΔΕΞΙΑ) - Πράσινο με progress bar
            graphics::drawRect(x + 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float sickness_cd = wizard->getSpellCooldown(1);
            if (sickness_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (sickness_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawSector(x + 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x + 0.45f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)sickness_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(1)) {
                    graphics::drawText(x + 0.47f, y + 1.1f - 1.17f, 0.12f, "30", br_text);  // Κόστος 30 HP
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x + 0.47f, y + 1.1f - 1.17f, 0.12f, "30", br_text);  // Κόκκινο αν δεν μπορεί
                }
            }
        }
        // ΕΠΙΠΕΔΟ 3: Freeze + Convert + Sickness (ΧΩΡΙΣ ΑΝΑΒΑΘΜΙΣΗ)
        else if (level >= 3) {
            // FREEZE BUTTON (ΑΡΙΣΤΕΡΑ) - Ίδια λογική
            graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float freeze_cd = wizard->getSpellCooldown(0);
            if (freeze_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (freeze_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawSector(x - 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.55f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)freeze_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(0)) {
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
            }

            // CONVERT BUTTON (ΚΕΝΤΡΟ) - Κόκκινο με progress bar
            graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);

            float convert_cd = wizard->getSpellCooldown(2);
            if (convert_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x, y + 1.1f - 1.5f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (convert_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 1.0f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 0.3f;
                graphics::drawSector(x, y + 1.1f - 1.5f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.12f, y + 1.1f - 1.35f, 0.12f, std::to_string((int)convert_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 1.0f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 0.3f;
                graphics::drawDisk(x, y + 1.1f - 1.5f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(2)) {
                    graphics::drawText(x - 0.07f, y + 1.1f - 1.35f, 0.12f, "50", br_text);  // Κόστος 50 HP
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.07f, y + 1.1f - 1.35f, 0.12f, "50", br_text);  // Κόκκινο αν δεν μπορεί
                }
            }

            // SICKNESS BUTTON (ΔΕΞΙΑ) - Ίδια λογική με level 2
            graphics::drawRect(x + 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float sickness_cd = wizard->getSpellCooldown(1);
            if (sickness_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (sickness_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawSector(x + 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x + 0.45f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)sickness_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(1)) {
                    graphics::drawText(x + 0.47f, y - 0.1f, 0.12f, "30", br_text);  // Κόστος 30 HP
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x + 0.47f, y - 0.1f, 0.12f, "30", br_text);  // Κόκκινο αν δεν μπορεί
                }
            }
        }
    }
    else {
        // ΚΑΝΟΝΙΚΟ UPGRADE BUTTON για άλλες οντότητες (Warrior, Tower)
        graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);  // Background
        graphics::drawSector(x, y + 1.1f - 1.65f, 0, 0.25f, 240, 300, br_char);  // Μπλε τρίγωνο
        br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
        graphics::drawRect(x, y + 1.1f - 1.4f, 0.3f, 0.08f, br_back);  // Κάλυψη
        graphics::drawLine(x - 0.12f, y + 1.1f - 1.45f, x + 0.13f, y + 1.1f - 1.45f, br_text);  // Γραμμή
        graphics::drawText(x - 0.17f, y + 1.1f - 1.32f, 0.12f, std::to_string(cost), br_text);  // Κόστος αναβάθμισης
        graphics::drawRect(x, y + 1.1f - 1.41f, 0.08f, 0.08f, br_char);  // Μικρό τετράγωνο
    }
}