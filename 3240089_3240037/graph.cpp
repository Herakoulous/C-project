#include "graph.h"
#include "sgg/graphics.h"
#include <cmath>
#include <algorithm>

//Προσθήκη αμφίδρομης σύνδεσης
void EntityGraph::addEdge(Entity* from, Entity* to) {
    if (!from || !to || from == to) return;
    connections[from].push_back(to);
    connections[to].push_back(from);
}

//Υπολογισμός μονοπατιών για όλες τις συνδέσεις
void EntityGraph::calculatePaths(const std::vector<std::unique_ptr<Entity>>& entities) {
    paths.clear();

    for (const auto& pair : connections) {
        Entity* from = pair.first;
        for (Entity* to : pair.second) {
            PathData path_forward;

            //Αρχικό σημείο
            float x = from->getX();
            float y = from->getY();
            path_forward.waypoints.push_back({ x, y });

            const float STEP = 0.05f; //Βήμα κίνησης
			const int MAX_STEPS = 2000; //Μέγιστος αριθμός βημάτων

            for (int step = 0; step < MAX_STEPS; step++) {
				//Υπολογισμός κατεύθυνσης προς τον στοχο
                float dx = to->getX() - x;
                float dy = to->getY() - y;
                float dist = std::sqrt(dx * dx + dy * dy);

                //Έλεγχος άφιξης στον στόχο
                if (dist < 0.2f) {
                    path_forward.waypoints.push_back({ to->getX(), to->getY() });
                    break;
                }

                dx /= dist;
                dy /= dist;

				//Αποφυγή σύγκρουσης με άλλα entities
                for (const auto& entity : entities) {
                    if (entity.get() == from || entity.get() == to) continue;

                    float ex = x - entity->getX();
                    float ey = y - entity->getY();
                    float e_dist = std::sqrt(ex * ex + ey * ey);
                    float safe_dist = entity->getSize() + 0.5f;

                    if (e_dist < safe_dist && e_dist > 0.01f) {
                        float push = (safe_dist - e_dist) / safe_dist * 4.0f;
                        dx += (ex / e_dist) * push;
                        dy += (ey / e_dist) * push;
                    }
                }

				//Κανονικοποίηση διανύσματος κατεύθυνσης
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

                //Αποθήκευση waypoint ανα διαστήματα
                if (path_forward.waypoints.size() == 1 ||
                    std::sqrt(std::pow(x - path_forward.waypoints.back().first, 2) +
                        std::pow(y - path_forward.waypoints.back().second, 2)) > 0.15f) {
                    path_forward.waypoints.push_back({ x, y });
                }
            }

			//Αποθήκευση μονοπατιού
            paths[{from, to}] = path_forward;
        }
    }
}

//Getter που επιστρεφει το μονοπατι μεταξύ 2 entities
const PathData* EntityGraph::getPath(Entity* from, Entity* to) const {
    auto it = paths.find({ from, to });
    return (it != paths.end()) ? &it->second : nullptr;
}

void EntityGraph::draw() const {

}

//Καθαρισμός γράφου
void EntityGraph::clear() {
    connections.clear();
    paths.clear();
}