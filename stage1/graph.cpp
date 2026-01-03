#include "graph.h"
#include "sgg/graphics.h"
#include <cmath>
#include <algorithm>

void EntityGraph::addEdge(Entity* from, Entity* to) {
    if (!from || !to || from == to) return;
    connections[from].push_back(to);
    connections[to].push_back(from);
}

void EntityGraph::calculatePaths(const std::vector<std::unique_ptr<Entity>>& entities) {
    paths.clear();

    for (const auto& pair : connections) {
        Entity* from = pair.first;
        for (Entity* to : pair.second) {
            // Calculate path from 'from' to 'to'
            PathData path_forward;

            float x = from->x;
            float y = from->y;
            path_forward.waypoints.push_back({ x, y });

            const float STEP = 0.05f;
            const int MAX_STEPS = 2000;

            for (int step = 0; step < MAX_STEPS; step++) {
                float dx = to->x - x;
                float dy = to->y - y;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < 0.2f) {
                    path_forward.waypoints.push_back({ to->x, to->y });
                    break;
                }

                dx /= dist;
                dy /= dist;

                // Push away from obstacles
                for (const auto& entity : entities) {
                    if (entity.get() == from || entity.get() == to) continue;

                    float ex = x - entity->x;
                    float ey = y - entity->y;
                    float e_dist = std::sqrt(ex * ex + ey * ey);
                    float safe_dist = entity->size + 0.5f;

                    if (e_dist < safe_dist && e_dist > 0.01f) {
                        float push = (safe_dist - e_dist) / safe_dist * 4.0f;
                        dx += (ex / e_dist) * push;
                        dy += (ey / e_dist) * push;
                    }
                }

                // Normalize
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.01f) {
                    dx /= len;
                    dy /= len;
                }
                else {
                    dx = 0.7f;
                    dy = 0.3f;
                }

                x += dx * STEP;
                y += dy * STEP;

                // Sample waypoints
                if (path_forward.waypoints.size() == 1 ||
                    std::sqrt(std::pow(x - path_forward.waypoints.back().first, 2) +
                        std::pow(y - path_forward.waypoints.back().second, 2)) > 0.15f) {
                    path_forward.waypoints.push_back({ x, y });
                }
            }

            // Store the forward path
            paths[{from, to}] = path_forward;
        }
    }
}

const PathData* EntityGraph::getPath(Entity* from, Entity* to) const {
    auto it = paths.find({ from, to });
    return (it != paths.end()) ? &it->second : nullptr;
}

void EntityGraph::draw() const {
    graphics::Brush br;
    br.outline_opacity = 0.4f;
    br.outline_width = 2.0f;
    br.fill_opacity = 0.0f;

    // Draw all stored paths
    for (const auto& path_entry : paths) {
        Entity* from = path_entry.first.first;
        Entity* to = path_entry.first.second;
        const PathData& path = path_entry.second;

        br.outline_color[0] = (from->side == to->side) ? 0.2f : 0.8f;
        br.outline_color[1] = 0.8f;
        br.outline_color[2] = (from->side == to->side) ? 0.2f : 0.2f;

        if (path.waypoints.size() >= 2) {
            for (size_t i = 0; i < path.waypoints.size() - 1; i++) {
                graphics::drawLine(
                    path.waypoints[i].first, path.waypoints[i].second,
                    path.waypoints[i + 1].first, path.waypoints[i + 1].second,
                    br
                );
            }
        }
    }
}

void EntityGraph::clear() {
    connections.clear();
    paths.clear();
}