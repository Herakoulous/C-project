#pragma once
#include "entity.h"
#include <vector>

class HoverMenu {
private:
    float x, y;
    bool visible = false;
    Entity* target = nullptr;
    bool is_baby;
    int cost;

    struct Button {
        float center_x, center_y;
        float width, height;
        int option_id;
    };
    std::vector<Button> buttons;

public:
    void show(float menu_x, float menu_y, Entity* entity, bool baby, int cost);
    int checkClick(float mx, float my);
    void hide();
    void draw();

    int getUpgradeCost() const { return cost; }
    bool isVisible() const { return visible; }
    Entity* getTarget() const { return target; }
    bool getIsBaby() const { return is_baby; }
};
