#pragma once
#include "entity.h"
#include <vector>

// Κλάση για το hover menu που εμφανίζεται όταν ο παίκτης κάνει hover σε entity
// Προσφερει επιλογές αναβαθμισης ή μετατροπής (για Baby)
class HoverMenu {
private:
    float x, y;                 // Θέση στην οθόνη
    bool visible = false;       // Αν το menu είναι ορατό
    Entity* target = nullptr;   // Οντότητα στην οποία αναφέρεται το menu
    bool is_baby;               // Αν η οντότητα είναι Baby
    int cost;                   // Κόστος αναβάθμισης (αν υπάρχει)

    // Δομή για τα buttons του menu
    struct Button {
        float center_x, center_y;  // Κέντρο
        float width, height;       // Μέγεθος
        int option_id;             // ID επιλογής (0, 1, 2 ή 10, 11, 12 για spells)
    };
    std::vector<Button> buttons;  // Λίστα όλων των buttons στο menu

public:
    // Εμφάνιση menu σε συγκεκριμένη θέση για συγκεκριμένη οντότητα
    void show(float menu_x, float menu_y, Entity* entity, bool baby, int cost);

    // Έλεγχος αν έγινε click αε κάποιο button του menu
    int checkClick(float mx, float my);

    // Απόκρυψη menu
    void hide();

    // Σχεδίαση menu
    void draw();

    // Getters
    int getUpgradeCost() const { return cost; }
    bool isVisible() const { return visible; }
    Entity* getTarget() const { return target; }
    bool getIsBaby() const { return is_baby; }

    float getX() const { return x; }
    float getY() const { return y; }
};