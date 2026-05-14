#include "character.h"

//character is the baseclass from which player and enemy enherit from
// Constructor: sets up a character with all their starting stats and position on the map.
// maxHealth is saved separately so we always know the original HP cap for healing later.
Character::Character(const QString& name, int health, int attackPower, int defense, int x, int y)
    : name(name), health(health), maxHealth(health), attackPower(attackPower), defense(defense), posX(x), posY(y) {}

// Called whenever this character gets hit.
// Defense reduces the incoming damage, but we always deal at least 1 — nobody is fully immune.
void Character::takeDamage(int damage) {
    int reduced;
    if (damage - defense <= 0) {
        reduced = 1; // minimum 1 damage, even against the greatest defence
    } else {
        reduced = damage - defense; // normal case: subtract defense from the hit
    }
    health -= reduced;
    if (health < 0) {
        health = 0; // health can't go negative, least is it at 0 so it never becomes negative
    }
}

// Restores HP by the given amount, but never beyond the original max.
void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth; // no overhealing above the maxhealth allowed
    }
}

// Returns true if the character still has HP left. Used to check for death after combat.
bool Character::isAlive() const {
    return health > 0;
}

// Directly sets health to a specific value (used for loading saves or special effects).
// Still considers not going above  the max health.
void Character::setHealth(int amount) {
    health = amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

// Getters: read-only access to private stats

QString Character::getName() const { return name; }
int Character::getHealth() const { return health; }
int Character::getMaxHealth() const { return maxHealth; }
int Character::getAttackPower() const { return attackPower; }
int Character::getDefense() const { return defense; }

// Stat modifiers: used by buffs, level-ups, and equipment

void Character::addAttack(int amount) { attackPower += amount; }   // adds to current attack (can be negative to debuff)
void Character::addDefense(int amount) { defense += amount; }      // adds to current defense (also used to remove buffs with negative values)
void Character::setAttackPower(int value) { attackPower = value; } // hard-sets attack to an exact value
void Character::setDefense(int value) { defense = value; }         // hard-sets defense to an exact value

// --- Position on the map ---

int Character::getRow() const { return posX; } // row in the grid (x = vertical)
int Character::getCol() const { return posY; } // column in the grid (y = horizontal)

// Moves the character to a new tile on the map.
void Character::setPos(int x, int y) {
    posX = x;
    posY = y;
}
