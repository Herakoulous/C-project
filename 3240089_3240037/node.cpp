#include "node.h"

Node::Node(float x, float y) : x(x), y(y) {}

bool Node::contains(float mx, float my) const {
    return false;
}

float Node::getX() const { return x; }
float Node::getY() const { return y; }

void Node::setPosition(float new_x, float new_y) {
    x = new_x;
    y = new_y;
}