#pragma once
#include "entity.h"
#include <vector>
#include <memory>

// Forward declaration για το EntityGraph
class EntityGraph;

// Κλάση για διαχείριση επιπέδων (levels) του παιχνιδιού
// Αυτή η κλάση είναι υπεύθυνη για τη δημιουργία και αρχικοποίηση κάθε level
class LevelManager {
private:
    int current_level; // Τρέχον level (1, 2, ή 3)
    int max_levels; // Μέγιστος αριθμός επιπέδων (3)

public:
    LevelManager();

    // Διαχείριση των levels
    void initLevel(int level, std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph); // Αρχικοποίηση συγκεκριμένου level
    bool hasNextLevel() const; // Έλεγχος αν υπάρχει επόμενο level

    //Getters
    int getCurrentLevel() const; // Επιστροφή τρέχοντος level
    int getMaxLevels() const; // Επιστροφή μέγιστου αριθμού επιπέδων

private:
    // Βοηθητικές συναρτήσεις για αρχικοποίηση κάθε level
    void initLevel1(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph); //Level 1
    void initLevel2(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph); //Level 2
	void initLevel3(std::vector<std::unique_ptr<Entity>>& entities, EntityGraph& graph); //Level 3
};