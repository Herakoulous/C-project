#include "node.h"

//Constructor (αρχικοποιεί τις συντεταγμένες του κόμβου)
Node::Node(float x, float y) : x(x), y(y) {}

//Default υλοποιηση που επιστρεφει πάντα false
//Οι υποκλάσεις θα κάνουν override
bool Node::contains(float mx, float my) const {
    return false;
}

//Getters συντεταγμενων
float Node::getX() const { return x; }
float Node::getY() const { return y; }

//Setter θέσης κόμβου
void Node::setPosition(float new_x, float new_y) {
    x = new_x;
    y = new_y;
}