#include "enemy.h"

// Constructor: builds an enemy with a name, a type label (e.g. "Goblin", "Troll"),
// their combat stats, and their starting position on the map.
// Everything except type is passed up to Character via the initializer list.
Enemy::Enemy(const QString& name, const QString& type, int health, int attackPower, int defense, int x, int y)
    : Character(name, health, attackPower, defense, x, y), type(type) {
}

// Returns the enemy's type string — used by the UI to display what kind of enemy this is.
QString Enemy::getType() const {
    return type;
}

// Returns a text line describing that the enemy is acting this turn.
// Enemies don't make decisions — they always attack automatically.
QString Enemy::decideActionText() const {
    return name + " attacks automatically.";
}

// Returns the attack flavor text shown in the combat log.
QString Enemy::attackText() const {
    return name + " attacks";
}

// Enemies deal straight attackPower with no bonus — simpler than player classes on purpose.
int Enemy::basicAttackDamage() const {
    return attackPower;
}