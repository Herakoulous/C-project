#pragma once

class Node {
protected:
    float x, y;

public:
    Node(float x = 0.0f, float y = 0.0f);
    virtual ~Node() = default;

    virtual void update(float dt) = 0;
    virtual void draw() = 0;

    // Virtual function for collision detection
    virtual bool contains(float mx, float my) const;

    // Getters
    float getX() const;
    float getY() const;

    // Setters
    void setPosition(float new_x, float new_y);
};
