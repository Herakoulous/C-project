#include "global_state.h"
#include "sgg/graphics.h"
#include <algorithm>
#include <cmath>

GlobalState* GlobalState::instance = nullptr;

GlobalState::GlobalState() : selected_entity(nullptr) {
}

GlobalState* GlobalState::getInstance() {
    if (!instance) {
        instance = new GlobalState();
    }
    return instance;
}

GlobalState::~GlobalState() {
    entities.clear();
    troops.clear();
}

void GlobalState::init() {
    entities.clear();
    troops.clear();
    entity_graph.clear();
    selected_entity = nullptr;

    entities.push_back(std::make_unique<Warrior>(3.5f, 3.0f, 100, Side::PLAYER));
    entities.push_back(std::make_unique<Wizard>(1.5f, 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(3.5f, 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(1.5f, 1.5f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Tower>(8.0f, 4.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(16 - 5.5f, 1.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Tower>(5.5f, 8 - 1.4f, 10, Side::NEUTRAL));

    entities.push_back(std::make_unique<Wizard>(16 - 3.5f, 8 - 3.0f, 10, Side::ENEMY));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 3.5f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 3.5f, 8 - 1.0f, 10, Side::NEUTRAL));
    entities.push_back(std::make_unique<Baby>(16 - 1.5f, 8 - 1.5f, 10, Side::NEUTRAL));

    for (size_t i = 0; i < entities.size(); i++) {
        for (size_t j = i + 1; j < entities.size(); j++) {
            entity_graph.addEdge(entities[i].get(), entities[j].get());
        }
    }

    entity_graph.calculatePaths(entities);
}

void GlobalState::handleTroopArrivals() {
    for (auto it = troops.begin(); it != troops.end(); ) {
        if ((*it)->hasArrived()) {
            Entity* target = (*it)->getTarget();
            Entity* source = (*it)->getSource();
            int amount = (*it)->getHealthAmount();

            if (target->getSide() == source->getSide()) {
                target->setHealth(target->getHealth() + amount);
            }
            else {
                int damage = std::max(1, amount - (int)target->getDefense());
                target->setHealth(target->getHealth() - damage);
                if (target->getHealth() < 0) {
                    target->setSide(source->getSide());
                    target->setHealth(std::abs(target->getHealth()));
                }
            }

            it = troops.erase(it);
        }
        else {
            ++it;
        }
    }
}

void GlobalState::handleHoverMenu(float canvas_x, float canvas_y) {
    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            if (entity->getSide() == Side::PLAYER) {
                if (entity->getType() == "Baby") {
                    hover_menu.show(entity->getX(), entity->getY() + 1.0f,
                        entity.get(), true, 0);
                }
                else {
                    hover_menu.show(entity->getX(), entity->getY() + 1.3f,
                        entity.get(), false, entity->getUpgradeCost());
                }
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
}
void GlobalState::handleMenuClick(float canvas_x, float canvas_y, int clicked_option) {
    Entity* target = hover_menu.getTarget();
    bool is_baby = hover_menu.getIsBaby();

    if (is_baby) {
        if (clicked_option == 0 && target->getHealth() >= 5) {
            int hp = target->getHealth() - 5;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    entities[i] = std::make_unique<Warrior>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    for (size_t j = 0; j < entities.size(); j++) {
                        if (j != i && entities[j].get() != new_entity) {
                            entity_graph.addEdge(new_entity, entities[j].get());
                        }
                    }

                    hover_menu.hide();
                    break;
                }
            }
        }
        else if (clicked_option == 1 && target->getHealth() >= 10) {
            int hp = target->getHealth() - 10;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    entities[i] = std::make_unique<Tower>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    for (size_t j = 0; j < entities.size(); j++) {
                        if (j != i && entities[j].get() != new_entity) {
                            entity_graph.addEdge(new_entity, entities[j].get());
                        }
                    }

                    hover_menu.hide();
                    break;
                }
            }
        }
        else if (clicked_option == 2 && target->getHealth() >= 10) {
            int hp = target->getHealth() - 10;
            float px = target->getX(), py = target->getY();
            Side side = target->getSide();

            for (size_t i = 0; i < entities.size(); i++) {
                if (entities[i].get() == target) {
                    std::vector<Entity*> old_connections;
                    for (auto& conn : entity_graph.getConnections(target)) {
                        old_connections.push_back(conn);
                    }

                    entities[i] = std::make_unique<Wizard>(px, py, hp, side);
                    Entity* new_entity = entities[i].get();

                    for (Entity* connected_entity : old_connections) {
                        entity_graph.addEdge(new_entity, connected_entity);
                    }

                    for (size_t j = 0; j < entities.size(); j++) {
                        if (j != i && entities[j].get() != new_entity) {
                            entity_graph.addEdge(new_entity, entities[j].get());
                        }
                    }

                    hover_menu.hide();
                    break;
                }
            }
        }
    }
    else {
        if (target->canUpgrade()) {
            target->performUpgrade();
            hover_menu.hide();
        }
        else {
            hover_menu.hide();
        }
    }

    entity_graph.calculatePaths(entities);
}


void GlobalState::handleEntityClick(float canvas_x, float canvas_y) {
    bool clicked_entity = false;

    for (auto& entity : entities) {
        if (entity->contains(canvas_x, canvas_y)) {
            clicked_entity = true;

            if (selected_entity && selected_entity != entity.get()) {
                if (selected_entity->getSide() == Side::PLAYER) {
                    int amount = selected_entity->getHealth() / 2;
                    if (amount > 0) {
                        const PathData* path = entity_graph.getPath(selected_entity, entity.get());
                        if (path && !path->waypoints.empty()) {
                            troops.push_back(std::make_unique<Troop>(
                                selected_entity, entity.get(), amount, path->waypoints
                            ));
                            selected_entity->setHealth(selected_entity->getHealth() - amount);
                        }
                    }
                }
                selected_entity = nullptr;
            }
            else {
                if (entity->getSide() == Side::PLAYER) {
                    selected_entity = entity.get();
                }
                else {
                    selected_entity = nullptr;
                }
            }
            break;
        }
    }

    if (!clicked_entity) {
        selected_entity = nullptr;
    }
}

void GlobalState::handleMouseInput(float canvas_x, float canvas_y, bool mouse_pressed) {
    if (mouse_pressed) {
        int clicked_option = hover_menu.checkClick(canvas_x, canvas_y);
        if (clicked_option != -1) {
            handleMenuClick(canvas_x, canvas_y, clicked_option);
        }
        else {
            handleEntityClick(canvas_x, canvas_y);
        }
    }
}

void GlobalState::update(float dt) {
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

    handleHoverMenu(canvas_x, canvas_y);
    handleMouseInput(canvas_x, canvas_y, mouse.button_left_pressed);
}

void GlobalState::draw() {
    graphics::Brush bg;
    bg.fill_color[0] = 0.0f; bg.fill_color[1] = 1.0f; bg.fill_color[2] = 0.0f;
    graphics::drawRect(8.0f, 4.0f, 16.0f, 8.0f, bg);

    entity_graph.draw();

    for (auto& entity : entities) {
        entity->draw();
    }

    for (auto& troop : troops) {
        troop->draw();
    }

    if (selected_entity) {
        graphics::Brush hl;
        hl.fill_color[0] = hl.fill_color[1] = 1.0f; hl.fill_color[2] = 0.0f;
        hl.fill_opacity = 0.0f;
        hl.outline_opacity = 1.0f;
        hl.outline_width = 3.0f;
        graphics::drawDisk(selected_entity->getX(), selected_entity->getY(),
            selected_entity->getSize() + 0.1f, hl);
    }

    hover_menu.draw();
}