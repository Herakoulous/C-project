#include "hover_menu.h"

void HoverMenu::show(float menu_x, float menu_y, Entity* entity, bool baby , int upgrade_cost) {
    x = menu_x;
    y = menu_y;
    visible = true;
    target = entity;
    is_baby = baby;
    cost = upgrade_cost;
    buttons.clear();

    if (is_baby) {
        buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 0 });
        buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 1 });
        buttons.push_back({ x + 0.5f, y - 1.3f, 0.4f, 0.4f, 2 });
    }
    else {
        buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 });
    }
}


int HoverMenu::checkClick(float mx, float my) {
    if (!visible) return -1;
    for (size_t i = 0; i < buttons.size(); i++) {
        const Button& btn = buttons[i];
        if (mx >= btn.center_x - btn.width / 2 && mx <= btn.center_x + btn.width / 2 &&
            my >= btn.center_y - btn.height / 2 && my <= btn.center_y + btn.height / 2) {
            return btn.option_id;
        }
    }
    return -1;
}

void HoverMenu::hide() {
    visible = false;
    target = nullptr;
}

void HoverMenu::draw() {
    if (!visible || !target) return;
    graphics::Brush br_back, br_char, br_text;

    br_back.fill_color[0] = 0.1f; br_back.fill_color[1] = 0.1f; br_back.fill_color[2] = 0.2f;
    br_char.fill_color[0] = 0.1f; br_char.fill_color[1] = 0.1f; br_char.fill_color[2] = 1.0f;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;

    if (is_baby) {
        // Circle (5)
        graphics::drawRect(x - 0.5f, y - 1.3f, 0.4f, 0.4f, br_back);
        graphics::drawDisk(x - 0.5f, y - 1.3f, 0.15f, br_char);
        graphics::drawText(x - 0.52f, y - 1.20f, 0.15f, "5", br_text);

        // Square (10)
        graphics::drawRect(x, y - 1.5f, 0.4f, 0.4f, br_back);
        graphics::drawRect(x, y - 1.5f, 0.25f, 0.25f, br_char);
        graphics::drawText(x - 0.06f, y - 1.4f, 0.15f, "10", br_text);

        // Triangle (10)
        graphics::drawRect(x + 0.5f, y - 1.3f, 0.4f, 0.4f, br_back);
        graphics::drawSector(x + 0.5f, y - 1.45f, 0, 0.3f, 240, 300, br_char);
        br_back.outline_color[0] = 0.0f; br_back.outline_color[1] = 0.0f; br_back.outline_color[2] = 0.0f;
        graphics::drawRect(x + 0.5f, y - 1.15f, 0.3f, 0.08f, br_back);
        graphics::drawLine(x + 0.35f, y - 1.2f, x + 0.65f, y - 1.2f, br_text);
        graphics::drawText(x + 0.44f, y - 1.20f, 0.15f, "10", br_text);
    }
    else {
        graphics::drawRect(x, y - 1.5f, 0.4f, 0.4f, br_back);
        graphics::drawSector(x , y - 1.65f, 0, 0.25f, 240, 300, br_char);
        br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
        graphics::drawRect(x, y - 1.4f, 0.3f, 0.08f, br_back);
        graphics::drawLine(x - 0.12f, y - 1.45f, x + 0.13f, y - 1.45f, br_text);
        graphics::drawText(x - 0.17f, y - 1.32f, 0.12f, std::to_string(cost) , br_text);
        graphics::drawRect(x, y - 1.41f, 0.08f, 0.08f, br_char);
    }
}