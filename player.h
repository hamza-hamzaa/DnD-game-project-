#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"

// Human-controlled hero: race + class (style), inventory potions, and limited special moves.
class Player : public Character {
protected:
    QString race;
    QString style;
    int potions;
    int specialCharges;


public:
    Player(const QString& name, const QString& race, const QString& style, int health, int attackPower, int defense = 0);
    ~Player() override= default;

    QString getRace() const;
    QString getStyle() const;
    int getPotions() const;
    int getSpecialCharges() const;

    void addPotion(int count = 1);
    bool usePotion(QString& log);

    // Each class implements its own big attack (Heavy Strike, Fireball, etc.).
    virtual bool useSpecial(Character& target, QString& log)=0;

    QString attackText() const override;
};

// Tanky melee: higher HP/defense, strong basic hits, burst special.
class Warrior : public Player {
public:
    Warrior(const QString& name, const QString& race);
    int basicAttackDamage() const override;
    bool useSpecial(Character& target, QString& log) override;
};

// Glass cannon magic: high burst on basic and special.
class FireMage : public Player {
public:
    FireMage(const QString& name, const QString& race);
    int basicAttackDamage() const override;
    bool useSpecial(Character& target, QString& log) override;
};

// Balanced mage: slightly tougher than fire, special is a bit cheaper in power.
class IceMage : public Player {
public:
    IceMage(const QString& name, const QString& race);
    int basicAttackDamage() const override;
    bool useSpecial(Character& target, QString& log) override;
};

#endif
