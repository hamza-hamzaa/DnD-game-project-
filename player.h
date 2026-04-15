#ifndef PLAYER_H
#define PLAYER_H

#include "character.h"
#include <QStringList>

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

    virtual bool useSpecial(Character& target, QString& log)=0;

    QString attackText() const override;
};
class Warrior : public Player {public:
    Warrior(const QString& name, const QString& race);
    int basicAttackDamage() const override;
    bool useSpecial(Character& target, QString& log) override;
};

class FireMage : public Player {
public:
    FireMage(const QString& name, const QString& race);
    int basicAttackDamage() const override;
    bool useSpecial(Character& target, QString& log) override;
};

class IceMage : public Player {
public:
    IceMage(const QString& name, const QString& race);
    int basicAttackDamage() const override;
    bool useSpecial(Character& target, QString& log) override;
};

#endif