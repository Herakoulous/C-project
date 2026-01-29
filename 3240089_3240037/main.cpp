#include "global_state.h"
#include "sgg/graphics.h"

// Wrapper functions για τα callbacks της sgg
void draw() {
    GlobalState::getInstance()->draw();
}

void update(float dt) {
    GlobalState::getInstance()->update(dt);
}

int main() {
    // Αρχικοποίηση του παιχνιδιού μέσω του GlobalState (Singleton pattern)
    GlobalState::getInstance()->init();

    // Ρύθμιση παραθύρου και κανβά
    graphics::createWindow(1600, 800, "Jelly Go Game");
    graphics::setFullScreen(true);
    graphics::setFont("assets/arial.ttf");
    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);
    graphics::setCanvasSize(16, 8);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);
    graphics::startMessageLoop();

    return 0;
}