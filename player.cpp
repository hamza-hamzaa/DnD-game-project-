#include "player.h"

Player::Player(const QString& name, const QString& race, const QString& style, int health, int attackPower, int defense)
    : Character(name, health, attackPower, defense,0,0), race(race), style(style), potions(2), specialCharges(2){}
QString Player::getRace() const { return race; }
QString Player::getStyle() const { return style; }
int Player::getPotions() const { return potions; }
int Player::getSpecialCharges() const { return specialCharges; }

void Player::addPotion(int count) {
    potions += count;
}

bool Player::usePotion(QString& log) {
    if (potions <= 0) {
        log = name + " has no potions left.";
        return false;
    }
    --potions;
    heal(25);
    log = name + " used a potion and restored 25 HP.";
    return true;
}

QString Player::attackText() const {
    return name + " attacks";
}







Warrior::Warrior(const QString& name, const QString& race)
    : Player(name, race, "Warrior", 125, 21, 6) {}

int Warrior::basicAttackDamage() const {
    return attackPower + 4;
}

bool Warrior::useSpecial(Character& target, QString& log) {
    if (specialCharges <= 0) {
        log = name + " has no special charges left.";
        return false;
    }
    --specialCharges;
    const int damage = attackPower + 14;
    target.takeDamage(damage);
    log = name + " used Heavy Strike for " + QString::number(damage) + " raw damage.";
    return true;
}

FireMage::FireMage(const QString& name, const QString& race)
    : Player(name, race, "Fire Mage", 100, 25, 3) {}

int FireMage::basicAttackDamage() const {
    return attackPower + 6;
}

bool FireMage::useSpecial(Character& target, QString& log) {
    if (specialCharges <= 0) {
        log = name + " has no special charges left.";
        return false;
    }
    --specialCharges;
    const int damage = attackPower + 18;
    target.takeDamage(damage);
    log = name + " cast Fireball for " + QString::number(damage) + " raw damage.";
    return true;
}

IceMage::IceMage(const QString& name, const QString& race)
    : Player(name, race, "Ice Mage", 108, 23, 4) {}

int IceMage::basicAttackDamage() const {
    return attackPower + 5;
}

bool IceMage::useSpecial(Character& target, QString& log) {
    if (specialCharges <= 0) {
        log = name + " has no special charges left.";
        return false;
    }
    --specialCharges;
    const int damage = attackPower + 12;
    target.takeDamage(damage);
    log = name + " cast Frost Burst for " + QString::number(damage) + " raw damage and slowed the enemy.";
    return true;
}
