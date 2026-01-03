// COPY THIS - Replace your entire main.cpp
#include "entity.h"
#include "hover_menu.h"
#include "troop.h"
#include "graph.h"
#include <vector>
#include <memory>

HoverMenu hover_menu;
std::vector<std::unique_ptr<Entity>> entities;
std::vector<std::unique_ptr<Troop>> troops;
EntityGraph entity_graph;
Entity* selected_entity = nullptr;

void initializeGame() {
    entities.clear();
    troops.clear();
    entity_graph.clear();
    selected_entity = nullptr;

    entities.push_back(std::make_unique<Wizard>(3.5f, 3.0f, 10, Side::PLAYER));
    entities.push_back(std::make_unique<Baby>(1.5f, 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(3.5f, 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(1.5f, 1.5f, 10, Side::NEUTRAL));


    entities.push_back(std::make_unique<Tower>(8.0f, 4.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 5.5f, 1.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(5.5f, 8 - 1.4f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Warrior>(16 - 3.5f, 8 - 3.0f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 3.5f, 8 - 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 1.5f, 10, Side::NEUTRAL));

    // Connect all entities in graph
    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            entity_graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    // Calculate all paths in the graph
    entity_graph.calculatePaths(entities);
}

void draw() {
    graphics::Brush bg;
    bg.fill_color[0] = 0.0f; bg.fill_color[1] = 1.0f; bg.fill_color[2] = 0.0f;
    graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, bg);

    entity_graph.draw();

    for (auto& entity : entities) {
        entity->draw();
    }

    if (selected_entity) {
        graphics::Brush hl;
        hl.fill_color[0] = hl.fill_color[1] = 1.0f; hl.fill_color[2] = 0.0f;
        hl.fill_opacity = 0.0f;
        hl.outline_opacity = 1.0f;
        hl.outline_width = 3.0f;
        graphics::drawDisk(selected_entity->x, selected_entity->y, selected_entity->size + 0.1f, hl);
    }

    for (auto& troop : troops) {
        troop->draw();
    }

    hover_menu.draw();
}

void handleTroopArrivals() {
    for (auto& troop : troops) {
        if (troop->hasArrived()) {
            Entity* target = troop->getTarget();
            Entity* source = troop->getSource();
            int amount = troop->getHealthAmount();

            if (target->side == source->side) {
                target->health += amount;
            }
            else {
                int damage = std::max(1, amount - (int)target->defense);
                target->health -= damage;
                if (target->health < 0) {
                    target->side = source->side;
                    target->health = std::abs(target->health);
                }
            }
        }
    }

    troops.erase(
        std::remove_if(troops.begin(), troops.end(),
            [](const std::unique_ptr<Troop>& t) { return t->hasArrived(); }),
        troops.end()
    );
}

void update(float dt) {
    for (auto& entity : entities) {
        entity->update(dt);
    }

    for (auto& troop : troops) {
        troop->update(dt);
    }

    handleTroopArrivals();

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
                hover_menu.show(entity->x, entity->y + 1.3f, entity.get(), false, wizard->getUpgradeCost());
            }
            else if (tower && tower->getLevel() != 5 && tower->getSide() == Side::PLAYER) {
                hover_menu.show(entity->x, entity->y + 1.0f, entity.get(), false, tower->getUpgradeCost());
            }
            else if (warrior && warrior->getLevel() != 5 && warrior->getSide() == Side::PLAYER) {
                hover_menu.show(entity->x, entity->y + 1.0f, entity.get(), false, warrior->getUpgradeCost());
            }
            else if (baby && baby->getSide() == Side::PLAYER) {
                hover_menu.show(entity->x, entity->y + 1.0f, entity.get(), true, 0);
            }
            break;
        }
    }

    if (hover_menu.isVisible()) {
        int hovered = hover_menu.checkClick(canvas_x, canvas_y);
        if (hovered == -1) {
            bool over_entity = false;
            for (auto& entity : entities) {
                if (entity->contains(canvas_x, canvas_y)) {
                    over_entity = true;
                    break;
                }
            }
            if (!over_entity) hover_menu.hide();
        }
    }

    if (mouse.button_left_pressed) {
        int clicked_option = hover_menu.checkClick(canvas_x, canvas_y);
        if (clicked_option != -1) {
            Entity* target = hover_menu.getTarget();
            bool is_baby = hover_menu.getIsBaby();

            if (is_baby) {
                if (clicked_option == 0 && target->health >= 5) {
                    int hp = target->health - 5;
                    float px = target->x, py = target->y;
                    Side side = target->side;
                    for (size_t i = 0; i < entities.size(); i++) {
                        if (entities[i].get() == target) {
                            entities[i] = std::make_unique<Warrior>(px, py, hp, side);
                            break;
                        }
                    }
                    hover_menu.hide();
                }
                else if (clicked_option == 1 && target->health >= 10) {
                    int hp = target->health - 10;
                    float px = target->x, py = target->y;
                    Side side = target->side;
                    for (size_t i = 0; i < entities.size(); i++) {
                        if (entities[i].get() == target) {
                            entities[i] = std::make_unique<Tower>(px, py, hp, side);
                            break;
                        }
                    }
                    hover_menu.hide();
                }
                else if (clicked_option == 2 && target->health >= 10) {
                    int hp = target->health - 10;
                    float px = target->x, py = target->y;
                    Side side = target->side;
                    for (size_t i = 0; i < entities.size(); i++) {
                        if (entities[i].get() == target) {
                            entities[i] = std::make_unique<Wizard>(px, py, hp, side);
                            break;
                        }
                    }
                    hover_menu.hide();
                }
            }
            else {
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
        else {
            bool clicked_entity = false;
            for (auto& entity : entities) {
                if (entity->contains(canvas_x, canvas_y)) {
                    if (entity->getSide() == Side::PLAYER) {
                        clicked_entity = true;
                    }
                    if (selected_entity && selected_entity != entity.get()) {
                        int amount = selected_entity->health / 2;
                        if (amount > 0) {
                            // Get the pre-calculated path from the graph
                            const PathData* path = entity_graph.getPath(selected_entity, entity.get());
                            if (path && !path->waypoints.empty()) {
                                troops.push_back(std::make_unique<Troop>(
                                    selected_entity, entity.get(), amount, path->waypoints
                                ));
                                selected_entity->health -= amount;
                            }
                        }
                        selected_entity = nullptr;
                    }
                    else {
                        selected_entity = entity.get();
                    }
                    break;
                }
            }

            if (!clicked_entity) {
                selected_entity = nullptr;
            }
        }
    }
}

int main() {
    initializeGame();

    graphics::createWindow(1600, 800, "Inheritance Demo");
    graphics::setFont("assets/arial.ttf");
    graphics::setDrawFunction(draw);
    graphics::setUpdateFunction(update);
    graphics::setCanvasSize(16, 8);
    graphics::setCanvasScaleMode(graphics::CANVAS_SCALE_FIT);
    graphics::startMessageLoop();

    return 0;
}