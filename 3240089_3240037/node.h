#pragma once


//Αφηρημένη κλάση για κόμβους γραφικων, που κληρονομείται από άλλες κλάσεις (Troop και Entity)
class Node {
protected:
	float x, y; //Συντεταγμένες κόμβου

public:

	//Δήλωση Constructor και Destructor
    Node(float x = 0.0f, float y = 0.0f);
    virtual ~Node() = default;

    //Eνημέρωση κατάστασης και σχεδίαση κόμβου
    virtual void update(float dt) = 0;
    virtual void draw() = 0;

	//Έλεγχος αν το (mx, my) βρίσκεται εντός του κόμβου
    virtual bool contains(float mx, float my) const;

    // Getters συντεταγμενων
    float getX() const;
    float getY() const;

    // Setter θεσης
    void setPosition(float new_x, float new_y);
};