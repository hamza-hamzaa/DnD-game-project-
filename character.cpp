#include "character.h"

// Store starting stats and place on the map (row/col live in posX / posY).
Character::Character(const QString& name, int health, int attackPower, int defense,int x,int y)
    : name(name), health(health), maxHealth(health), attackPower(attackPower), defense(defense),posX(x),posY(y) {}

// Turn a "rolled" damage number into real HP loss after armor (at least 1 if the hit was positive).
int Character::reducedDamageFrom(int rawDamage) const
{
    if (rawDamage <= 0) {
        return 0;
    }
    if (rawDamage - defense <= 0) {
        return 1;
    }
    return rawDamage - defense;
}

// Apply a hit: subtract reduced damage and clamp HP at zero.
void Character::takeDamage(int damage) {
    const int reduced = reducedDamageFrom(damage);
    health -= reduced;
    if (health < 0) {
        health = 0;
    }
}

// Add HP but never go above maxHealth.
void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

bool Character::isAlive() const {
    return health > 0;
}

// Force HP to a value (used when loading or restarting), still clamped 0..max.
void Character::setHealth(int amount){
    health = amount;
    if (health < 0) {
        health = 0;
    }
    if (health > maxHealth) {
        health = maxHealth;
    }
}

QString Character::getName() const {return name; }
int Character::getHealth() const { return health; }
int Character::getMaxHealth() const { return maxHealth; }
int Character::getAttackPower() const { return attackPower; }
int Character::getDefense() const { return defense; }

void Character::addAttack(int amount) { attackPower += amount; }
void Character::addDefense(int amount) { defense += amount; }

int Character::getRow() const { return posX; }
int Character::getCol() const { return posY; }

void Character::setPos(int x, int y){
    posX=x;
    posY=y;
}
