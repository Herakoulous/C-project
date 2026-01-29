#pragma once
#include "sgg/graphics.h"
#include <string>

// Κλάση για το pause menu του παιχνιδιού
// Χειρίζεται το pause button και το μενού επιλογών όταν το παιχνίδι είναι paused
class PauseMenu {
private:
    // Κατάσταση παύσης         //Οι παρακάτω συναρτησεις ελέγχουν αν:
    bool game_paused;           // το παιχνίδι είναι paused
    bool pause_button_hovered;  // ο δείκτης είναι πάνω στο pause button
    bool resume_button_hovered; // ο δείκτης είναι πάνω στο resume button
    bool restart_button_hovered;// ο δείκτης είναι πάνω στο restart button
    bool next_level_hovered;    // ο δείκτης είναι πάνω στο next level button
    bool exit_button_hovered;

    // Αποτελέσματα κλικ σε buttons (resume, restart, next level (αν είναι διαθέσιμο-υπάρχει))
    bool resume_clicked;        
    bool restart_clicked;       
    bool next_level_clicked;    
    bool exit_clicked;

    bool next_level_available;  
    bool exit_available;

    // Θέση και μέγεθος pause button
    float pause_button_x, pause_button_y;
    float pause_button_width, pause_button_height;

    // Θέση και μέγεθος pause menu
    float pause_menu_x, pause_menu_y;
    float pause_menu_width, pause_menu_height;

    // Θέσεις buttons στο μενού
    float resume_button_x, resume_button_y;
    float restart_button_x, restart_button_y;
    float next_level_button_x, next_level_button_y;
    float exit_button_x, exit_button_y;
    float button_width, button_height;  // Μέγεθος όλων των buttons

public:
    //constructor
    PauseMenu();

    void init();

    void setExitAvailable(bool available);

    // Ενημέρωση θέσεων buttons ανάλογα με το αν υπάρχει επόμενο level
    void updateButtonPositions(bool has_next_level);

    // Ενημέρωση κατάστασης (κλικ, hover) κάθε frame
    void update(float canvas_x, float canvas_y, bool mouse_pressed);

    // Σχεδίαση του pause button και του μενού (αν είναι visible)
    void draw() const;

    // Setters/Getters
    void setNextLevelAvailable(bool available);
    bool isGamePaused() const { return game_paused; }
    bool isResumeClicked() { bool val = resume_clicked; resume_clicked = false; return val; }
    bool isRestartClicked() { bool val = restart_clicked; restart_clicked = false; return val; }
    bool isNextLevelClicked() { bool val = next_level_clicked; next_level_clicked = false; return val; }
    bool isExitClicked() { bool val = exit_clicked; exit_clicked = false; return val; }

    // Έλεγχος παύσης/συνέχισης παιχνιδιού
    void pauseGame();
    void resumeGame();

    // Επαναφορά κατάστασης (όταν ξεκινάει νέο level)
    void reset();

    // Έλεγχος αν ο δείκτης είναι πάνω στο pause button
    bool isMouseOverPauseButton(float mx, float my) const;

    // Έλεγχος αν ο δείκτης είναι πάνω στο next level button
    bool isMouseOverNextLevelButton(float mx, float my) const;
};