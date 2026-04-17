#include "enemy.h"

Enemy::Enemy(const QString& name,const QString& type,int health,int attackPower,int defense,int x,int y)
    : Character(name, health, attackPower, defense, x, y), type(type) {
}

QString Enemy::getType() const {
    return type;
}

QString Enemy::decideActionText() const {
    return name + " attacks automatically.";
}

QString Enemy::attackText() const {
    return name + " attacks";
}
int Enemy::basicAttackDamage() const {
    return attackPower;
}