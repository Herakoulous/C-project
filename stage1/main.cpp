#include "entity.h"
#include "hover_menu.h"
#include <vector>
#include <memory>

HoverMenu hover_menu;
std::vector<std::unique_ptr<Entity>> entities;

void initializeGame() {
    entities.clear();
    entities.push_back(std::make_unique<Warrior>(3.5f, 3.0f, 10, Side::PLAYER));
    entities.push_back(std::make_unique<Baby>(1.5f, 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(3.5f, 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(1.5f, 1.5f, 10, Side::NEUTRAL));


    entities.push_back(std::make_unique<Tower>(8.0f , 4.0f , 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 5.5f, 1.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(5.5f, 8 - 1.5f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Warrior>(16 - 3.5f,8 - 3.0f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>( 16 - 1.5f,8 - 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>( 16 - 3.5f,8 - 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f,8 - 1.5f, 10, Side::NEUTRAL));

}

void draw() {
    graphics::Brush bg;
    bg.fill_color[0] = 0.0f; bg.fill_color[1] = 1.0f; bg.fill_color[2] = 0.0f;
    graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, bg);

    for (auto& entity : entities) {
        entity->draw();
    }
    hover_menu.draw();
}

void update(float dt) {
    // Update all entities
    for (auto& entity : entities) {
        entity->update(dt);
    }

    // Get mouse state
    graphics::MouseState mouse;
    graphics::getMouseState(mouse);
    float canvas_x = graphics::windowToCanvasX((float)mouse.cur_pos_x);
    float canvas_y = graphics::windowToCanvasY((float)mouse.cur_pos_y);

    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            Wizard* wizard = dynamic_cast<Wizard*>(entity.get());
            Tower* tower = dynamic_cast<Tower*>(entity.get());
            Warrior* warrior = dynamic_cast<Warrior*>(entity.get());
            Baby* baby = dynamic_cast<Baby*>(entity.get());


            if (wizard && wizard->getLevel() != 3 && wizard->getSide() == Side::PLAYER) {
                int cost = wizard->getUpgradeCost();
                hover_menu.show(entity->x, entity->y + 1.3f, entity.get(), false, cost);
            }
            else if (tower && tower->getLevel()!=5 && tower->getSide() == Side::PLAYER) {
                int cost = tower->getUpgradeCost();
                hover_menu.show(entity->x, entity->y + 1.0f, entity.get(), false, cost);
            }
            else if (warrior && warrior->getLevel() != 5 && warrior->getSide() == Side::PLAYER) {
                int cost = warrior->getUpgradeCost();
                hover_menu.show(entity->x, entity->y + 1.0f, entity.get(), false, cost);
            }
            else if (baby && baby->getSide() == Side::PLAYER) {
                hover_menu.show(entity->x, entity->y + 1.0f, entity.get(), true, 0);
            }
            break;
        }
    }

    // Keep menu visible if mouse is over the menu
    if (hover_menu.isVisible()) {
        int hovered_button = hover_menu.checkClick(canvas_x, canvas_y);
        if (hovered_button != -1) {
            // Mouse is over a button, keep menu visible
        }
        else {
            bool mouse_over_entity = false;
            for (auto& entity : entities) {
                if (entity->contains(canvas_x, canvas_y)) {
                    mouse_over_entity = true;
                    break;
                }
            }
            if (!mouse_over_entity) {
                hover_menu.hide();
            }
        }
    }

    // Handle clicks
    if (mouse.button_left_pressed) {
        int clicked_option = hover_menu.checkClick(canvas_x, canvas_y);
        if (clicked_option != -1) {
            Entity* target = hover_menu.getTarget();
            bool is_baby = hover_menu.getIsBaby();

            if (is_baby) {
                // Baby evolution options
                if (clicked_option == 0 && target->health >= 5) {
                    // Circle: Evolve to Warrior (cost 5)
                    int remaining_health = target->health - 5;
                    float pos_x = target->x;
                    float pos_y = target->y;
                    Side entity_side = target->side;

                    for (size_t i = 0; i < entities.size(); i++) {
                        if (entities[i].get() == target) {
                            entities[i] = std::make_unique<Warrior>(pos_x, pos_y, remaining_health, entity_side);
                            break;
                        }
                    }
                    hover_menu.hide();
                }
                else if (clicked_option == 1 && target->health >= 10) {
                    // Square: Evolve to Tower (cost 10)
                    int remaining_health = target->health - 10;
                    float pos_x = target->x;
                    float pos_y = target->y;
                    Side entity_side = target->side;

                    for (size_t i = 0; i < entities.size(); i++) {
                        if (entities[i].get() == target) {
                            entities[i] = std::make_unique<Tower>(pos_x, pos_y, remaining_health, entity_side);
                            break;
                        }
                    }
                    hover_menu.hide();
                }
                else if (clicked_option == 2 && target->health >= 10) {
                    // Triangle: Evolve to Wizard (cost 10)
                    int remaining_health = target->health - 10;
                    float pos_x = target->x;
                    float pos_y = target->y;
                    Side entity_side = target->side;

                    for (size_t i = 0; i < entities.size(); i++) {
                        if (entities[i].get() == target) {
                            entities[i] = std::make_unique<Wizard>(pos_x, pos_y, remaining_health, entity_side);
                            break;
                        }
                    }
                    hover_menu.hide();
                }
            }
            else {
                // Upgrade existing Warrior, Tower or wizard
                Warrior* warrior = dynamic_cast<Warrior*>(target);
                Tower* tower = dynamic_cast<Tower*>(target);
                Wizard* wizard = dynamic_cast<Wizard*>(target);

                if (warrior && warrior->canUpgrade()) {
                    warrior->performUpgrade();
                    hover_menu.hide();
                }
                else if (tower && tower->canUpgrade()) {
                    tower->performUpgrade();
                    hover_menu.hide();
                }
                else if (wizard && wizard->canUpgrade()) {
                    wizard->performUpgrade();
                    hover_menu.hide();
                }
                else {
                    hover_menu.hide();
                }
            }
        }
    }
}

int main() {
    initializeGame();

    graphics::createWindow(1600, 800, "Inheritance Demo");
    graphics::setFullScreen(true);
    graphics::setFont("assets/arial.ttf");
    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);
    graphics::setCanvasSize(16, 8);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);
    graphics::startMessageLoop();

    return 0;
}