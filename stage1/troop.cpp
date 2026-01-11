#include "troop.h"
#include <cmath>
#include <algorithm>
#include "sgg/graphics.h"

Troop::Troop(Entity* src, Entity* tgt, int amount, const std::vector<std::pair<float, float>>& path)
    : Node(path.empty() ? 0.0f : path[0].first,
        path.empty() ? 0.0f : path[0].second),
    source(src), target(tgt), health_amount(amount),
    waypoints(path), progress(0.0f), arrived(false) {

    speed = src->getAttackingSpeed() * 0.5f;

    if (waypoints.empty()) {
        arrived = true;
    }
}

bool Troop::contains(float mx, float my) const {
    float dx = mx - x;
    float dy = my - y;
    return (dx * dx + dy * dy) <= (0.2f * 0.2f);  // radius 0.2
}

std::pair<float, float> Troop::interpolatePosition(float t) {
    if (waypoints.size() < 2) return waypoints[0];

    t = std::max(0.0f, std::min(t, (float)(waypoints.size() - 1)));

    int segment = (int)t;
    float local_t = t - segment;

    int p0_idx = std::max(0, segment - 1);
    int p1_idx = segment;
    int p2_idx = std::min(segment + 1, (int)waypoints.size() - 1);
    int p3_idx = std::min(segment + 2, (int)waypoints.size() - 1);

    auto& p0 = waypoints[p0_idx];
    auto& p1 = waypoints[p1_idx];
    auto& p2 = waypoints[p2_idx];
    auto& p3 = waypoints[p3_idx];

    // Catmull-Rom spline formula
    float t2 = local_t * local_t;
    float t3 = t2 * local_t;

    float x = 0.5f * (
        (2.0f * p1.first) +
        (-p0.first + p2.first) * local_t +
        (2.0f * p0.first - 5.0f * p1.first + 4.0f * p2.first - p3.first) * t2 +
        (-p0.first + 3.0f * p1.first - 3.0f * p2.first + p3.first) * t3
        );

    float y = 0.5f * (
        (2.0f * p1.second) +
        (-p0.second + p2.second) * local_t +
        (2.0f * p0.second - 5.0f * p1.second + 4.0f * p2.second - p3.second) * t2 +
        (-p0.second + 3.0f * p1.second - 3.0f * p2.second + p3.second) * t3
        );

    return { x, y };
}

void Troop::update(float dt) {
    if (arrived || waypoints.empty()) return;

    float dt_sec = dt / 1000.0f;

    // Move along the curve
    progress += speed * dt_sec * 2.0f; // Adjust multiplier for desired speed

    if (progress >= waypoints.size() - 1) {
        arrived = true;
        x = waypoints.back().first;
        y = waypoints.back().second;
        return;
    }

    auto pos = interpolatePosition(progress);
    x = pos.first;
    y = pos.second;
}

void Troop::draw() {
    if (arrived) return;

    graphics::Brush br;
    if (source->getSide() == Side::PLAYER) {
        br.fill_color[0] = 0.3f; br.fill_color[1] = 0.3f; br.fill_color[2] = 1.0f;
    }
    else if (source->getSide() == Side::ENEMY) {
        br.fill_color[0] = 1.0f; br.fill_color[1] = 0.3f; br.fill_color[2] = 0.3f;
    }
    else {
        br.fill_color[0] = 0.6f; br.fill_color[1] = 0.6f; br.fill_color[2] = 0.6f;
    }

    graphics::drawDisk(x, y, 0.2f, br);

    graphics::Brush txt;
    txt.fill_color[0] = txt.fill_color[1] = txt.fill_color[2] = 1.0f;
    graphics::drawText(x - 0.1f, y + 0.1f, 0.15f, std::to_string(health_amount), txt);
}