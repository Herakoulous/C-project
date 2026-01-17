#pragma once
#include "sgg/graphics.h"
#include <string>

class PauseMenu {
private:
    // Pause state
    bool game_paused;
    bool pause_button_hovered;
    bool resume_button_hovered;
    bool restart_button_hovered;
    bool next_level_hovered;

    // Button click results
    bool resume_clicked;
    bool restart_clicked;
    bool next_level_clicked;

    // Pause button position and size
    float pause_button_x, pause_button_y;
    float pause_button_width, pause_button_height;

    // Pause menu position and size
    float pause_menu_x, pause_menu_y;
    float pause_menu_width, pause_menu_height;

    // Resume/Restart buttons
    float resume_button_x, resume_button_y;
    float restart_button_x, restart_button_y;
    float next_level_button_x, next_level_button_y;
    float button_width, button_height;

public:
    // Constructor
    PauseMenu();

    // Initialization
    void init();

    // Update button positions based on whether next level exists
    void updateButtonPositions(bool has_next_level);

    // Update methods
    void update(float canvas_x, float canvas_y, bool mouse_pressed);

    // Draw methods
    void draw() const;

    // Getters
    bool isGamePaused() const { return game_paused; }
    bool isResumeClicked() { bool val = resume_clicked; resume_clicked = false; return val; }
    bool isRestartClicked() { bool val = restart_clicked; restart_clicked = false; return val; }
    bool isNextLevelClicked() { bool val = next_level_clicked; next_level_clicked = false; return val; }

    // Game control
    void pauseGame();
    void resumeGame();

    // Reset (called when game restarts)
    void reset();

    // Check if point is in pause button (for external use)
    bool isMouseOverPauseButton(float mx, float my) const;

    // Check if point is in next level button
    bool isMouseOverNextLevelButton(float mx, float my) const;
};