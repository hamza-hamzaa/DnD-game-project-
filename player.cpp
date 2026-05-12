#include "player.h"
#include <algorithm>

Player::Player(const QString& name, const QString& race, const QString& style, int health, int attackPower, int defense)
    : Character(name, health, attackPower, defense,0,0), race(race), style(style), potions(2), specialCharges(2){}
QString Player::getRace() const { return race; }
QString Player::getStyle() const { return style; }
bool Player::getHasLevelKey() const { return hasLevelKey; }
void Player::setHasLevelKey(bool value) { hasLevelKey = value; }
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

void Player::setRacialMovePeriod(int n)
{
    racialMovePeriod = std::max(3, n);
}

int Player::getRacialMovePeriod() const
{
    return racialMovePeriod;
}

int Player::getMovesTowardRacial() const
{
    return movesTowardRacial;
}

void Player::onSuccessfulMove()
{
    ++movesTowardRacial;
    if (dwarfArmorTurnsRemaining > 0) {
        --dwarfArmorTurnsRemaining;
        if (dwarfArmorTurnsRemaining == 0 && dwarfArmorDefenseBonus > 0) {
            addDefense(-dwarfArmorDefenseBonus);
            dwarfArmorDefenseBonus = 0;
        }
    }
}

bool Player::isRacialAbilityReady() const
{
    return movesTowardRacial >= racialMovePeriod;
}

void Player::resetRacialProgressAfterUse()
{
    movesTowardRacial = 0;
}

int Player::getDwarfArmorTurnsRemaining() const
{
    return dwarfArmorTurnsRemaining;
}

void Player::applyDwarfStoneblood(int armorBonus, int turns)
{
    if (dwarfArmorDefenseBonus > 0) {
        addDefense(-dwarfArmorDefenseBonus);
    }
    dwarfArmorDefenseBonus = armorBonus;
    dwarfArmorTurnsRemaining = turns;
    addDefense(armorBonus);
}

int Player::getDwarfArmorDefenseBonus() const
{
    return dwarfArmorDefenseBonus;
}

void Player::setMovesTowardRacialForLoad(int v)
{
    movesTowardRacial = v;
}

void Player::restoreDwarfBuffForLoad(int bonus, int turns)
{
    dwarfArmorDefenseBonus = bonus;
    dwarfArmorTurnsRemaining = turns;
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
