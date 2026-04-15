#include "character.h"


Character::Character(const QString& name, int health, int attackPower, int defense,int x,int y)
    : name(name), health(health), maxHealth(health), attackPower(attackPower), defense(defense),posX(x),posY(y) {}




void Character::takeDamage(int damage) {
    int reduced;
    if(damage-defense<=0){
        reduced=1;
    }
    else{
        reduced=damage-defense;
    }
    health -= reduced;
    if (health < 0) {
        health = 0;
    }
}

void Character::heal(int amount) {
    health += amount;
    if (health > maxHealth) {
        health = maxHealth;
    }
}

bool Character::isAlive() const {
    return health > 0;
}
void Character::setHealth(int amount){
    health=amount;
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
int Character::getRow(){return posX;}
int Character::getCol(){return posY;}
void Character::setPos(int x, int y){
    posX=x;
    posY=y;
}
