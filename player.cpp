#include "player.h"
#include <algorithm>

// Constructor: sets up the player with their race, combat style, stats, and starting position (0,0).
// Every player starts with 2 potions and 2 special ability charges.
Player::Player(const QString& name, const QString& race, const QString& style, int health, int attackPower, int defense)
    : Character(name, health, attackPower, defense, 0, 0), race(race), style(style), potions(2), specialCharges(2) {}

//  Basic identity getters
QString Player::getRace() const { return race; }   // for exampl "Dwarf", "Elf"
QString Player::getStyle() const { return style; } //  "Warrior", "Fire Mage"

// Whether the player is carrying the key needed to exit the current level.
bool Player::getHasLevelKey() const { return hasLevelKey; }
void Player::setHasLevelKey(bool value) { hasLevelKey = value; }

// How many potions and special charges the player has left.
int Player::getPotions() const { return potions; }
int Player::getSpecialCharges() const { return specialCharges; }

// Adds potions to the player's inventory (picked up from the map, for example).
void Player::addPotion(int count) {
    potions += count;
}

// Tries to use a potion. Heals 25 HP if the player has one.
// Writes a result message into `log` so the UI can display what happened.
// Returns false (and does nothing) if the player is out of potions.
bool Player::usePotion(QString& log) {
    if (potions <= 0) {
        log = name + " has no potions left.";
        return false;
    }
    --potions;
    heal(25); // heal() is inherited from Character and never exceeds the max HP cap
    log = name + " used a potion and restored 25 HP.";
    return true;
}

// Flavor text for the combat log when the player does a basic attack.
QString Player::attackText() const {
    return name + " attacks";
}

// RACIAL ABILITY SYSTEM
// The racial ability charges up as the player moves around the map.
// Once enough moves have been made, the ability becomes available to use.

// Sets how many moves are needed to charge the racial ability.
// Minimum is 3 — we enforce that with std::max so nobody can set it to 0 or 1.
void Player::setRacialMovePeriod(int n) {
    racialMovePeriod = std::max(3, n);
}

int Player::getRacialMovePeriod() const { return racialMovePeriod; }

// How many moves the player has made since the last racial ability use.
int Player::getMovesTowardRacial() const { return movesTowardRacial; }

// Called every time the player successfully moves to a new tile.
// Ticks the racial cooldown counter and also counts down any active Dwarf armor buff.
void Player::onSuccessfulMove() {
    ++movesTowardRacial; // one step closer to the racial ability being ready

    // If the Dwarf Stoneblood buff is active, count it down each move
    if (dwarfArmorTurnsRemaining > 0) {
        --dwarfArmorTurnsRemaining;
        // When it expires, reverse the defense bonus exactly
        if (dwarfArmorTurnsRemaining == 0 && dwarfArmorDefenseBonus > 0) {
            addDefense(-dwarfArmorDefenseBonus); // undo the bonus
            dwarfArmorDefenseBonus = 0;
        }
    }
}

// Returns true when the player has moved enough to use their racial ability.
bool Player::isRacialAbilityReady() const {
    return movesTowardRacial >= racialMovePeriod;
}

// Resets the move counter after the racial ability is used — starts charging again from 0.
void Player::resetRacialProgressAfterUse() {
    movesTowardRacial = 0;
}


// DWARF STONEBLOOD BUFF
// A temporary defense boost that wears off after a set number of moves.
// We store the bonus amount separately so we can cleanly remove it later.

int Player::getDwarfArmorTurnsRemaining() const { return dwarfArmorTurnsRemaining; }

// Applies the Stoneblood armor buff. If one is already active, it removes the old
// bonus first before applying the new one — no stacking, clean replacement.
void Player::applyDwarfStoneblood(int armorBonus, int turns) {
    if (dwarfArmorDefenseBonus > 0) {
        addDefense(-dwarfArmorDefenseBonus); // remove the old buff before applying a new one
    }
    dwarfArmorDefenseBonus = armorBonus;   // remember the bonus so we can undo it later
    dwarfArmorTurnsRemaining = turns;      // set the countdown
    addDefense(armorBonus);                // actually apply the defense increase
}

int Player::getDwarfArmorDefenseBonus() const { return dwarfArmorDefenseBonus; }


// SAVE/LOAD HELPERS
// These directly set internal state when loading a saved game,
// bypassing normal game logic since we're just restoring a snapshot.


void Player::setMovesTowardRacialForLoad(int v) {
    movesTowardRacial = v;
}

void Player::restoreDwarfBuffForLoad(int bonus, int turns) {
    dwarfArmorDefenseBonus = bonus;
    dwarfArmorTurnsRemaining = turns;
    // Note: the actual defense value is restored separately when loading stats
}

// WARRIOR
// High HP, solid defense, reliable damage. The tanky frontliner.
// Basic attack adds a flat +4. Special "Heavy Strike" hits much harder.

Warrior::Warrior(const QString& name, const QString& race)
    : Player(name, race, "Warrior", 125, 21, 6) {}

int Warrior::basicAttackDamage() const {
    return attackPower + 4; // slightly boosted over raw attackPower
}

// Heavy Strike: spends a charge for a big raw damage hit (bypasses nothing, just high number).
bool Warrior::useSpecial(Character& target, QString& log) {
    if (specialCharges <= 0) {
        log = name + " has no special charges left.";
        return false;
    }
    --specialCharges;
    const int damage = attackPower + 14; // significantly stronger than a basic attack
    target.takeDamage(damage);
    log = name + " used Heavy Strike for " + QString::number(damage) + " raw damage.";
    return true;
}



// FIRE MAGE
// Low HP and defense, but the highest attack power and biggest special hit.
// Glass cannon — dies fast, hits hardest.


FireMage::FireMage(const QString& name, const QString& race)
    : Player(name, race, "Fire Mage", 100, 25, 3) {}

int FireMage::basicAttackDamage() const {
    return attackPower + 6; // highest basic attack bonus of all three classes
}

// Fireball: the biggest special hit in the game. Expensive but devastating.
bool FireMage::useSpecial(Character& target, QString& log) {
    if (specialCharges <= 0) {
        log = name + " has no special charges left.";
        return false;
    }
    --specialCharges;
    const int damage = attackPower + 18; // highest special damage bonus
    target.takeDamage(damage);
    log = name + " cast Fireball for " + QString::number(damage) + " raw damage.";
    return true;
}


// ICE MAGE
// Middle ground between Warrior and Fire Mage. Decent HP, decent defense.
// Special "Frost Burst" does moderate damage and narratively slows the enemy.


IceMage::IceMage(const QString& name, const QString& race)
    : Player(name, race, "Ice Mage", 108, 23, 4) {}

int IceMage::basicAttackDamage() const {
    return attackPower + 5; // middle-tier bonus
}

// Frost Burst: moderate special damage. The "slowed" effect is noted in the log
// but its actual gameplay impact would be handled elsewhere in the combat system.
bool IceMage::useSpecial(Character& target, QString& log) {
    if (specialCharges <= 0) {
        log = name + " has no special charges left.";
        return false;
    }
    --specialCharges;
    const int damage = attackPower + 12; // lowest special bonus, but comes with a slow
    target.takeDamage(damage);
    log = name + " cast Frost Burst for " + QString::number(damage) + " raw damage and slowed the enemy.";
    return true;
}
