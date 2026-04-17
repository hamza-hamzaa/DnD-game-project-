#include "enemy.h"

Enemy::Enemy(const QString& name,const QString& type,int health,int attackPower,int defense,int x,int y)
    : Character(name, health, attackPower, defense, x, y), type(type) {
}

QString Enemy::getType() const {
    return type;
}

// Short line you could show when an enemy acts (chase / attack); flavor by monster kind.
QString Enemy::decideActionText() const {
    if (type == "Goblin") {
        return name + " darts toward you.";
    }
    if (type == "Orc") {
        return name + " charges aggressively.";
    }
    if (type == "Skeleton") {
        return name + " shambles forward.";
    }
    return name + " moves in to attack.";
}

QString Enemy::attackText() const {
    return name + " attacks";
}

// Base attack from stats, plus a tiny bump for tougher types (orc / skeleton).
int Enemy::basicAttackDamage() const {
    int bonus = 0;
    if (type == "Orc") {
        bonus = 2;
    } else if (type == "Skeleton") {
        bonus = 1;
    }
    return attackPower + bonus;
}
