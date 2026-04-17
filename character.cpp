#include "character.h"


// Build a character with starting stats and position.
Character::Character(const QString& name, int health, int attackPower, int defense,int x,int y)
    : name(name), health(health), maxHealth(health), attackPower(attackPower), defense(defense),posX(x),posY(y) {}
// Apply incoming damage after defense is considered.
void Character::takeDamage(int damage) {
    int reduced;
    // Always take at least 1 damage so strong defense doesn't block everything.
    if(damage-defense<=0){
        reduced=1;
    }
    else{
        reduced=damage-defense;
    }
    health -= reduced;
    // Health should never go below zero.
    if (health < 0) {
        health = 0;
    }
}

// Restore health, but never above max health.
void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

// A character is alive as long as health is above zero.
bool Character::isAlive() const {
    return health > 0;
}
// Set health directly, still respecting max health limit.
void Character::setHealth(int amount){
    health=amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}
// Basic stat/name getters.
QString Character::getName() const {return name; }
int Character::getHealth() const { return health; }
int Character::getMaxHealth() const { return maxHealth; }
int Character::getAttackPower() const { return attackPower; }
int Character::getDefense() const { return defense; }
// Increase offensive/defensive stats.
void Character::addAttack(int amount) { attackPower += amount; }
void Character::addDefense(int amount) { defense += amount; }
// Position getters (row/column).
int Character::getRow(){return posX;}
int Character::getCol(){return posY;}
// Move the character to a new grid position.
void Character::setPos(int x, int y){
    posX=x;
    posY=y;
}
