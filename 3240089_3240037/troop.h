#pragma once
#include "node.h"
#include <vector>
#include "entity.h"

// Forward declarations (αποφυγή circular dependencies)
enum class Side;
class Entity;

// Κλάση για τα troops που ταξιδεύουν μεταξύ οντοτήτων
// Κληρονομεί από  την Node και αναπαριστά μια ομάδα hp που μετακινείται
class Troop : public Node {
private:
    Entity* source; // Entity πηγής που στέλνει το troop
	Entity* target; // Entity στόχος που λαμβάνει το troop
    int health_amount; // Ποσότητα hp που μεταφέρει το troop
    std::vector<std::pair<float, float>> waypoints; // Μονοπάτι ταξιδιού
    float progress; // Πρόοδος στο μονοπάτι
    float speed; // Ταχύτητα κίνησης (εξαρτάται από attacking_speed της πηγής)
    bool arrived; // Αν έφτασε στον στόχο

    // Εσωτερική συνάρτηση για εύρεση θέσης σε συγκεκριμένο χρόνο
    std::pair<float, float> interpolatePosition(float t);

public:
	// Constructor
    Troop(Entity* src, Entity* tgt, int amount, const std::vector<std::pair<float, float>>& path);

    // Βασικές συναρτήσεις από τη Node
    void update(float dt) override;
    void draw() override;
    bool contains(float mx, float my) const override;

    // Getters
    bool hasArrived() const { return arrived; }
    Entity* getTarget() const { return target; }
    Entity* getSource() const { return source; }
    int getHealthAmount() const { return health_amount; }

    // Συναρτήσεις για ζημιά από πύργους
    void reduceHealth(int amount);
    bool isDead() const { return health_amount <= 0; }
};