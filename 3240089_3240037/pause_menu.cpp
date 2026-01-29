#include "pause_menu.h"
#include "sgg/graphics.h"
#include <cmath>

PauseMenu::PauseMenu() :
    game_paused(false),
    pause_button_hovered(false),
    resume_button_hovered(false),
    restart_button_hovered(false),
    next_level_hovered(false),
    exit_button_hovered(false),
    resume_clicked(false),
    restart_clicked(false),
    next_level_clicked(false),
    exit_clicked(false),
    next_level_available(true),
    exit_available(false) {

	// Σταθερές θέσεις και μεγέθη
    pause_button_x = 15.5f;
    pause_button_y = 0.5f;
    pause_button_width = 0.5f;
    pause_button_height = 0.5f;

    resume_button_x = 8.0f;
    resume_button_y = 3.0f;

    next_level_button_x = 8.0f;
    next_level_button_y = 4.0f;

    exit_button_x = 8.0f;
    exit_button_y = 4.0f;

    restart_button_x = 8.0f;
    restart_button_y = 5.0f;

    button_width = 2.0f;
    button_height = 0.5f;
}

void PauseMenu::setExitAvailable(bool available) {
    exit_available = available;
}

void PauseMenu::updateButtonPositions(bool has_next_level) {
    next_level_available = has_next_level;
}

void PauseMenu::draw() const {
    graphics::Brush br;

    // 1. Pause button (πάντα ορατό)
    if (pause_button_hovered) {
        br.fill_color[0] = 0.8f; br.fill_color[1] = 0.8f; br.fill_color[2] = 0.8f;
    }
    else {
        br.fill_color[0] = 1.0f; br.fill_color[1] = 1.0f; br.fill_color[2] = 1.0f;
    }

    // Σταθερό pause icon
    float bar_width = 0.08f;
    float bar_height = 0.3f;
    graphics::drawRect(15.45f, 0.5f, bar_width, bar_height, br);
    graphics::drawRect(15.55f, 0.5f, bar_width, bar_height, br);

    // Background για το pause button
    br.fill_color[0] = 0.2f; br.fill_color[1] = 0.2f; br.fill_color[2] = 0.2f;
    br.fill_opacity = 0.7f;
    graphics::drawRect(pause_button_x, pause_button_y, pause_button_width, pause_button_height, br);

    // 2. Αν είναι paused, σχεδίασε μενού
    if (game_paused) {
        // Μαύρο overlay
        br.fill_color[0] = 0.0f; br.fill_color[1] = 0.0f; br.fill_color[2] = 0.0f;
        br.fill_opacity = 0.5f;
        graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, br);

        // Menu background
        br.fill_color[0] = 0.1f; br.fill_color[1] = 0.1f; br.fill_color[2] = 0.2f;
        br.fill_opacity = 1.0f;
        br.outline_color[0] = 1.0f; br.outline_color[1] = 1.0f; br.outline_color[2] = 1.0f;
        br.outline_opacity = 1.0f;
        br.outline_width = 2.0f;
        graphics::drawRect(8.0f, 4.0f, 3.0f, 4.0f, br);

        // "PAUSED" text
        graphics::Brush text_br;
        text_br.fill_color[0] = 1.0f; text_br.fill_color[1] = 1.0f; text_br.fill_color[2] = 1.0f;
        graphics::drawText(7.5f, 2.4f, 0.3f, "PAUSED", text_br);

        // RESUME button
        if (resume_button_hovered) {
            br.fill_color[0] = 0.3f; br.fill_color[1] = 0.3f; br.fill_color[2] = 0.8f;
        }
        else {
            br.fill_color[0] = 0.2f; br.fill_color[1] = 0.2f; br.fill_color[2] = 0.6f;
        }
        br.fill_opacity = 1.0f;
        br.outline_opacity = 1.0f;
        graphics::drawRect(resume_button_x, resume_button_y, button_width, button_height, br);
        graphics::drawText(resume_button_x - 0.3f, resume_button_y + 0.05f, 0.2f, "RESUME", text_br);

        // NEXT LEVEL button (μόνο αν υπάρχει)
        if (next_level_available) {
            if (next_level_hovered) {
                br.fill_color[0] = 0.3f; br.fill_color[1] = 0.8f; br.fill_color[2] = 0.3f;
            }
            else {
                br.fill_color[0] = 0.2f; br.fill_color[1] = 0.6f; br.fill_color[2] = 0.2f;
            }
            graphics::drawRect(next_level_button_x, next_level_button_y, button_width, button_height, br);
            graphics::drawText(next_level_button_x - 0.45f, next_level_button_y + 0.05f, 0.2f, "NEXT LEVEL", text_br);
        }

        // EXIT button (μόνο στο level 3)
        if (exit_available) {
            if (exit_button_hovered) {
                br.fill_color[0] = 0.8f; br.fill_color[1] = 0.3f; br.fill_color[2] = 0.3f;
            }
            else {
                br.fill_color[0] = 0.6f; br.fill_color[1] = 0.2f; br.fill_color[2] = 0.2f;
            }
            graphics::drawRect(exit_button_x, exit_button_y, button_width, button_height, br);
            graphics::drawText(exit_button_x - 0.25f, exit_button_y + 0.05f, 0.2f, "EXIT", text_br);
        }

        // RESTART button
        if (restart_button_hovered) {
            br.fill_color[0] = 0.8f; br.fill_color[1] = 0.3f; br.fill_color[2] = 0.3f;
        }
        else {
            br.fill_color[0] = 0.6f; br.fill_color[1] = 0.2f; br.fill_color[2] = 0.2f;
        }
        graphics::drawRect(restart_button_x, restart_button_y, button_width, button_height, br);
        graphics::drawText(restart_button_x - 0.35f, restart_button_y + 0.05f, 0.2f, "RESTART", text_br);
    }
}


// Ενημέρωση κατάστασης κάθε frame (κλικ, hover)
void PauseMenu::update(float canvas_x, float canvas_y, bool mouse_pressed) {
    // Reset click flags
    resume_clicked = false;
    restart_clicked = false;
    next_level_clicked = false;
    exit_clicked = false;

	// Έλεγχος hover στο pause button
    pause_button_hovered = isMouseOverPauseButton(canvas_x, canvas_y);

    if (pause_button_hovered && mouse_pressed) {
        pauseGame();
        return;
    }

	//Αν είναι paused, έλεγχος hover και κλικ στα μενού buttons
    if (game_paused) {
        // Έλεγχος resume button hover
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

        // Έλεγχος restart button hover
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

        // Έλεγχος next level button hover
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
        
        exit_button_hovered = false;
        if (exit_available) {
            if (canvas_x >= exit_button_x - button_width / 2 &&
                canvas_x <= exit_button_x + button_width / 2 &&
                canvas_y >= exit_button_y - button_height / 2 &&
                canvas_y <= exit_button_y + button_height / 2) {
                exit_button_hovered = true;

                if (mouse_pressed) {
                    exit_clicked = true;
                    return;
                }
            }
        }
    }
}

// Παύση παιχνιδιού
void PauseMenu::pauseGame() {
    game_paused = true;
}

// Συνέχιση παιχνιδιού
void PauseMenu::resumeGame() {
    game_paused = false;
}

// Επαναφορά κατάστασης (όταν ξεκινάει νέο level)
void PauseMenu::reset() {
    game_paused = false;
    pause_button_hovered = false;
    resume_button_hovered = false;
    restart_button_hovered = false;
    next_level_hovered = false;
    exit_button_hovered = false;
    resume_clicked = false;
    restart_clicked = false;
    next_level_clicked = false;
    exit_clicked = false;
}

// Έλεγχος αν ο δείκτης είναι πάνω στο pause button
bool PauseMenu::isMouseOverPauseButton(float mx, float my) const {
    return (mx >= pause_button_x - pause_button_width / 2 &&
        mx <= pause_button_x + pause_button_width / 2 &&
        my >= pause_button_y - pause_button_height / 2 &&
        my <= pause_button_y + pause_button_height / 2);
}

// Έλεγχος αν ο δείκτης είναι πάνω στο next level button
bool PauseMenu::isMouseOverNextLevelButton(float mx, float my) const {
    return (mx >= next_level_button_x - button_width / 2 &&
        mx <= next_level_button_x + button_width / 2 &&
        my >= next_level_button_y - button_height / 2 &&
        my <= next_level_button_y + button_height / 2);
}

// Ορισμός διαθεσιμότητας επόμενου level
void PauseMenu::setNextLevelAvailable(bool available) {
    next_level_available = available;
}
