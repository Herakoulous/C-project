#include "hover_menu.h"
#include "sgg/graphics.h"

void HoverMenu::show(float menu_x, float menu_y, Entity* entity, bool baby, int upgrade_cost) {
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
    else if (entity->getType() == "Wizard") {
        // Wizard spells: Freeze, Sickness, Convert
        Wizard* wizard = static_cast<Wizard*>(entity);
        int level = wizard->getLevel();

        // Level 1: Freeze button (left) + Upgrade button (center)
        if (level == 1) {
            buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 10 }); // ID 10 = Freeze
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 }); // ID 0 = Upgrade
        }
        // Level 2: Freeze (left), Upgrade (center), Sickness (right)
        else if (level == 2) {
            buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 10 }); // Freeze
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 0 }); // Upgrade
            buttons.push_back({ x + 0.5f, y - 1.3f, 0.4f, 0.4f, 11 }); // ID 11 = Sickness
        }
        // Level 3: Freeze (left), Convert (center), Sickness (right) - NO UPGRADE
        else if (level >= 3) {
            buttons.push_back({ x - 0.5f, y - 1.3f, 0.4f, 0.4f, 10 }); // Freeze
            buttons.push_back({ x, y - 1.5f, 0.4f, 0.4f, 12 }); // ID 12 = Convert
            buttons.push_back({ x + 0.5f, y - 1.3f, 0.4f, 0.4f, 11 }); // Sickness
        }
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

    float x = target->getX();
    float y = target->getY();
    graphics::Brush br_back, br_char, br_text;

    br_back.fill_color[0] = 0.1f; br_back.fill_color[1] = 0.1f; br_back.fill_color[2] = 0.2f;
    br_char.fill_color[0] = 0.1f; br_char.fill_color[1] = 0.1f; br_char.fill_color[2] = 1.0f;
    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;

    if (is_baby) {
        // Circle (5)
        graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);
        graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);
        graphics::drawText(x - 0.52f, y + 1.1f - 1.20f, 0.15f, "5", br_text);

        // Square (10)
        graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);
        graphics::drawRect(x, y + 1.1f - 1.5f, 0.25f, 0.25f, br_char);
        graphics::drawText(x - 0.06f, y + 1.1f - 1.4f, 0.15f, "10", br_text);

        // Triangle (10)
        graphics::drawRect(x + 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);
        graphics::drawSector(x + 0.5f, y + 1.1f - 1.45f, 0, 0.3f, 240, 300, br_char);
        br_back.outline_color[0] = 0.0f; br_back.outline_color[1] = 0.0f; br_back.outline_color[2] = 0.0f;
        graphics::drawRect(x + 0.5f, y + 1.1f - 1.15f, 0.3f, 0.08f, br_back);
        graphics::drawLine(x + 0.35f, y + 1.1f - 1.2f, x + 0.65f, y + 1.1f - 1.2f, br_text);
        graphics::drawText(x + 0.44f, y + 1.1f - 1.20f, 0.15f, "10", br_text);
    }
    else if (target->getType() == "Wizard") {
        Wizard* wizard = static_cast<Wizard*>(target);
        int level = wizard->getLevel();

        // Level 1: Freeze (left) + Upgrade (center)
        if (level == 1) {
            // Freeze button (left) - Blue with progress bar
            graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float freeze_cd = wizard->getSpellCooldown(0);
            if (freeze_cd > 0.0f) {
                // Cooldown in progress - draw gray disk
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                // Draw circular progress bar
                float cooldown_progress = 1.0f - (freeze_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawSector(x - 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                // Draw cooldown timer
                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.55f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)freeze_cd + 1), br_text);
            }
            else {
                // Ready to cast - normal blue disk
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                // Draw cost
                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(0)) {
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
            }

            // Upgrade button (center)
            graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);
            graphics::drawSector(x, y + 1.1f - 1.65f, 0, 0.25f, 240, 300, br_char);
            br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
            graphics::drawRect(x, y + 1.1f - 1.4f, 0.3f, 0.08f, br_back);
            graphics::drawLine(x - 0.12f, y + 1.1f - 1.45f, x + 0.13f, y + 1.1f - 1.45f, br_text);
            graphics::drawText(x - 0.17f, y + 1.1f - 1.32f, 0.12f, std::to_string(cost), br_text);
            graphics::drawRect(x, y + 1.1f - 1.41f, 0.08f, 0.08f, br_char);
        }
        // Level 2: Freeze (left), Upgrade (center), Sickness (right)
        else if (level == 2) {
            // Freeze button (left) - Blue with progress bar
            graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float freeze_cd = wizard->getSpellCooldown(0);
            if (freeze_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (freeze_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawSector(x - 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.55f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)freeze_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(0)) {
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
            }

            // Upgrade button (center)
            graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);
            graphics::drawSector(x, y + 1.1f - 1.65f, 0, 0.25f, 240, 300, br_char);
            br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
            graphics::drawRect(x, y + 1.1f - 1.4f, 0.3f, 0.08f, br_back);
            graphics::drawLine(x - 0.12f, y + 1.1f - 1.45f, x + 0.13f, y + 1.1f - 1.45f, br_text);
            graphics::drawText(x - 0.17f, y + 1.1f - 1.32f, 0.12f, std::to_string(cost), br_text);
            graphics::drawRect(x, y + 1.1f - 1.41f, 0.08f, 0.08f, br_char);

            // Sickness button (right) - Green with progress bar
            graphics::drawRect(x + 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float sickness_cd = wizard->getSpellCooldown(1);
            if (sickness_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (sickness_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawSector(x + 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x + 0.45f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)sickness_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(1)) {
                    graphics::drawText(x + 0.47f, y + 1.1f - 1.17f, 0.12f, "30", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x + 0.47f, y + 1.1f - 1.17f, 0.12f, "30", br_text);
                }
            }
        }
        // Level 3: Freeze (left), Convert (center), Sickness (right)
        else if (level >= 3) {
            // Freeze button (left) - Blue with progress bar
            graphics::drawRect(x - 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float freeze_cd = wizard->getSpellCooldown(0);
            if (freeze_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (freeze_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawSector(x - 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.55f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)freeze_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 1.0f;
                graphics::drawDisk(x - 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(0)) {
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.53f, y + 1.1f - 1.17f, 0.12f, "25", br_text);
                }
            }

            // Convert button (center) - Red with progress bar
            graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);

            float convert_cd = wizard->getSpellCooldown(2);
            if (convert_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x, y + 1.1f - 1.5f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (convert_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 1.0f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 0.3f;
                graphics::drawSector(x, y + 1.1f - 1.5f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x - 0.12f, y + 1.1f - 1.35f, 0.12f, std::to_string((int)convert_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 1.0f; br_char.fill_color[1] = 0.3f; br_char.fill_color[2] = 0.3f;
                graphics::drawDisk(x, y + 1.1f - 1.5f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(2)) {
                    graphics::drawText(x - 0.07f, y + 1.1f - 1.35f, 0.12f, "50", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x - 0.07f, y + 1.1f - 1.35f, 0.12f, "50", br_text);
                }
            }

            // Sickness button (right) - Green with progress bar
            graphics::drawRect(x + 0.5f, y + 1.1f - 1.3f, 0.4f, 0.4f, br_back);

            float sickness_cd = wizard->getSpellCooldown(1);
            if (sickness_cd > 0.0f) {
                br_char.fill_color[0] = 0.5f; br_char.fill_color[1] = 0.5f; br_char.fill_color[2] = 0.5f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                float cooldown_progress = 1.0f - (sickness_cd / 10.0f);
                float angle = 360.0f * cooldown_progress;

                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawSector(x + 0.5f, y + 1.1f - 1.3f, 0, 0.15f, 90, 90 + angle, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                graphics::drawText(x + 0.45f, y + 1.1f - 1.17f, 0.12f, std::to_string((int)sickness_cd + 1), br_text);
            }
            else {
                br_char.fill_color[0] = 0.3f; br_char.fill_color[1] = 1.0f; br_char.fill_color[2] = 0.3f;
                graphics::drawDisk(x + 0.5f, y + 1.1f - 1.3f, 0.15f, br_char);

                br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 1.0f; br_text.fill_color[2] = 1.0f;
                if (wizard->canCastSpell(1)) {
                    graphics::drawText(x + 0.47f, y + 1.1f - 1.17f, 0.12f, "30", br_text);
                }
                else {
                    br_text.fill_color[0] = 1.0f; br_text.fill_color[1] = 0.3f; br_text.fill_color[2] = 0.3f;
                    graphics::drawText(x + 0.47f, y + 1.1f - 1.17f, 0.12f, "30", br_text);
                }
            }
        }
    }
    else {
        // Regular upgrade button for other entities
        graphics::drawRect(x, y + 1.1f - 1.5f, 0.4f, 0.4f, br_back);
        graphics::drawSector(x, y + 1.1f - 1.65f, 0, 0.25f, 240, 300, br_char);
        br_back.outline_color[0] = 0.1f; br_back.outline_color[1] = 0.1f; br_back.outline_color[2] = 0.2f;
        graphics::drawRect(x, y + 1.1f - 1.4f, 0.3f, 0.08f, br_back);
        graphics::drawLine(x - 0.12f, y + 1.1f - 1.45f, x + 0.13f, y + 1.1f - 1.45f, br_text);
        graphics::drawText(x - 0.17f, y + 1.1f - 1.32f, 0.12f, std::to_string(cost), br_text);
        graphics::drawRect(x, y + 1.1f - 1.41f, 0.08f, 0.08f, br_char);
    }
}