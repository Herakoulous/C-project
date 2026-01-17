#include "pause_menu.h"
#include "sgg/graphics.h"
#include <cmath>

// Constants
const float PAUSE_BUTTON_SIZE = 0.5f;
const float PAUSE_MENU_WIDTH = 3.0f;
const float PAUSE_MENU_HEIGHT = 2.5f;
const float BUTTON_WIDTH = 2.0f;
const float BUTTON_HEIGHT = 0.5f;
const float BUTTON_GAP = 0.1f;

PauseMenu::PauseMenu() :
    game_paused(false),
    pause_button_hovered(false),
    resume_button_hovered(false),
    restart_button_hovered(false),
    next_level_hovered(false),
    resume_clicked(false),
    restart_clicked(false),
    next_level_clicked(false) {
    init();
}

void PauseMenu::init() {
    // Pause button position (top right)
    pause_button_x = 15.5f;
    pause_button_y = 0.5f;
    pause_button_width = PAUSE_BUTTON_SIZE;
    pause_button_height = PAUSE_BUTTON_SIZE;

    // Pause menu position (center)
    pause_menu_x = 8.0f;
    pause_menu_y = 4.0f;
    pause_menu_width = PAUSE_MENU_WIDTH;
    pause_menu_height = PAUSE_MENU_HEIGHT;

    // Initialize button positions
    updateButtonPositions(false);
}

void PauseMenu::updateButtonPositions(bool has_next_level) {
    // Calculate total height of all buttons
    int button_count = 2; // Resume and Restart are always shown
    if (has_next_level) button_count++;

    float total_height = button_count * BUTTON_HEIGHT + (button_count - 1) * BUTTON_GAP;
    float start_y = pause_menu_y - total_height / 2 + BUTTON_HEIGHT / 2;

    // Resume button (top button)
    resume_button_x = pause_menu_x;
    resume_button_y = start_y;

    // Next Level button (middle if exists)
    if (has_next_level) {
        next_level_button_x = pause_menu_x;
        next_level_button_y = start_y + BUTTON_HEIGHT + BUTTON_GAP;
        start_y += BUTTON_HEIGHT + BUTTON_GAP;
    }

    // Restart button (bottom button)
    restart_button_x = pause_menu_x;
    restart_button_y = start_y + BUTTON_HEIGHT + BUTTON_GAP;

    button_width = BUTTON_WIDTH;
    button_height = BUTTON_HEIGHT;
}

void PauseMenu::update(float canvas_x, float canvas_y, bool mouse_pressed) {
    // Reset click flags
    resume_clicked = false;
    restart_clicked = false;
    next_level_clicked = false;

    // Check if pause button is hovered
    pause_button_hovered = isMouseOverPauseButton(canvas_x, canvas_y);

    if (pause_button_hovered && mouse_pressed) {
        pauseGame();
        return;
    }

    // If game is paused, check menu buttons
    if (game_paused) {
        // Check resume button hover
        resume_button_hovered = false;
        if (canvas_x >= resume_button_x - button_width / 2 &&
            canvas_x <= resume_button_x + button_width / 2 &&
            canvas_y >= resume_button_y - button_height / 2 &&
            canvas_y <= resume_button_y + button_height / 2) {
            resume_button_hovered = true;

            if (mouse_pressed) {
                resume_clicked = true;
                return;
            }
        }

        // Check restart button hover
        restart_button_hovered = false;
        if (canvas_x >= restart_button_x - button_width / 2 &&
            canvas_x <= restart_button_x + button_width / 2 &&
            canvas_y >= restart_button_y - button_height / 2 &&
            canvas_y <= restart_button_y + button_height / 2) {
            restart_button_hovered = true;

            if (mouse_pressed) {
                restart_clicked = true;
                return;
            }
        }

        // Check next level button hover
        next_level_hovered = false;
        if (canvas_x >= next_level_button_x - button_width / 2 &&
            canvas_x <= next_level_button_x + button_width / 2 &&
            canvas_y >= next_level_button_y - button_height / 2 &&
            canvas_y <= next_level_button_y + button_height / 2) {
            next_level_hovered = true;

            if (mouse_pressed) {
                next_level_clicked = true;
                return;
            }
        }
    }
}

void PauseMenu::draw() const {
    graphics::Brush br;

    // 1. Draw pause button (always visible)
    if (pause_button_hovered) {
        br.fill_color[0] = 0.8f;
        br.fill_color[1] = 0.8f;
        br.fill_color[2] = 0.8f;
    }
    else {
        br.fill_color[0] = 1.0f;
        br.fill_color[1] = 1.0f;
        br.fill_color[2] = 1.0f;
    }

    // Draw pause icon (two vertical bars)
    float bar_width = 0.08f;
    float bar_height = 0.3f;
    float gap = 0.05f;

    // Left bar
    graphics::drawRect(pause_button_x - gap / 2 - bar_width / 2, pause_button_y,
        bar_width, bar_height, br);
    // Right bar
    graphics::drawRect(pause_button_x + gap / 2 + bar_width / 2, pause_button_y,
        bar_width, bar_height, br);

    // Draw button background (optional, for better visibility)
    br.fill_color[0] = 0.2f;
    br.fill_color[1] = 0.2f;
    br.fill_color[2] = 0.2f;
    br.fill_opacity = 0.7f;
    graphics::drawRect(pause_button_x, pause_button_y,
        pause_button_width, pause_button_height, br);

    // Pulsing effect when game is running
    if (!game_paused) {
        static float pulse = 0.0f;
        pulse += 0.05f;

        br.fill_color[0] = 1.0f;
        br.fill_color[1] = 1.0f;
        br.fill_color[2] = 1.0f;
        br.fill_opacity = 0.3f + 0.2f * sinf(pulse);
        graphics::drawDisk(pause_button_x, pause_button_y,
            pause_button_width / 2, br);
    }

    // 2. If game is paused, draw the pause menu overlay
    if (game_paused) {
        // Draw semi-transparent overlay
        br.fill_color[0] = 0.0f;
        br.fill_color[1] = 0.0f;
        br.fill_color[2] = 0.0f;
        br.fill_opacity = 0.5f;
        graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, br);

        // Draw pause menu background
        br.fill_color[0] = 0.1f;
        br.fill_color[1] = 0.1f;
        br.fill_color[2] = 0.2f;
        br.fill_opacity = 0.9f;
        br.outline_color[0] = 1.0f;
        br.outline_color[1] = 1.0f;
        br.outline_color[2] = 1.0f;
        br.outline_opacity = 1.0f;
        br.outline_width = 2.0f;

        graphics::drawRect(pause_menu_x, pause_menu_y,
            pause_menu_width, pause_menu_height, br);

        // Draw "PAUSED" text
        graphics::Brush text_br;
        text_br.fill_color[0] = 1.0f;
        text_br.fill_color[1] = 1.0f;
        text_br.fill_color[2] = 1.0f;
        graphics::drawText(pause_menu_x - 0.5f, pause_menu_y - 1.2f,
            0.3f, "PAUSED", text_br);

        // Draw resume button
        if (resume_button_hovered) {
            br.fill_color[0] = 0.3f;
            br.fill_color[1] = 0.3f;
            br.fill_color[2] = 0.8f;
        }
        else {
            br.fill_color[0] = 0.2f;
            br.fill_color[1] = 0.2f;
            br.fill_color[2] = 0.6f;
        }
        br.fill_opacity = 1.0f;
        br.outline_opacity = 1.0f;

        graphics::drawRect(resume_button_x, resume_button_y,
            button_width, button_height, br);

        text_br.fill_color[0] = 1.0f;
        text_br.fill_color[1] = 1.0f;
        text_br.fill_color[2] = 1.0f;
        graphics::drawText(resume_button_x - 0.3f, resume_button_y + 0.05f,
            0.2f, "RESUME", text_br);

        // Draw next level button (if applicable)
        // TODO: Implement logic to check if next level exists
        // For now, always show it
        if (true) { // Replace with actual condition
            if (next_level_hovered) {
                br.fill_color[0] = 0.3f;
                br.fill_color[1] = 0.8f;
                br.fill_color[2] = 0.3f;
            }
            else {
                br.fill_color[0] = 0.2f;
                br.fill_color[1] = 0.6f;
                br.fill_color[2] = 0.2f;
            }

            graphics::drawRect(next_level_button_x, next_level_button_y,
                button_width, button_height, br);

            text_br.fill_color[0] = 1.0f;
            text_br.fill_color[1] = 1.0f;
            text_br.fill_color[2] = 1.0f;
            graphics::drawText(next_level_button_x - 0.45f, next_level_button_y + 0.05f,
                0.2f, "NEXT LEVEL", text_br);
        }

        // Draw restart button
        if (restart_button_hovered) {
            br.fill_color[0] = 0.8f;
            br.fill_color[1] = 0.3f;
            br.fill_color[2] = 0.3f;
        }
        else {
            br.fill_color[0] = 0.6f;
            br.fill_color[1] = 0.2f;
            br.fill_color[2] = 0.2f;
        }

        graphics::drawRect(restart_button_x, restart_button_y,
            button_width, button_height, br);

        text_br.fill_color[0] = 1.0f;
        text_br.fill_color[1] = 1.0f;
        text_br.fill_color[2] = 1.0f;
        graphics::drawText(restart_button_x - 0.35f, restart_button_y + 0.05f,
            0.2f, "RESTART", text_br);
    }
}

void PauseMenu::pauseGame() {
    game_paused = true;
}

void PauseMenu::resumeGame() {
    game_paused = false;
}

void PauseMenu::reset() {
    game_paused = false;
    pause_button_hovered = false;
    resume_button_hovered = false;
    restart_button_hovered = false;
    next_level_hovered = false;
    resume_clicked = false;
    restart_clicked = false;
    next_level_clicked = false;
}

bool PauseMenu::isMouseOverPauseButton(float mx, float my) const {
    return (mx >= pause_button_x - pause_button_width / 2 &&
        mx <= pause_button_x + pause_button_width / 2 &&
        my >= pause_button_y - pause_button_height / 2 &&
        my <= pause_button_y + pause_button_height / 2);
}

bool PauseMenu::isMouseOverNextLevelButton(float mx, float my) const {
    return (mx >= next_level_button_x - button_width / 2 &&
        mx <= next_level_button_x + button_width / 2 &&
        my >= next_level_button_y - button_height / 2 &&
        my <= next_level_button_y + button_height / 2);
}