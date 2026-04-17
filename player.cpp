#include "player.h"

// Start at (0,0) with a couple of potions and special uses for the run.
Player::Player(const QString& name, const QString& race, const QString& style, int health, int attackPower, int defense)
    : Character(name, health, attackPower, defense,0,0), race(race), style(style), potions(2), specialCharges(2){}

QString Player::getRace() const { return race; }
QString Player::getStyle() const { return style; }
int Player::getPotions() const { return potions; }
int Player::getSpecialCharges() const { return specialCharges; }

void Player::addPotion(int count) {
    potions += count;
}

// Spend one potion for a fixed heal, or refuse and explain in log.
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
    const int rawDamage = attackPower + 14;
    const int applied = target.reducedDamageFrom(rawDamage);
    target.takeDamage(rawDamage);
    log = name + " used Heavy Strike for " + QString::number(applied) + " damage.";
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
    const int rawDamage = attackPower + 18;
    const int applied = target.reducedDamageFrom(rawDamage);
    target.takeDamage(rawDamage);
    log = name + " cast Fireball for " + QString::number(applied) + " damage.";
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
    const int rawDamage = attackPower + 12;
    const int applied = target.reducedDamageFrom(rawDamage);
    target.takeDamage(rawDamage);
    log = name + " cast Frost Burst for " + QString::number(applied) + " damage.";
    return true;
}
