#pragma once
#include "entity.h"
#include <vector>
#include <map>
#include <memory>

//Δομή για αποθήκευση μονοπατιών (waypoints)
struct PathData {
    std::vector<std::pair<float, float>> waypoints;
};

//Κλαση για διαχείριση γράφου συνδέσεων μεταξύ οντοτήτων
//Δημιουργεί και αποθηκεύει μονοπάτια για μετακίνηση troops
class EntityGraph {
private:
	std::map<Entity*, std::vector<Entity*>> connections; //Χάρτης συνδέσεων entities
	std::map<std::pair<Entity*, Entity*>, PathData> paths; //Χάρτης μονοπατιών μεταξύ των συνδεδεμένων entities
public:
    void addEdge(Entity* from, Entity* to); //Προσθηκη αμφίδρομης σύνδεσης μεταξύ 2 entities
	void calculatePaths(const std::vector<std::unique_ptr<Entity>>& entities); //Υπολογισμός μονοπατιών 
    const PathData* getPath(Entity* from, Entity* to) const; //Getter για επιστροφή μονοπατιου 
	void clear(); //Καθαρισμός γράφου
	void draw() const;

	//Getter για τις συνδέσεις ενός entity
    const std::vector<Entity*>& getConnections(Entity* entity) const {
        static const std::vector<Entity*> empty;
        auto it = connections.find(entity);
        if (it != connections.end()) {
            return it->second;
        }
        return empty;
    }
};

